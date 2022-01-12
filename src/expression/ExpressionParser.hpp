#ifndef __EXPRESSIONPARSER_HPP__
#define __EXPRESSIONPARSER_HPP__

#include <string>
#include <queue>
#include "ExpressionTokenizer.hpp"
#include "ExpressionPostfixParser.hpp"
#include "ExpressionEvaluator.hpp"
#include "Token.hpp"
#include "SyntaxException.hpp"

template<class T>
class ExpressionParser : public ExpressionTokenizer<T>, public ExpressionPostfixParser<T>, public ExpressionEvaluator<T>
{
  public:
  using ValueType          = ValueToken<T>;
  using UnaryOperatorType  = UnaryOperatorToken<ValueType*>;
  using BinaryOperatorType = BinaryOperatorToken<ValueType*>;
  using FunctionType       = FunctionToken<ValueType*>;
  using VariableType       = VariableToken<T>;
  using MiscType           = GenericToken<char>;

  const VariableType& GetVariable(const std::string& identifier) const { return *m_Variables.at(identifier); }
  VariableType& GetVariable(const std::string& identifier) { return *m_Variables.at(identifier); }

  void AddVariable(const std::string& identifier, std::nullptr_t value) { m_Variables[identifier] = std::make_unique<VariableType>(identifier, value); }
  void AddVariable(const std::string& identifier, const std::string& value) { m_Variables[identifier] = std::make_unique<VariableType>(identifier, value); }
  void AddVariable(const std::string& identifier, const T& value) { m_Variables[identifier] = std::make_unique<VariableType>(identifier, value); }

  void RemoveVariable(const std::string& identifier, const T& value) { m_Variables.erase(identifier); }

  ValueType Evaluate(const std::string& expression)
  {
    m_VariableCache.clear();

    auto tokens  = ExpressionTokenizer<T>::Execute(expression, m_Variables, m_VariableCache);
    auto postfix = ExpressionPostfixParser<T>::Execute(tokens);
    auto result  = ExpressionEvaluator<T>::Execute(postfix, m_Variables, m_VariableCache);

    if(!m_VariableCache.empty())
    {
      throw SyntaxException("Expression uninitialized variable(s)");
    }

    return result;
  }

  ExpressionParser()
      : ExpressionTokenizer<T>()
      , ExpressionPostfixParser<T>()
      , ExpressionEvaluator<T>()
      , m_Variables()
      , m_VariableCache()
  {}

  ExpressionParser(const ExpressionParser<T>& other)
      : ExpressionTokenizer<T>(other)
      , ExpressionPostfixParser<T>(other)
      , ExpressionEvaluator<T>(other)
      , m_Variables(other.m_Variables)
      , m_VariableCache(other.m_VariableCache)
  {}

  ExpressionParser(ExpressionParser<T>&& other)
      : ExpressionTokenizer<T>(std::move(other))
      , ExpressionPostfixParser<T>(std::move(other))
      , ExpressionEvaluator<T>(std::move(other))
      , m_Variables(std::move(other.m_Variables))
      , m_VariableCache(std::move(other.m_VariableCache))
  {}

  virtual ~ExpressionParser() override = default;

  private:
  using ExpressionTokenizer<T>::Execute;
  using ExpressionPostfixParser<T>::Execute;
  using ExpressionEvaluator<T>::Execute;

  std::unordered_map<std::string, std::unique_ptr<VariableType>> m_Variables;
  std::unordered_map<std::string, std::unique_ptr<VariableType>> m_VariableCache;
};

#endif // __EXPRESSIONPARSER_HPP__
