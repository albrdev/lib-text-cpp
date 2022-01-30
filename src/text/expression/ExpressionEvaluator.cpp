#include "ExpressionEvaluator.hpp"
#include "IVariableToken.hpp"
#include "IUnaryOperatorToken.hpp"
#include "IBinaryOperatorToken.hpp"
#include "FunctionToken.hpp"
#include "../SyntaxException.hpp"

IValueToken* ExpressionEvaluator::Execute(std::queue<IToken*> postfix)
{
  m_ResultCache.clear();

  std::vector<IValueToken*> stack;

  IValueToken* value                   = nullptr;
  IUnaryOperatorToken* unaryOperator   = nullptr;
  IBinaryOperatorToken* binaryOperator = nullptr;
  FunctionToken* function              = nullptr;

  while(!postfix.empty())
  {
    const auto current = postfix.front();

    if((value = current->AsPointer<IValueToken>()) != nullptr)
    {
      stack.push_back(value);
    }
    else if((unaryOperator = current->AsPointer<IUnaryOperatorToken>()) != nullptr)
    {
      if(stack.size() < 1u)
      {
        throw SyntaxException("Insufficient arguments provided for unary operator: " + unaryOperator->GetIdentifier());
      }

      const auto rhs = stack.back();
      stack.pop_back();

      auto value = (*unaryOperator)(rhs);
      m_ResultCache.push_back(std::unique_ptr<IValueToken>(value));
      stack.push_back(value);
    }
    else if((binaryOperator = current->AsPointer<IBinaryOperatorToken>()) != nullptr)
    {
      if(stack.size() < 2u)
      {
        throw SyntaxException("Insufficient arguments provided for binary operator: " + binaryOperator->GetIdentifier());
      }

      const auto rhs = stack.back();
      stack.pop_back();
      const auto lhs = stack.back();
      stack.pop_back();

      auto value = (*binaryOperator)(lhs, rhs);
      if(!value->IToken::IsType<IVariableToken>())
      {
        m_ResultCache.push_back(std::unique_ptr<IValueToken>(value)); //*
      }

      stack.push_back(value);
    }
    else if((function = current->AsPointer<FunctionToken>()) != nullptr)
    {
      if(function->m_ArgumentCount < function->GetMinArgumentCount() ||
         function->m_ArgumentCount > std::min(function->GetMaxArgumentCount(), FunctionToken::GetArgumentCountMaxLimit()))
      {
        throw SyntaxException("Invalid number of arguments provided for function: " + function->GetIdentifier());
      }
      else if(stack.size() < function->m_ArgumentCount)
      {
        throw SyntaxException("Insufficient arguments provided for function: " + function->GetIdentifier());
      }

      std::vector<IValueToken*> args;
      if(function->m_ArgumentCount > 0u)
      {
        std::copy(stack.cend() - function->m_ArgumentCount, stack.cend(), std::back_inserter(args));
      }

      auto value = (*function)(args);
      stack.erase(stack.end() - function->m_ArgumentCount, stack.end());

      m_ResultCache.push_back(std::unique_ptr<IValueToken>(value));
      stack.push_back(value);
    }
    else
    {
      throw SyntaxException("Unknown token encountered during evaluation process: " + current->ToString());
    }

    postfix.pop();
  }

  if(stack.size() != 1u)
  {
    throw SyntaxException("Excessive values provided: " + std::to_string(stack.size()));
  }

  return stack.back();
}

ExpressionEvaluator::ExpressionEvaluator()
    : m_ResultCache()
{}

ExpressionEvaluator::ExpressionEvaluator(const ExpressionEvaluator& other)
    : m_ResultCache()
{
  static_cast<void>(other);
}

ExpressionEvaluator::ExpressionEvaluator(ExpressionEvaluator&& other)
    : m_ResultCache(std::move(other.m_ResultCache))
{}
