#include "ExpressionPostfixParser.hpp"
#include "GenericToken.hpp"
#include "IOperatorToken.hpp"
#include "IValueToken.hpp"
#include "text/exception/SyntaxError.hpp"

#include <stack>

namespace Text::Expression
{
  std::queue<IToken*> ExpressionPostfixParser::Execute(std::queue<IToken*>& tokens)
  {
    using MiscType = GenericToken<char>;

    m_FunctionCache.clear();

    const IOperatorToken* anyOperator = nullptr;
    const IFunctionToken* function    = nullptr;
    const MiscType* misc              = nullptr;

    std::queue<IToken*> queue;
    std::stack<IToken*> stack;
    std::stack<FunctionTokenHelper*> functions;

    const IToken* previous = nullptr;
    while(!tokens.empty())
    {
      const auto current = tokens.front();
      if(current->IsType<IValueToken>())
      {
        queue.push(current);
      }
      else if((anyOperator = current->As<IOperatorToken*>()) != nullptr)
      {
        IOperatorToken* tmpOperator;
        while(!stack.empty() && (tmpOperator = stack.top()->As<IOperatorToken*>()) != nullptr)
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
      else if((function = current->As<IFunctionToken*>()) != nullptr)
      {
        auto functionHelper = std::make_unique<FunctionTokenHelper>(*function);
        m_FunctionCache.push_back(std::move(functionHelper));
        stack.push(m_FunctionCache.back().get());
        functions.push(m_FunctionCache.back().get());
      }
      else if((misc = current->As<MiscType*>()) != nullptr)
      {
        switch(*misc)
        {
          case '(':
          {
            if(!functions.empty())
            {
              functions.top()->m_BracketBalance++;
            }

            stack.push(current);
            break;
          }
          case ')':
          {
            if(!functions.empty())
            {
              functions.top()->m_BracketBalance--;

              if(functions.top()->m_BracketBalance == 0)
              {
                if(previous != nullptr && ((misc = previous->As<const MiscType*>()) == nullptr || *misc != '('))
                {
                  functions.top()->m_ArgumentCount++;
                }

                functions.pop();
              }
            }

            misc = nullptr;
            while(!stack.empty() && ((misc = stack.top()->As<MiscType*>()) == nullptr || *misc != '('))
            {
              queue.push(stack.top());
              stack.pop();
            }

            if(misc == nullptr || *misc != '(')
            {
              throw Exception::SyntaxError("Missing matching opening bracket");
            }

            stack.pop();

            if(!stack.empty() && stack.top()->IsType<IFunctionToken>())
            {
              queue.push(stack.top());
              stack.pop();
            }

            break;
          }
          case ',':
          {
            if(!functions.empty())
            {
              functions.top()->m_ArgumentCount++;
            }

            while(!stack.empty() && ((misc = stack.top()->As<MiscType*>()) == nullptr || *misc != '('))
            {
              queue.push(stack.top());
              stack.pop();
            }

            if(misc == nullptr || *misc != '(')
            {
              throw Exception::SyntaxError("Missing matching opening bracket");
            }

            break;
          }
        }
      }
      else
      {
        throw Exception::SyntaxError("Unknown token encountered during postfix process: " + current->ToString());
      }

      previous = current;
      tokens.pop();
    }

    while(!stack.empty())
    {
      if((misc = stack.top()->As<MiscType*>()) != nullptr && (*misc == '(' || *misc == ')'))
      {
        throw Exception::SyntaxError("Missing matching closing bracket");
      }

      queue.push(stack.top());
      stack.pop();
    }

    return queue;
  }

  ExpressionPostfixParser::ExpressionPostfixParser()
      : m_FunctionCache()
  {}

  ExpressionPostfixParser::ExpressionPostfixParser(const ExpressionPostfixParser& other)
      : m_FunctionCache()
  {
    static_cast<void>(other);
  }

  ExpressionPostfixParser::ExpressionPostfixParser(ExpressionPostfixParser&& other)
      : m_FunctionCache()
  {
    static_cast<void>(other);
  }
} // namespace Text::Expression
