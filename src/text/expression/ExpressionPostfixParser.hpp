#ifndef __EXPRESSIONPOSTFIXPARSER_HPP__
#define __EXPRESSIONPOSTFIXPARSER_HPP__

#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>
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

    ValueType* value           = nullptr;
    OperatorToken* anyOperator = nullptr;
    FunctionType* function     = nullptr;
    VariableType* variable     = nullptr;
    MiscType* misc             = nullptr;

    std::queue<IToken*> queue;
    std::stack<IToken*> stack;
    std::stack<FunctionHelperType*> functionHelpers;

    IToken* previous = nullptr;
    while(!tokens.empty())
    {
      const auto current = tokens.front();
      if((value = dynamic_cast<ValueType*>(current)) != nullptr)
      {
        if(m_JuxtapositionOperator != nullptr)
        {
          if(((misc = dynamic_cast<MiscType*>(previous)) != nullptr && misc->GetObject() == ')') ||
             ((variable = dynamic_cast<VariableType*>(current)) != nullptr && (value = dynamic_cast<ValueType*>(previous)) != nullptr))
          {
            stack.push(m_JuxtapositionOperator.get());
          }
        }

        queue.push(current);
      }
      else if((anyOperator = dynamic_cast<OperatorToken*>(current)) != nullptr)
      {
        OperatorToken* tmpOperator;
        while(!stack.empty() && (tmpOperator = dynamic_cast<OperatorToken*>(stack.top())) != nullptr)
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
      else if((function = dynamic_cast<FunctionType*>(current)) != nullptr)
      {
        if(m_JuxtapositionOperator != nullptr)
        {
          if(((misc = dynamic_cast<MiscType*>(previous)) != nullptr && misc->GetObject() == ')') || (value = dynamic_cast<ValueType*>(previous)) != nullptr)
          {
            stack.push(m_JuxtapositionOperator.get());
          }
        }

        auto functionHelper = std::make_unique<FunctionHelperType>(*function);
        m_FunctionHelperCache.push_back(std::move(functionHelper));
        stack.push(m_FunctionHelperCache.back().get());
        functionHelpers.push(m_FunctionHelperCache.back().get());
      }
      else if((misc = dynamic_cast<MiscType*>(current)) != nullptr)
      {
        switch(misc->GetObject())
        {
          case '(':
          {
            if(m_JuxtapositionOperator != nullptr && (value = dynamic_cast<ValueType*>(previous)) != nullptr)
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
                if(previous != nullptr && ((misc = dynamic_cast<MiscType*>(previous)) == nullptr || misc->GetObject() != '('))
                {
                  functionHelpers.top()->IncrementArgumentCount();
                }

                functionHelpers.pop();
              }
            }

            misc = nullptr;
            while(!stack.empty() && ((misc = dynamic_cast<MiscType*>(stack.top())) == nullptr || misc->GetObject() != '('))
            {
              queue.push(stack.top());
              stack.pop();
            }

            if(misc == nullptr || misc->GetObject() != '(')
            {
              throw SyntaxException("Missing matching closing bracket");
            }

            stack.pop();

            if(!stack.empty() && dynamic_cast<FunctionHelperType*>(stack.top()))
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

            while(!stack.empty() && ((misc = dynamic_cast<MiscType*>(stack.top())) == nullptr || misc->GetObject() != '('))
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
      if(dynamic_cast<MiscType*>(stack.top()) != nullptr)
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
