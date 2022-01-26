#ifndef __EXPRESSIONEVALUATOR_HPP__
#define __EXPRESSIONEVALUATOR_HPP__

#include <vector>
#include <queue>
#include <memory>
#include "IValueToken.hpp"

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

#endif // __EXPRESSIONEVALUATOR_HPP__
