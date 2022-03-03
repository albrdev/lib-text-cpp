#include "ExpressionParserBase.hpp"

namespace Text::Expression
{
  std::queue<IToken*> ExpressionParserBase::Parse(const std::string& expression)
  {
    auto tokens  = ExpressionTokenizer::Execute(expression, m_pUnaryOperators, m_pBinaryOperators, m_pVariables, m_pFunctions);
    auto postfix = ExpressionPostfixParser::Execute(tokens);

    return postfix;
  }

  IValueToken* ExpressionParserBase::Evaluate(std::queue<IToken*>& postfix) { return ExpressionEvaluator::Execute(postfix); }

  void ExpressionParserBase::SetUnaryOperators(const std::unordered_map<char, IUnaryOperatorToken*>* value) { m_pUnaryOperators = value; }
  void ExpressionParserBase::SetBinaryOperators(const std::unordered_map<std::string, IBinaryOperatorToken*>* value) { m_pBinaryOperators = value; }
  void ExpressionParserBase::SetVariables(const std::unordered_map<std::string, IVariableToken*>* value) { m_pVariables = value; }
  void ExpressionParserBase::SetFunctions(const std::unordered_map<std::string, IFunctionToken*>* value) { m_pFunctions = value; }

  ExpressionParserBase::ExpressionParserBase()
      : ExpressionTokenizer()
      , ExpressionPostfixParser()
      , ExpressionEvaluator()
      , m_pVariables(nullptr)
  {}

  ExpressionParserBase::ExpressionParserBase(const ExpressionParserBase& other)
      : ExpressionTokenizer(other)
      , ExpressionPostfixParser(other)
      , ExpressionEvaluator(other)
      , m_pVariables(other.m_pVariables)
  {}

  ExpressionParserBase::ExpressionParserBase(ExpressionParserBase&& other)
      : ExpressionTokenizer(std::move(other))
      , ExpressionPostfixParser(std::move(other))
      , ExpressionEvaluator(std::move(other))
      , m_pVariables(std::move(other.m_pVariables))
  {}
} // namespace Text::Expression
