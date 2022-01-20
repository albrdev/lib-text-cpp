#ifndef __EXPRESSIONPARSER_HPP__
#define __EXPRESSIONPARSER_HPP__

#include <string>
#include <queue>
#include <unordered_map>
#include "ExpressionTokenizer.hpp"
#include "ExpressionPostfixParser.hpp"
#include "ExpressionEvaluator.hpp"
#include "Token.hpp"
#include "SyntaxException.hpp"

template<class... Ts>
class ExpressionParser : public ExpressionTokenizer<Ts...>, public ExpressionPostfixParser<Ts...>, public ExpressionEvaluator<Ts...>
{
  public:
  using ValueType          = ValueToken<Ts...>;
  using UnaryOperatorType  = UnaryOperatorToken<ValueType*>;
  using BinaryOperatorType = BinaryOperatorToken<ValueType*>;
  using FunctionType       = FunctionToken<ValueType*>;
  using VariableType       = VariableToken<Ts...>;
  using MiscType           = GenericToken<char>;

  ValueType Evaluate(const std::string& expression)
  {
    auto tokens  = ExpressionTokenizer<Ts...>::Execute(expression, *m_pVariables);
    auto postfix = ExpressionPostfixParser<Ts...>::Execute(tokens);
    auto result  = ExpressionEvaluator<Ts...>::Execute(postfix);

    return result;
  }

  void SetVariables(const std::unordered_map<std::string, VariableType*>* value) { m_pVariables = value; }

  ExpressionParser(const std::function<ValueType*(const std::string&)>& numberConverter)
      : ExpressionTokenizer<Ts...>(numberConverter)
      , ExpressionPostfixParser<Ts...>()
      , ExpressionEvaluator<Ts...>()
      , m_pVariables(nullptr)
  {}

  ExpressionParser(const ExpressionParser<Ts...>& other)
      : ExpressionTokenizer<Ts...>(other)
      , ExpressionPostfixParser<Ts...>(other)
      , ExpressionEvaluator<Ts...>(other)
      , m_pVariables(other.m_pVariables)
  {}

  ExpressionParser(ExpressionParser<Ts...>&& other)
      : ExpressionTokenizer<Ts...>(std::move(other))
      , ExpressionPostfixParser<Ts...>(std::move(other))
      , ExpressionEvaluator<Ts...>(std::move(other))
      , m_pVariables(std::move(other.m_pVariables))
  {}

  virtual ~ExpressionParser() override = default;

  private:
  using ExpressionTokenizer<Ts...>::Execute;
  using ExpressionPostfixParser<Ts...>::Execute;
  using ExpressionEvaluator<Ts...>::Execute;

  const std::unordered_map<std::string, VariableType*>* m_pVariables;
};

#endif // __EXPRESSIONPARSER_HPP__
