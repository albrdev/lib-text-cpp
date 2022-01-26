#ifndef __EXPRESSIONPARSER_HPP__
#define __EXPRESSIONPARSER_HPP__

#include <string>
#include <queue>
#include <unordered_map>
#include "IValueToken.hpp"
#include "IVariableToken.hpp"
#include "IUnaryOperatorToken.hpp"
#include "IBinaryOperatorToken.hpp"
#include "IFunctionToken.hpp"
#include "ExpressionTokenizer.hpp"
#include "ExpressionPostfixParser.hpp"
#include "ExpressionEvaluator.hpp"
#include "Token.hpp"

class ExpressionParser : public ExpressionTokenizer, public ExpressionPostfixParser, public ExpressionEvaluator
{
  public:
  IValueToken* Evaluate(const std::string& expression);

  void SetUnaryOperators(const std::unordered_map<char, IUnaryOperatorToken*>* value);
  void SetBinaryOperators(const std::unordered_map<std::string, IBinaryOperatorToken*>* value);
  void SetVariables(const std::unordered_map<std::string, IVariableToken*>* value);
  void SetFunctions(const std::unordered_map<std::string, IFunctionToken*>* value);

  virtual ~ExpressionParser() override = default;
  ExpressionParser();
  ExpressionParser(const ExpressionParser& other);
  ExpressionParser(ExpressionParser&& other);

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
