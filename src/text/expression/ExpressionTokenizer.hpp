#ifndef __EXPRESSIONTOKENIZER_HPP__
#define __EXPRESSIONTOKENIZER_HPP__

#include <string>
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "text/Parser.hpp"
#include "Token.hpp"
#include "SyntaxException.hpp"

class ExpressionTokenizer : public Parser
{
  public:
  using MiscType = GenericToken<char>;

  constexpr static char DefaultCommentIdentifier = '#';

  void SetOnParseNumberCallback(const std::function<IValueToken*(const std::string&)>& value) { m_OnParseNumberCallback = value; }
  void SetOnParseStringCallback(const std::function<IValueToken*(const std::string&)>& value) { m_OnParseStringCallback = value; }
  void SetOnUnknownIdentifierCallback(const std::function<IValueToken*(const std::string&)>& value) { m_OnParseUnknownIdentifier = value; }
  void SetJuxtapositionOperator(IBinaryOperatorToken* value) { m_pJuxtapositionOperator = value; }

  const char& GetCommentIdentifier() const { return m_CommentIdentifier; }
  void SetCommentIdentifier(char value) { m_CommentIdentifier = value; }

  ExpressionTokenizer()
      : Parser()
      , m_OnParseNumberCallback()
      , m_OnParseStringCallback()
      , m_OnParseUnknownIdentifier()
      , m_pJuxtapositionOperator()
      , m_CommentIdentifier(ExpressionTokenizer::DefaultCommentIdentifier)
      , m_TokenCache()
  {}

  ExpressionTokenizer(const ExpressionTokenizer& other)
      : Parser(other)
      , m_OnParseNumberCallback(other.m_OnParseNumberCallback)
      , m_OnParseStringCallback(other.m_OnParseStringCallback)
      , m_OnParseUnknownIdentifier(other.m_OnParseUnknownIdentifier)
      , m_pJuxtapositionOperator(other.m_pJuxtapositionOperator)
      , m_CommentIdentifier(other.m_CommentIdentifier)
      , m_TokenCache()
  {}

  ExpressionTokenizer(ExpressionTokenizer&& other)
      : Parser(std::move(other))
      , m_OnParseNumberCallback(std::move(other.m_OnParseNumberCallback))
      , m_OnParseStringCallback(std::move(other.m_OnParseStringCallback))
      , m_OnParseUnknownIdentifier(std::move(other.m_OnParseUnknownIdentifier))
      , m_pJuxtapositionOperator(std::move(other.m_pJuxtapositionOperator))
      , m_CommentIdentifier(std::move(other.m_CommentIdentifier))
      , m_TokenCache(std::move(other.m_TokenCache))
  {}

  virtual ~ExpressionTokenizer() override = default;

  std::queue<IToken*> Execute(const std::string& expression,
                              const std::unordered_map<char, IUnaryOperatorToken*>* unaryOperators,
                              const std::unordered_map<std::string, IBinaryOperatorToken*>* binaryOperators,
                              const std::unordered_map<std::string, IVariableToken*>* variables,
                              const std::unordered_map<std::string, IFunctionToken*>* functions)
  {
    m_TokenCache.clear();
    this->SetText(expression);

    std::unordered_set<char> unOps;
    std::unordered_set<char> binOps;

    const bool hasUnOps  = unaryOperators != nullptr;
    const bool hasBinOps = binaryOperators != nullptr;
    if(hasUnOps)
    {
      std::transform(unaryOperators->begin(), unaryOperators->end(), std::inserter(unOps, unOps.end()), [](const auto& pair) { return pair.first; });
    }

    if(hasBinOps)
    {
      for(const auto& i : *binaryOperators)
      {
        std::transform(i.first.begin(), i.first.end(), std::inserter(binOps, binOps.end()), [](const auto& ch) { return ch; });
      }
    }

    IToken* current = nullptr;
    std::queue<IToken*> result;
    while(GetState())
    {
      Next(Parser::IsWhitespace);

      if(!GetState() || GetCurrent() == m_CommentIdentifier)
      {
        break;
      }
      else if(IsNumber(GetCurrent()))
      {
        std::string stringValue = ParseNumber();
        if(m_OnParseNumberCallback == nullptr)
        {
          throw SyntaxException("Unhandled numeric token: " + stringValue, GetIndex() - stringValue.length());
        }

        auto value = m_OnParseNumberCallback(stringValue);
        m_TokenCache.push_back(std::unique_ptr<IToken>(value));
        current = m_TokenCache.back().get();
      }
      else if(IsString(GetCurrent()))
      {
        std::string stringValue = ParseString();
        if(m_OnParseStringCallback == nullptr)
        {
          throw SyntaxException("Unhandled string token: " + stringValue, GetIndex() - stringValue.length());
        }

        auto value = m_OnParseStringCallback(stringValue);
        m_TokenCache.push_back(std::unique_ptr<IToken>(value));
        current = m_TokenCache.back().get();
      }
      else if(unOps.find(GetCurrent()) != unOps.end() || binOps.find(GetCurrent()) != binOps.end())
      {
        MiscType* misc;
        if(hasUnOps && (result.empty() || result.back()->IsType<IOperatorToken>() ||
                        ((misc = result.back()->AsPointer<MiscType>()) != nullptr && (misc->GetObject() == '(' || misc->GetObject() == ','))))
        {
          const auto iter = unaryOperators->find(GetCurrent());
          if(iter == unaryOperators->cend())
          {
            throw SyntaxException("Unknown unary operator: " + GetCurrent(), GetIndex());
          }

          current = iter->second;
          Next();
        }
        else if(hasBinOps)
        {
          std::string identifier = Get(1u);
          identifier += Get([unOps, binOps](char c) { return binOps.find(c) != binOps.end() && unOps.find(c) == unOps.end(); });

          const auto iter = binaryOperators->find(identifier);
          if(iter == binaryOperators->cend())
          {
            throw SyntaxException("Unknown binary operator: " + identifier, GetIndex() - identifier.length());
          }

          current = iter->second;
        }
        else
        {
          throw SyntaxException("Unknown operator: " + GetCurrent(), GetIndex());
        }
      }
      else if(IsIdentifier(GetCurrent()))
      {
        std::string identifier = ParseIdentifier();

        typename std::unordered_map<std::string, IFunctionToken*>::const_iterator functionIter;
        typename std::unordered_map<std::string, IVariableToken*>::const_iterator variableIter;
        if(functions != nullptr && (functionIter = functions->find(identifier)) != functions->cend())
        {
          current = functionIter->second;

          Next(Parser::IsWhitespace);
          if(GetCurrent() != '(')
          {
            throw SyntaxException("Expected function opening parenthesis: " + functionIter->second->GetIdentifier(), GetIndex() - identifier.length());
          }
        }
        else if(variables != nullptr && (variableIter = variables->find(identifier)) != variables->cend())
        {
          current = variableIter->second;
        }
        else
        {
          if(m_OnParseUnknownIdentifier == nullptr)
          {
            throw SyntaxException("Unkown identifier: " + identifier, GetIndex() - identifier.length());
          }

          auto value = m_OnParseUnknownIdentifier(identifier);
          if(value == nullptr)
          {
            throw SyntaxException("Invalid identifier: " + identifier, GetIndex() - identifier.length());
          }

          current = value;
        }
      }
      else if(GetCurrent() == '(' || GetCurrent() == ')' || GetCurrent() == ',')
      {
        m_TokenCache.push_back(std::unique_ptr<IToken>(new MiscType(GetCurrent())));
        current = m_TokenCache.back().get();
        Next();
      }
      else
      {
        throw SyntaxException("Unknown token: " + GetCurrent(), GetIndex());
      }

      if(m_pJuxtapositionOperator != nullptr && !result.empty())
      {
        auto previous = result.back();
        MiscType* misc;

        bool previousIsValue            = previous->IsType<IValueToken>();
        bool previousIsRightParenthesis = (misc = previous->AsPointer<MiscType>()) != nullptr && misc->GetObject() == ')';
        bool currentIsLeftParenthesis   = (misc = current->AsPointer<MiscType>()) && misc->GetObject() == '(';

        if((current->IsType<IValueToken>() && (previousIsRightParenthesis || (current->IsType<IVariableToken>() && previousIsValue))) ||
           (current->IsType<IFunctionToken>() && (previousIsRightParenthesis || previousIsValue)) || (currentIsLeftParenthesis && previousIsValue))
        {
          result.push(m_pJuxtapositionOperator);
        }
      }

      result.push(current);
    }

    return result;
  }

  private:
  using Parser::SetText;

  std::function<IValueToken*(const std::string&)> m_OnParseNumberCallback;
  std::function<IValueToken*(const std::string&)> m_OnParseStringCallback;
  std::function<IValueToken*(const std::string&)> m_OnParseUnknownIdentifier;
  IBinaryOperatorToken* m_pJuxtapositionOperator;
  char m_CommentIdentifier;

  std::vector<std::unique_ptr<IToken>> m_TokenCache;
};

#endif // __EXPRESSIONTOKENIZER_HPP__
