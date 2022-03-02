#ifndef __TEXT_EXPRESSION_EXPRESSIONEVALUATOR_HPP__
#define __TEXT_EXPRESSION_EXPRESSIONEVALUATOR_HPP__

#include <vector>
#include <queue>
#include <memory>
#include "IValueToken.hpp"

namespace text::expression
{
  class ExpressionEvaluator
  {
    public:
    IValueToken* Execute(std::queue<IToken*> postfix);

    virtual ~ExpressionEvaluator() = default;
    ExpressionEvaluator();
    ExpressionEvaluator(const ExpressionEvaluator& other);
    ExpressionEvaluator(ExpressionEvaluator&& other);

    private:
    std::vector<std::unique_ptr<IValueToken>> m_ResultCache;
  };
} // namespace text::expression

#endif // __TEXT_EXPRESSION_EXPRESSIONEVALUATOR_HPP__
