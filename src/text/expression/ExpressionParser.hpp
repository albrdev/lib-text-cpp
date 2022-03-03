#ifndef __TEXT_EXPRESSION_EXPRESSIONPARSER_HPP__
#define __TEXT_EXPRESSION_EXPRESSIONPARSER_HPP__

#include <string>
#include "IValueToken.hpp"
#include "ExpressionParserBase.hpp"

namespace Text::Expression
{
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

#endif // __TEXT_EXPRESSION_EXPRESSIONPARSER_HPP__
