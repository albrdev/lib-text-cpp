#ifndef __TEXT_EXPRESSION__EXPRESSIONPOSTFIXPARSER_HPP__
#define __TEXT_EXPRESSION__EXPRESSIONPOSTFIXPARSER_HPP__

#include "FunctionTokenHelper.hpp"

#include <memory>
#include <queue>
#include <vector>

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
    std::vector<std::unique_ptr<FunctionTokenHelper>> m_FunctionCache;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__EXPRESSIONPOSTFIXPARSER_HPP__
