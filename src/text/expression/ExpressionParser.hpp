#ifndef __EXPRESSIONPARSER_HPP__
#define __EXPRESSIONPARSER_HPP__

#include <string>
#include <queue>
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

  const VariableType& GetVariable(const std::string& identifier) const { return *m_Variables.at(identifier); }
  VariableType& GetVariable(const std::string& identifier) { return *m_Variables.at(identifier); }

  template<class T>
  void AddVariable(const std::string& identifier, const T& value)
  {
    m_Variables[identifier] = std::make_unique<VariableType>(identifier, value);
  }

  void RemoveVariable(const std::string& identifier) { m_Variables.erase(identifier); }

  ValueType Evaluate(const std::string& expression)
  {
    m_VariableCache.clear();

    auto tokens  = ExpressionTokenizer<Ts...>::Execute(expression, m_Variables, m_VariableCache);
    auto postfix = ExpressionPostfixParser<Ts...>::Execute(tokens);
    auto result  = ExpressionEvaluator<Ts...>::Execute(postfix, m_Variables, m_VariableCache);

    if(!m_VariableCache.empty())
    {
      throw SyntaxException("Accessing nonexistent variable(s)");
    }

    return result;
  }

  ExpressionParser(const std::function<ValueType*(const std::string&)>& numberConverter)
      : ExpressionTokenizer<Ts...>(numberConverter)
      , ExpressionPostfixParser<Ts...>()
      , ExpressionEvaluator<Ts...>()
      , m_Variables()
      , m_VariableCache()
  {}

  ExpressionParser(const ExpressionParser<Ts...>& other)
      : ExpressionTokenizer<Ts...>(other)
      , ExpressionPostfixParser<Ts...>(other)
      , ExpressionEvaluator<Ts...>(other)
      , m_Variables(other.m_Variables)
      , m_VariableCache(other.m_VariableCache)
  {}

  ExpressionParser(ExpressionParser<Ts...>&& other)
      : ExpressionTokenizer<Ts...>(std::move(other))
      , ExpressionPostfixParser<Ts...>(std::move(other))
      , ExpressionEvaluator<Ts...>(std::move(other))
      , m_Variables(std::move(other.m_Variables))
      , m_VariableCache(std::move(other.m_VariableCache))
  {}

  virtual ~ExpressionParser() override = default;

  private:
  using ExpressionTokenizer<Ts...>::Execute;
  using ExpressionPostfixParser<Ts...>::Execute;
  using ExpressionEvaluator<Ts...>::Execute;

  std::unordered_map<std::string, std::unique_ptr<VariableType>> m_Variables;
  std::unordered_map<std::string, std::unique_ptr<VariableType>> m_VariableCache;
};

#endif // __EXPRESSIONPARSER_HPP__
