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

  std::unordered_map<char, ExpressionTokenizer<Ts...>::UnaryOperatorType>& GetUnaryOperators() { return m_UnaryOperators; }
  std::unordered_map<std::string, ExpressionTokenizer<Ts...>::BinaryOperatorType>& GetBinaryOperators() { return m_BinaryOperators; }
  std::unordered_map<std::string, ExpressionTokenizer<Ts...>::FunctionType>& GetFunctions() { return m_Functions; }
  std::unordered_map<std::string, ExpressionTokenizer<Ts...>::VariableType>& GetConstants() { return m_Constants; }

  void AddUnaryOperator(typename UnaryOperatorType::CallbackType callback, char identifier, int precedence, Associativity associativity)
  {
    m_UnaryOperators[identifier] = UnaryOperatorType(callback, identifier, precedence, associativity);
  }

  void RemoveUnaryOperator(char identifier) { m_UnaryOperators.erase(identifier); }

  void AddBinaryOperator(typename BinaryOperatorType::CallbackType callback, const std::string& identifier, int precedence, Associativity associativity)
  {
    m_BinaryOperators[identifier] = BinaryOperatorType(callback, identifier, precedence, associativity);
  }

  void RemoveBinaryOperator(const std::string& identifier) { m_BinaryOperators.erase(identifier); }

  void AddFunction(typename FunctionType::CallbackType callback,
                   const std::string& identifier,
                   std::size_t argsMin = 0u,
                   std::size_t argsMax = FunctionType::GetArgumentCountMaxLimit())
  {
    m_Functions[identifier] = FunctionType(callback, identifier, argsMin, argsMax);
  }

  void RemoveFunction(const std::string& identifier) { m_Functions.erase(identifier); }

  template<class T>
  void AddConstant(const T& value, const std::string& identifier)
  {
    m_Constants[identifier] = VariableType(identifier, value);
  }

  void RemoveConstant(const std::string& identifier) { m_Constants.erase(identifier); }

  void SetNumberConverter(const std::function<ValueType*(const std::string&)>& value) { m_NumberConverter = value; }

  const char& GetCommentIdentifier() const { return m_CommentIdentifier; }
  void SetCommentIdentifier(char value) { m_CommentIdentifier = value; }

  ExpressionTokenizer(const std::function<ValueType*(const std::string&)>& numberConverter)
      : Parser()
      , m_UnaryOperators()
      , m_BinaryOperators()
      , m_Functions()
      , m_Constants()
      , m_NumberConverter(numberConverter)
      , m_CommentIdentifier(ExpressionTokenizer<Ts...>::DefaultCommentIdentifier)
      , m_TokenCache()
  {}

  ExpressionTokenizer(const ExpressionTokenizer<Ts...>& other)
      : Parser(other)
      , m_UnaryOperators(other.m_UnaryOperators)
      , m_BinaryOperators(other.m_BinaryOperators)
      , m_Functions(other.m_Functions)
      , m_Constants(other.m_Constants)
      , m_NumberConverter(other.m_NumberConverter)
      , m_CommentIdentifier(other.m_CommentIdentifier)
      , m_TokenCache(other.m_TokenCache)
  {}

  ExpressionTokenizer(ExpressionTokenizer<Ts...>&& other)
      : Parser(std::move(other))
      , m_UnaryOperators(std::move(other.m_UnaryOperators))
      , m_BinaryOperators(std::move(other.m_BinaryOperators))
      , m_Functions(std::move(other.m_Functions))
      , m_Constants(std::move(other.m_Constants))
      , m_NumberConverter(std::move(other.m_NumberConverter))
      , m_CommentIdentifier(std::move(other.m_CommentIdentifier))
      , m_TokenCache(std::move(other.m_TokenCache))
  {}

  virtual ~ExpressionTokenizer() override = default;

  std::queue<IToken*> Execute(const std::string& text,
                              const std::unordered_map<std::string, std::unique_ptr<VariableType>>& variables,
                              std::unordered_map<std::string, std::unique_ptr<VariableType>>& variableCache)
  {
    m_TokenCache.clear();
    this->SetText(text);

    std::unordered_set<char> unOps;
    std::unordered_set<char> binOps;
    std::transform(m_UnaryOperators.begin(), m_UnaryOperators.end(), std::inserter(unOps, unOps.end()), [](const auto& pair) { return pair.first; });
    for(const auto& i : m_BinaryOperators)
    {
      std::transform(i.first.begin(), i.first.end(), std::inserter(binOps, binOps.end()), [](const auto& ch) { return ch; });
    }

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
        result.push(m_TokenCache.back().get());
      }
      else if(IsString(GetCurrent()))
      {
        std::string value = ParseString();
        m_TokenCache.push_back(std::unique_ptr<IToken>(new ValueType(value)));
        result.push(m_TokenCache.back().get());
      }
      else if(unOps.find(GetCurrent()) != unOps.end() || binOps.find(GetCurrent()) != binOps.end())
      {
        MiscType* misc = nullptr;
        if(result.empty() || result.back()->IsType<OperatorToken>() ||
           ((misc = result.back()->AsPointer<MiscType>()) != nullptr && (misc->GetObject() == '(' || misc->GetObject() == ',')))
        {
          const auto op = m_UnaryOperators.find(GetCurrent());
          if(op == m_UnaryOperators.cend())
          {
            throw SyntaxException("Unknown unary operator: " + GetCurrent(), GetIndex());
          }

          result.push(&op->second);
          Next();
        }
        else
        {
          std::string identifier = Get(1u);
          identifier += Get([unOps, binOps](char c) { return binOps.find(c) != binOps.end() && unOps.find(c) == unOps.end(); });

          const auto op = m_BinaryOperators.find(identifier);
          if(op == m_BinaryOperators.cend())
          {
            throw SyntaxException("Unknown binary operator: " + identifier, GetIndex() - identifier.length());
          }

          result.push(&op->second);
        }
      }
      else if(IsIdentifier(GetCurrent()))
      {
        std::string identifier = ParseIdentifier();

        const auto function = m_Functions.find(identifier);
        if(function != m_Functions.cend())
        {
          result.push(&function->second);
        }
        else
        {
          const auto constant = m_Constants.find(identifier);
          if(constant != m_Constants.cend())
          {
            result.push(&constant->second);
          }
          else
          {
            const auto variable = variables.find(identifier);
            if(variable != variables.cend())
            {
              result.push(variable->second.get());
            }
            else
            {
              auto variableIterator = variableCache.find(identifier);
              if(variableIterator != variableCache.cend())
              {
                result.push(variableIterator->second.get());
              }
              else
              {
                auto newVariable = std::make_unique<VariableType>(identifier);
                result.push(newVariable.get());
                variableCache[identifier] = std::move(newVariable);
              }

              //throw SyntaxException("Unkown identifier: " + identifier, GetIndex() - identifier.length());
            }
          }
        }
      }
      else if(GetCurrent() == '(' || GetCurrent() == ')' || GetCurrent() == ',')
      {
        m_TokenCache.push_back(std::unique_ptr<IToken>(new MiscType(GetCurrent())));
        result.push(m_TokenCache.back().get());
        Next();
      }
      else
      {
        throw SyntaxException("Unknown token: " + GetCurrent(), GetIndex());
      }
    }

    return result;
  }

  private:
  using Parser::SetText;

  std::unordered_map<char, ExpressionTokenizer<Ts...>::UnaryOperatorType> m_UnaryOperators;
  std::unordered_map<std::string, ExpressionTokenizer<Ts...>::BinaryOperatorType> m_BinaryOperators;

  std::unordered_map<std::string, ExpressionTokenizer<Ts...>::FunctionType> m_Functions;
  std::unordered_map<std::string, ExpressionTokenizer<Ts...>::VariableType> m_Constants;

  std::function<ValueType*(const std::string&)> m_NumberConverter;
  char m_CommentIdentifier;

  std::vector<std::unique_ptr<IToken>> m_TokenCache;
};

#endif // __EXPRESSIONTOKENIZER_HPP__
