#ifndef __EXPRESSIONEVALUATOR_HPP__
#define __EXPRESSIONEVALUATOR_HPP__

#include <vector>
#include <queue>
#include <memory>
#include "Token.hpp"
#include "FunctionTokenHelper.hpp"

template<class... Ts>
class ExpressionEvaluator
{
  public:
  using ValueType          = ValueToken<Ts...>;
  using UnaryOperatorType  = UnaryOperatorToken<ValueType*>;
  using BinaryOperatorType = BinaryOperatorToken<ValueType*>;
  using FunctionType       = FunctionToken<ValueType*>;
  using VariableType       = VariableToken<Ts...>;
  using MiscType           = GenericToken<char>;

  private:
  using FunctionHelperType = FunctionTokenHelper<typename FunctionType::InnerValueType>;

  public:
  ExpressionEvaluator()
      : m_ResultCache()
  {}

  ExpressionEvaluator(const ExpressionEvaluator<Ts...>& other)
      : m_ResultCache(other.m_ResultCache)
  {}

  ExpressionEvaluator(ExpressionEvaluator<Ts...>&& other)
      : m_ResultCache(std::move(other.m_ResultCache))
  {}

  virtual ~ExpressionEvaluator() = default;

  ValueType& Execute(std::queue<IToken*> postfix,
                     std::unordered_map<std::string, std::unique_ptr<VariableType>>& variables,
                     std::unordered_map<std::string, std::unique_ptr<VariableType>>& variableCache)
  {
    m_ResultCache.clear();

    std::vector<ValueType*> stack;

    ValueType* value                   = nullptr;
    UnaryOperatorType* unaryOperator   = nullptr;
    BinaryOperatorType* binaryOperator = nullptr;
    FunctionHelperType* functionHelper = nullptr;

    while(!postfix.empty())
    {
      const auto current = postfix.front();

      if((value = current->AsPointer<ValueType>()) != nullptr)
      {
        stack.push_back(value);
      }
      else if((unaryOperator = current->AsPointer<UnaryOperatorType>()) != nullptr)
      {
        if(stack.size() < 1u)
        {
          throw SyntaxException("Insufficient arguments provided for unary operator: " + unaryOperator->GetIdentifier());
        }

        const auto rhs = stack.back();
        stack.pop_back();

        auto value = (*unaryOperator)(rhs);
        m_ResultCache.push_back(std::unique_ptr<ValueType>(value));
        stack.push_back(value);
      }
      else if((binaryOperator = current->AsPointer<BinaryOperatorType>()) != nullptr)
      {
        if(stack.size() < 2u)
        {
          throw SyntaxException("Insufficient arguments provided for binary operator: " + binaryOperator->GetIdentifier());
        }

        const auto rhs = stack.back();
        stack.pop_back();
        const auto lhs = stack.back();
        stack.pop_back();

        VariableType* variable = lhs->IToken::AsPointer<VariableType>();
        bool isNewVariable     = variable != nullptr && !variable->IsInitialized();

        auto value = (*binaryOperator)(lhs, rhs);
        if(value->IToken::IsType<VariableType>())
        {
          if(isNewVariable && value == lhs)
          {
            auto variableIterator = variableCache.extract(variable->GetIdentifier());
            variables.insert(std::move(variableIterator));
          }
        }
        else
        {
          m_ResultCache.push_back(std::unique_ptr<ValueType>(value));
        }

        stack.push_back(value);
      }
      else if((functionHelper = current->AsPointer<FunctionHelperType>()) != nullptr)
      {
        if(functionHelper->GetArgumentCount() < functionHelper->GetFunction().GetMinArgumentCount() ||
           functionHelper->GetArgumentCount() > std::min(functionHelper->GetFunction().GetMaxArgumentCount(), FunctionType::GetArgumentCountMaxLimit()))
        {
          throw SyntaxException("Invalid number of arguments provided for function: " + functionHelper->GetFunction().GetIdentifier());
        }
        else if(stack.size() < functionHelper->GetArgumentCount())
        {
          throw SyntaxException("Insufficient arguments provided for function: " + functionHelper->GetFunction().GetIdentifier());
        }

        std::vector<ValueType*> args;
        if(functionHelper->GetArgumentCount() > 0u)
        {
          std::copy(stack.cend() - functionHelper->GetArgumentCount(), stack.cend(), std::back_inserter(args));
        }

        auto value = functionHelper->GetFunction()(args);
        stack.erase(stack.end() - functionHelper->GetArgumentCount(), stack.end());

        m_ResultCache.push_back(std::unique_ptr<ValueType>(value));
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

    return *stack.back();
  }

  private:
  std::vector<std::unique_ptr<ValueType>> m_ResultCache;
};

#endif // __EXPRESSIONEVALUATOR_HPP__
