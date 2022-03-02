#ifndef __TEXT_EXPRESSION_EXPRESSIONTOKENIZER_HPP__
#define __TEXT_EXPRESSION_EXPRESSIONTOKENIZER_HPP__

#include <string>
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "text/parse/Parser.hpp"
#include "IValueToken.hpp"
#include "IVariableToken.hpp"
#include "IUnaryOperatorToken.hpp"
#include "IBinaryOperatorToken.hpp"
#include "IFunctionToken.hpp"

namespace text::expression
{
  class ExpressionTokenizer : public parse::Parser
  {
    public:
    constexpr static char DefaultTerminatorCharacters[] = ";#";

    std::queue<IToken*> Execute(const std::string& expression,
                                const std::unordered_map<char, IUnaryOperatorToken*>* unaryOperators,
                                const std::unordered_map<std::string, IBinaryOperatorToken*>* binaryOperators,
                                const std::unordered_map<std::string, IVariableToken*>* variables,
                                const std::unordered_map<std::string, IFunctionToken*>* functions);

    void SetOnParseNumberCallback(const std::function<IValueToken*(const std::string&)>& value);
    void SetOnParseStringCallback(const std::function<IValueToken*(const std::string&)>& value);
    void SetOnUnknownIdentifierCallback(const std::function<IValueToken*(const std::string&)>& value);
    void SetJuxtapositionOperator(IBinaryOperatorToken* value);

    const std::string& GetTerminatorCharacters() const;
    void SetTerminatorCharacters(const std::string& value);

    virtual ~ExpressionTokenizer() override = default;
    ExpressionTokenizer();
    ExpressionTokenizer(const ExpressionTokenizer& other);
    ExpressionTokenizer(ExpressionTokenizer&& other);

    private:
    using Parser::SetText;

    std::function<IValueToken*(const std::string&)> m_OnParseNumberCallback;
    std::function<IValueToken*(const std::string&)> m_OnParseStringCallback;
    std::function<IValueToken*(const std::string&)> m_OnParseUnknownIdentifier;
    IBinaryOperatorToken* m_pJuxtapositionOperator;
    std::string m_TerminatorCharacters;

    std::vector<std::unique_ptr<IToken>> m_TokenCache;
  };
} // namespace text::expression

#endif // __TEXT_EXPRESSION_EXPRESSIONTOKENIZER_HPP__
