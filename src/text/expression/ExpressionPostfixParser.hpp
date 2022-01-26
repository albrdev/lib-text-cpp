#ifndef __EXPRESSIONPOSTFIXPARSER_HPP__
#define __EXPRESSIONPOSTFIXPARSER_HPP__

#include <vector>
#include <stack>
#include <queue>
#include <memory>
#include "IValueToken.hpp"
#include "FunctionTokenHelper.hpp"

class ExpressionPostfixParser
{
  public:
  std::queue<IToken*> Execute(std::queue<IToken*>& tokens);

  virtual ~ExpressionPostfixParser() = default;
  ExpressionPostfixParser();
  ExpressionPostfixParser(const ExpressionPostfixParser& other);
  ExpressionPostfixParser(ExpressionPostfixParser&& other);

  private:
  std::vector<std::unique_ptr<FunctionTokenHelper>> m_FunctionHelperCache;
};

#endif // __EXPRESSIONPOSTFIXPARSER_HPP__
