#ifndef __TEXT_EXPRESSION__EXPRESSIONPARSER_HPP__
#define __TEXT_EXPRESSION__EXPRESSIONPARSER_HPP__

#include "ExpressionParserBase.hpp"

namespace Text::Expression
{
  class IValueToken;

  class ExpressionParser : public ExpressionParserBase
  {
    public:
    IValueToken* Evaluate(const std::string& expression);

    virtual ~ExpressionParser() override = default;
    ExpressionParser();
    ExpressionParser(const ExpressionParser& other);
    ExpressionParser(ExpressionParser&& other);

    private:
    using ExpressionParserBase::Parse;
    using ExpressionParserBase::Evaluate;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__EXPRESSIONPARSER_HPP__
