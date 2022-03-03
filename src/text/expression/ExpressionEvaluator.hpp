#ifndef __TEXT_EXPRESSION__EXPRESSIONEVALUATOR_HPP__
#define __TEXT_EXPRESSION__EXPRESSIONEVALUATOR_HPP__

#include <vector>
#include <queue>
#include <memory>
#include "IValueToken.hpp"

namespace Text::Expression
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
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__EXPRESSIONEVALUATOR_HPP__
