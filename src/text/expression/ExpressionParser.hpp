#ifndef __EXPRESSIONPARSER_HPP__
#define __EXPRESSIONPARSER_HPP__

#include <string>
#include <queue>
#include <unordered_map>
#include "ExpressionTokenizer.hpp"
#include "ExpressionPostfixParser.hpp"
#include "ExpressionEvaluator.hpp"
#include "Token.hpp"
#include "IValueToken.hpp"
#include "IVariableToken.hpp"
#include "IUnaryOperatorToken.hpp"
#include "IBinaryOperatorToken.hpp"
#include "IFunctionToken.hpp"
#include "SyntaxException.hpp"

class ExpressionParser : public ExpressionTokenizer, public ExpressionPostfixParser, public ExpressionEvaluator
{
  public:
  using MiscType = GenericToken<char>;

  IValueToken* Evaluate(const std::string& expression)
  {
    auto tokens  = ExpressionTokenizer::Execute(expression, m_pUnaryOperators, m_pBinaryOperators, m_pVariables, m_pFunctions);
    auto postfix = ExpressionPostfixParser::Execute(tokens);
    auto result  = ExpressionEvaluator::Execute(postfix);

    return result;
  }

  void SetUnaryOperators(const std::unordered_map<char, IUnaryOperatorToken*>* value) { m_pUnaryOperators = value; }
  void SetBinaryOperators(const std::unordered_map<std::string, IBinaryOperatorToken*>* value) { m_pBinaryOperators = value; }
  void SetVariables(const std::unordered_map<std::string, IVariableToken*>* value) { m_pVariables = value; }
  void SetFunctions(const std::unordered_map<std::string, IFunctionToken*>* value) { m_pFunctions = value; }

  ExpressionParser()
      : ExpressionTokenizer()
      , ExpressionPostfixParser()
      , ExpressionEvaluator()
      , m_pVariables(nullptr)
  {}

  ExpressionParser(const ExpressionParser& other)
      : ExpressionTokenizer(other)
      , ExpressionPostfixParser(other)
      , ExpressionEvaluator(other)
      , m_pVariables(other.m_pVariables)
  {}

  ExpressionParser(ExpressionParser&& other)
      : ExpressionTokenizer(std::move(other))
      , ExpressionPostfixParser(std::move(other))
      , ExpressionEvaluator(std::move(other))
      , m_pVariables(std::move(other.m_pVariables))
  {}

  virtual ~ExpressionParser() override = default;

  private:
  using ExpressionTokenizer::Execute;
  using ExpressionPostfixParser::Execute;
  using ExpressionEvaluator::Execute;

  const std::unordered_map<char, IUnaryOperatorToken*>* m_pUnaryOperators;
  const std::unordered_map<std::string, IBinaryOperatorToken*>* m_pBinaryOperators;
  const std::unordered_map<std::string, IVariableToken*>* m_pVariables;
  const std::unordered_map<std::string, IFunctionToken*>* m_pFunctions;
};

#endif // __EXPRESSIONPARSER_HPP__
