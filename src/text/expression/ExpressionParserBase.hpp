#ifndef __TEXT_EXPRESSION__EXPRESSIONPARSERBASE_HPP__
#define __TEXT_EXPRESSION__EXPRESSIONPARSERBASE_HPP__

#include "ExpressionEvaluator.hpp"
#include "ExpressionPostfixParser.hpp"
#include "ExpressionTokenizer.hpp"
#include "Token.hpp"

#include <queue>

namespace Text::Expression
{
  class ExpressionParserBase : public ExpressionTokenizer, public ExpressionPostfixParser, public ExpressionEvaluator
  {
    public:
    std::queue<IToken*> Parse(const std::string& expression);
    IValueToken* Evaluate(std::queue<IToken*>& postfix);

    void SetUnaryOperators(const std::unordered_map<char, IUnaryOperatorToken*>* value);
    void SetBinaryOperators(const std::unordered_map<std::string, IBinaryOperatorToken*>* value);
    void SetVariables(const std::unordered_map<std::string, IVariableToken*>* value);
    void SetFunctions(const std::unordered_map<std::string, IFunctionToken*>* value);

    virtual ~ExpressionParserBase() override = default;
    ExpressionParserBase();
    ExpressionParserBase(const ExpressionParserBase& other);
    ExpressionParserBase(ExpressionParserBase&& other);

    private:
    using ExpressionTokenizer::Execute;
    using ExpressionPostfixParser::Execute;
    using ExpressionEvaluator::Execute;

    const std::unordered_map<char, IUnaryOperatorToken*>* m_pUnaryOperators;
    const std::unordered_map<std::string, IBinaryOperatorToken*>* m_pBinaryOperators;
    const std::unordered_map<std::string, IVariableToken*>* m_pVariables;
    const std::unordered_map<std::string, IFunctionToken*>* m_pFunctions;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__EXPRESSIONPARSERBASE_HPP__
