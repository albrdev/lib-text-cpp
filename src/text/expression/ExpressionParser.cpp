#include "ExpressionParser.hpp"

IValueToken* ExpressionParser::Evaluate(const std::string& expression)
{
  auto tokens  = ExpressionTokenizer::Execute(expression, m_pUnaryOperators, m_pBinaryOperators, m_pVariables, m_pFunctions);
  auto postfix = ExpressionPostfixParser::Execute(tokens);
  auto result  = ExpressionEvaluator::Execute(postfix);

  return result;
}

void ExpressionParser::SetUnaryOperators(const std::unordered_map<char, IUnaryOperatorToken*>* value) { m_pUnaryOperators = value; }
void ExpressionParser::SetBinaryOperators(const std::unordered_map<std::string, IBinaryOperatorToken*>* value) { m_pBinaryOperators = value; }
void ExpressionParser::SetVariables(const std::unordered_map<std::string, IVariableToken*>* value) { m_pVariables = value; }
void ExpressionParser::SetFunctions(const std::unordered_map<std::string, IFunctionToken*>* value) { m_pFunctions = value; }

ExpressionParser::ExpressionParser()
    : ExpressionTokenizer()
    , ExpressionPostfixParser()
    , ExpressionEvaluator()
    , m_pVariables(nullptr)
{}

ExpressionParser::ExpressionParser(const ExpressionParser& other)
    : ExpressionTokenizer(other)
    , ExpressionPostfixParser(other)
    , ExpressionEvaluator(other)
    , m_pVariables(other.m_pVariables)
{}

ExpressionParser::ExpressionParser(ExpressionParser&& other)
    : ExpressionTokenizer(std::move(other))
    , ExpressionPostfixParser(std::move(other))
    , ExpressionEvaluator(std::move(other))
    , m_pVariables(std::move(other.m_pVariables))
{}
