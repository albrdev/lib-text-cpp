#include "ExpressionParser.hpp"

IValueToken* ExpressionParser::Evaluate(const std::string& expression)
{
  auto postfix = ExpressionParserBase::Parse(expression);
  auto result  = ExpressionParserBase::Evaluate(postfix);

  return result;
}

ExpressionParser::ExpressionParser()
    : ExpressionParserBase()
{}

ExpressionParser::ExpressionParser(const ExpressionParser& other)
    : ExpressionParserBase(other)
{}

ExpressionParser::ExpressionParser(ExpressionParser&& other)
    : ExpressionParserBase(std::move(other))
{}
