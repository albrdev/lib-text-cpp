#include "ExpressionPostfixParser.hpp"
#include "IOperatorToken.hpp"
#include "IFunctionToken.hpp"
#include "GenericToken.hpp"
#include "SyntaxException.hpp"

std::queue<IToken*> ExpressionPostfixParser::Execute(std::queue<IToken*>& tokens)
{
  using MiscType = GenericToken<char>;

  m_FunctionHelperCache.clear();

  const IOperatorToken* anyOperator = nullptr;
  const IFunctionToken* function    = nullptr;
  const MiscType* misc              = nullptr;

  std::queue<IToken*> queue;
  std::stack<IToken*> stack;
  std::stack<FunctionTokenHelper*> functionHelpers;

  const IToken* previous = nullptr;
  while(!tokens.empty())
  {
    const auto current = tokens.front();
    if(current->IsType<IValueToken>())
    {
      queue.push(current);
    }
    else if((anyOperator = current->AsPointer<IOperatorToken>()) != nullptr)
    {
      IOperatorToken* tmpOperator;
      while(!stack.empty() && (tmpOperator = stack.top()->AsPointer<IOperatorToken>()) != nullptr)
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
    else if((function = current->AsPointer<IFunctionToken>()) != nullptr)
    {
      auto functionHelper = std::make_unique<FunctionTokenHelper>(*function);
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

          if(!stack.empty() && stack.top()->IsType<FunctionTokenHelper>())
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

ExpressionPostfixParser::ExpressionPostfixParser()
    : m_FunctionHelperCache()
{}

ExpressionPostfixParser::ExpressionPostfixParser(const ExpressionPostfixParser& other)
    : m_FunctionHelperCache()
{
  static_cast<void>(other);
}

ExpressionPostfixParser::ExpressionPostfixParser(ExpressionPostfixParser&& other)
    : m_FunctionHelperCache()
{
  static_cast<void>(other);
}
