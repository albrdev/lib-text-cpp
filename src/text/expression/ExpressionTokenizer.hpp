#ifndef __TEXT_EXPRESSION__EXPRESSIONTOKENIZER_HPP__
#define __TEXT_EXPRESSION__EXPRESSIONTOKENIZER_HPP__

#include "IBinaryOperatorToken.hpp"
#include "IFunctionToken.hpp"
#include "IUnaryOperatorToken.hpp"
#include "IValueToken.hpp"
#include "IVariableToken.hpp"
#include "text/parsing/Parser.hpp"

#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Text::Expression
{
  class ExpressionTokenizer : public Parsing::Parser
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
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__EXPRESSIONTOKENIZER_HPP__
