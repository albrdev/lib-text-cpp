#ifndef __TEXT_EXPRESSION_EXPRESSIONPOSTFIXPARSER_HPP__
#define __TEXT_EXPRESSION_EXPRESSIONPOSTFIXPARSER_HPP__

#include <vector>
#include <stack>
#include <queue>
#include <memory>
#include "IValueToken.hpp"
#include "FunctionToken.hpp"

namespace Text::Expression
{
  class ExpressionPostfixParser
  {
    public:
    std::queue<IToken*> Execute(std::queue<IToken*>& tokens);

    virtual ~ExpressionPostfixParser() = default;
    ExpressionPostfixParser();
    ExpressionPostfixParser(const ExpressionPostfixParser& other);
    ExpressionPostfixParser(ExpressionPostfixParser&& other);

    private:
    std::vector<std::unique_ptr<FunctionToken>> m_FunctionCache;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION_EXPRESSIONPOSTFIXPARSER_HPP__
