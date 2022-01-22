#ifndef __EXPRESSIONTOKENIZER_HPP__
#define __EXPRESSIONTOKENIZER_HPP__

#include <string>
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <sstream>
#include "text/Parser.hpp"
#include "Token.hpp"
#include "SyntaxException.hpp"

template<class... Ts>
class ExpressionTokenizer : public Parser
{
  public:
  using ValueType          = ValueToken<Ts...>;
  using UnaryOperatorType  = UnaryOperatorToken<ValueType*>;
  using BinaryOperatorType = BinaryOperatorToken<ValueType*>;
  using FunctionType       = FunctionToken<ValueType*>;
  using VariableType       = VariableToken<Ts...>;
  using MiscType           = GenericToken<char>;

  constexpr static char DefaultCommentIdentifier = '#';

  void SetNumberConverter(const std::function<ValueType*(const std::string&)>& value) { m_NumberConverter = value; }
  void SetOnUnknownIdentifierCallback(const std::function<ValueType*(const std::string&)>& value) { m_OnUnknownIdentifierCallback = value; }

  const char& GetCommentIdentifier() const { return m_CommentIdentifier; }
  void SetCommentIdentifier(char value) { m_CommentIdentifier = value; }

  ExpressionTokenizer(const std::function<ValueType*(const std::string&)>& numberConverter)
      : Parser()
      , m_NumberConverter(numberConverter)
      , m_CommentIdentifier(ExpressionTokenizer<Ts...>::DefaultCommentIdentifier)
      , m_TokenCache()
  {}

  ExpressionTokenizer(const ExpressionTokenizer<Ts...>& other)
      : Parser(other)
      , m_NumberConverter(other.m_NumberConverter)
      , m_CommentIdentifier(other.m_CommentIdentifier)
      , m_TokenCache(other.m_TokenCache)
  {}

  ExpressionTokenizer(ExpressionTokenizer<Ts...>&& other)
      : Parser(std::move(other))
      , m_NumberConverter(std::move(other.m_NumberConverter))
      , m_CommentIdentifier(std::move(other.m_CommentIdentifier))
      , m_TokenCache(std::move(other.m_TokenCache))
  {}

  virtual ~ExpressionTokenizer() override = default;

  std::queue<IToken*> Execute(const std::string& expression,
                              const std::unordered_map<char, UnaryOperatorType*>* unaryOperators,
                              const std::unordered_map<std::string, BinaryOperatorType*>* binaryOperators,
                              const std::unordered_map<std::string, VariableType*>* variables,
                              const std::unordered_map<std::string, FunctionType*>* functions)
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
        auto value = m_NumberConverter(ParseNumber());
        m_TokenCache.push_back(std::unique_ptr<IToken>(value));
        current = m_TokenCache.back().get();
      }
      else if(IsString(GetCurrent()))
      {
        std::string value = ParseString();
        m_TokenCache.push_back(std::unique_ptr<IToken>(new ValueType(value)));
        current = m_TokenCache.back().get();
      }
      else if(unOps.find(GetCurrent()) != unOps.end() || binOps.find(GetCurrent()) != binOps.end())
      {
        MiscType* misc;
        if(hasUnOps && (result.empty() || result.back()->IsType<OperatorToken>() ||
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

        typename std::unordered_map<std::string, FunctionType*>::const_iterator functionIter;
        typename std::unordered_map<std::string, VariableType*>::const_iterator variableIter;
        if(functions != nullptr && (functionIter = functions->find(identifier)) != functions->cend())
        {
          current = functionIter->second;

          Next(Parser::IsWhitespace);
          if(GetCurrent() != '(')
          {
            throw SyntaxException("Expected function opening parenthesis: " + functionIter->second->GetIdentifier(), GetIndex());
          }
        }
        else if(variables != nullptr && (variableIter = variables->find(identifier)) != variables->cend())
        {
          current = variableIter->second;
        }
        else
        {
          if(m_OnUnknownIdentifierCallback == nullptr)
          {
            throw SyntaxException("Unkown identifier: " + identifier, GetIndex() - identifier.length());
          }

          auto value = m_OnUnknownIdentifierCallback(identifier);
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

      result.push(current);
    }

    return result;
  }

  private:
  using Parser::SetText;

  std::function<ValueType*(const std::string&)> m_NumberConverter;
  std::function<ValueType*(const std::string&)> m_OnUnknownIdentifierCallback;
  char m_CommentIdentifier;

  std::vector<std::unique_ptr<IToken>> m_TokenCache;
};

#endif // __EXPRESSIONTOKENIZER_HPP__
