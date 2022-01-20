#ifndef __EXPRESSIONPOSTFIXPARSER_HPP__
#define __EXPRESSIONPOSTFIXPARSER_HPP__

#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <memory>
#include <sstream>
#include "Token.hpp"
#include "FunctionTokenHelper.hpp"
#include "SyntaxException.hpp"

template<class... Ts>
class ExpressionPostfixParser
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
  void SetJuxtapositionOperator(typename BinaryOperatorType::CallbackType callback, int precedence, Associativity associativity = Associativity::Right)
  {
    m_JuxtapositionOperator = std::make_unique<BinaryOperatorType>(callback, "", precedence, associativity);
  }

  void SetJuxtapositionOperator(std::nullptr_t) { m_JuxtapositionOperator = nullptr; }

  ExpressionPostfixParser()
      : m_JuxtapositionOperator()
  {}

  ExpressionPostfixParser(const ExpressionPostfixParser<Ts...>& other)
      : m_JuxtapositionOperator(other.m_JuxtapositionOperator)
  {}

  ExpressionPostfixParser(ExpressionPostfixParser<Ts...>&& other)
      : m_JuxtapositionOperator(std::move(other.m_JuxtapositionOperator))
  {}

  virtual ~ExpressionPostfixParser() = default;

  std::queue<IToken*> Execute(std::queue<IToken*>& tokens)
  {
    m_FunctionHelperCache.clear();

    const OperatorToken* anyOperator = nullptr;
    const FunctionType* function     = nullptr;
    const MiscType* misc             = nullptr;

    std::queue<IToken*> queue;
    std::stack<IToken*> stack;
    std::stack<FunctionHelperType*> functionHelpers;

    const IToken* previous = nullptr;
    while(!tokens.empty())
    {
      const auto current = tokens.front();
      if(current->IsType<ValueType>())
      {
        if(m_JuxtapositionOperator != nullptr)
        {
          if(((misc = previous->AsPointer<MiscType>()) != nullptr && misc->GetObject() == ')') ||
             (current->IsType<VariableType>() && previous->IsType<ValueType>()))
          {
            stack.push(m_JuxtapositionOperator.get());
          }
        }

        queue.push(current);
      }
      else if((anyOperator = current->AsPointer<OperatorToken>()) != nullptr)
      {
        OperatorToken* tmpOperator;
        while(!stack.empty() && (tmpOperator = stack.top()->AsPointer<OperatorToken>()) != nullptr)
        {
          if(((anyOperator->GetAssociativity() & Associativity::Left) != 0 && anyOperator->GetPrecedence() <= tmpOperator->GetPrecedence()) ||
             anyOperator->GetPrecedence() < tmpOperator->GetPrecedence())
          {
            queue.push(stack.top());
            stack.pop();
          }
          else
          {
            break;
          }
        }

        stack.push(current);
      }
      else if((function = current->AsPointer<FunctionType>()) != nullptr)
      {
        if(m_JuxtapositionOperator != nullptr)
        {
          if(((misc = previous->AsPointer<MiscType>()) != nullptr && misc->GetObject() == ')') || previous->IsType<ValueType>())
          {
            stack.push(m_JuxtapositionOperator.get());
          }
        }

        auto functionHelper = std::make_unique<FunctionHelperType>(*function);
        m_FunctionHelperCache.push_back(std::move(functionHelper));
        stack.push(m_FunctionHelperCache.back().get());
        functionHelpers.push(m_FunctionHelperCache.back().get());
      }
      else if((misc = current->AsPointer<MiscType>()) != nullptr)
      {
        switch(misc->GetObject())
        {
          case '(':
          {
            if(m_JuxtapositionOperator != nullptr && previous->IsType<ValueType>())
            {
              stack.push(m_JuxtapositionOperator.get());
            }

            if(!functionHelpers.empty())
            {
              functionHelpers.top()->IncrementBracketBalance();
            }

            stack.push(current);
            break;
          }
          case ')':
          {
            if(!functionHelpers.empty())
            {
              functionHelpers.top()->DecrementBracketBalance();

              if(functionHelpers.top()->GetBracketBalance() == 0)
              {
                if(previous != nullptr && ((misc = previous->AsPointer<MiscType>()) == nullptr || misc->GetObject() != '('))
                {
                  functionHelpers.top()->IncrementArgumentCount();
                }

                functionHelpers.pop();
              }
            }

            misc = nullptr;
            while(!stack.empty() && ((misc = stack.top()->AsPointer<MiscType>()) == nullptr || misc->GetObject() != '('))
            {
              queue.push(stack.top());
              stack.pop();
            }

            if(misc == nullptr || misc->GetObject() != '(')
            {
              throw SyntaxException("Missing matching closing bracket");
            }

            stack.pop();

            if(!stack.empty() && stack.top()->IsType<FunctionHelperType>())
            {
              queue.push(stack.top());
              stack.pop();
            }

            break;
          }
          case ',':
          {
            if(!functionHelpers.empty())
            {
              functionHelpers.top()->IncrementArgumentCount();
            }

            while(!stack.empty() && ((misc = stack.top()->AsPointer<MiscType>()) == nullptr || misc->GetObject() != '('))
            {
              queue.push(stack.top());
              stack.pop();
            }

            if(misc == nullptr || misc->GetObject() != '(')
            {
              throw std::runtime_error("Missing matching opening bracket");
            }

            break;
          }
        }
      }
      else
      {
        throw SyntaxException("Unknown token encountered during postfix process: " + current->ToString());
      }

      previous = current;
      tokens.pop();
    }

    while(!stack.empty())
    {
      if((misc = stack.top()->AsPointer<MiscType>()) != nullptr && (misc->GetObject() == '(' || misc->GetObject() == ')'))
      {
        throw std::runtime_error("Missing matching closing bracket");
      }

      queue.push(stack.top());
      stack.pop();
    }

    return queue;
  }

  private:
  std::unique_ptr<BinaryOperatorType> m_JuxtapositionOperator;

  std::vector<std::unique_ptr<FunctionHelperType>> m_FunctionHelperCache;
};

#endif // __EXPRESSIONPOSTFIXPARSER_HPP__
