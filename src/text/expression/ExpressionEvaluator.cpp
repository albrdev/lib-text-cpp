#include "ExpressionEvaluator.hpp"
#include "FunctionToken.hpp"
#include "FunctionTokenHelper.hpp"
#include "IBinaryOperatorToken.hpp"
#include "IUnaryOperatorToken.hpp"
#include "IVariableToken.hpp"
#include "text/exception/SyntaxError.hpp"

namespace Text::Expression
{
  IValueToken* ExpressionEvaluator::Execute(std::queue<IToken*>& postfix)
  {
    m_ResultCache.clear();

    std::vector<IValueToken*> stack;

    IValueToken* value                   = nullptr;
    IUnaryOperatorToken* unaryOperator   = nullptr;
    IBinaryOperatorToken* binaryOperator = nullptr;
    FunctionTokenHelper* function        = nullptr;

    while(!postfix.empty())
    {
      const auto current = postfix.front();

      if((value = current->As<IValueToken*>()) != nullptr)
      {
        stack.push_back(value);
      }
      else if((unaryOperator = current->As<IUnaryOperatorToken*>()) != nullptr)
      {
        if(stack.size() < 1u)
        {
          throw Exception::SyntaxError("Insufficient arguments provided for unary operator: " + unaryOperator->GetIdentifier());
        }

        const auto rhs = stack.back();
        stack.pop_back();

        auto value = (*unaryOperator)(rhs);
        m_ResultCache.push_back(std::unique_ptr<IValueToken>(value));
        stack.push_back(value);
      }
      else if((binaryOperator = current->As<IBinaryOperatorToken*>()) != nullptr)
      {
        if(stack.size() < 2u)
        {
          throw Exception::SyntaxError("Insufficient arguments provided for binary operator: " + binaryOperator->GetIdentifier());
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
      else if((function = current->As<FunctionTokenHelper*>()) != nullptr)
      {
        if(function->m_ArgumentCount < function->GetMinArgumentCount() ||
           function->m_ArgumentCount > std::min(function->GetMaxArgumentCount(), FunctionToken::GetArgumentCountMaxLimit()))
        {
          throw Exception::SyntaxError("Invalid number of arguments provided for function: " + function->GetIdentifier());
        }
        else if(stack.size() < function->m_ArgumentCount)
        {
          throw Exception::SyntaxError("Insufficient arguments provided for function: " + function->GetIdentifier());
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
        throw Exception::SyntaxError("Unknown token encountered during evaluation process: " + current->ToString());
      }

      postfix.pop();
    }

    if(stack.size() != 1u)
    {
      throw Exception::SyntaxError("Excessive values provided: " + std::to_string(stack.size()));
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
} // namespace Text::Expression
