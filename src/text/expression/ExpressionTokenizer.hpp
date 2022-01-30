#ifndef __EXPRESSIONTOKENIZER_HPP__
#define __EXPRESSIONTOKENIZER_HPP__

#include <string>
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "text/Parser.hpp"
#include "IValueToken.hpp"
#include "IVariableToken.hpp"
#include "IUnaryOperatorToken.hpp"
#include "IBinaryOperatorToken.hpp"
#include "IFunctionToken.hpp"

class ExpressionTokenizer : public Parser
{
  public:
  constexpr static char DefaultCommentIdentifier = '#';

  std::queue<IToken*> Execute(const std::string& expression,
                              const std::unordered_map<char, IUnaryOperatorToken*>* unaryOperators,
                              const std::unordered_map<std::string, IBinaryOperatorToken*>* binaryOperators,
                              const std::unordered_map<std::string, IVariableToken*>* variables,
                              const std::unordered_map<std::string, IFunctionToken*>* functions);

  void SetOnParseNumberCallback(const std::function<IValueToken*(const std::string&)>& value);
  void SetOnParseStringCallback(const std::function<IValueToken*(const std::string&)>& value);
  void SetOnUnknownIdentifierCallback(const std::function<IValueToken*(const std::string&)>& value);
  void SetJuxtapositionOperator(IBinaryOperatorToken* value);

  const char& GetCommentIdentifier() const;
  void SetCommentIdentifier(char value);

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
  char m_CommentIdentifier;

  std::vector<std::unique_ptr<IToken>> m_TokenCache;
};

#endif // __EXPRESSIONTOKENIZER_HPP__
