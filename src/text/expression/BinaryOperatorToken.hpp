#ifndef __TEXT_EXPRESSION__BINARYOPERATORTOKEN_HPP__
#define __TEXT_EXPRESSION__BINARYOPERATORTOKEN_HPP__

#include "IBinaryOperatorToken.hpp"
#include "IValueToken.hpp"
#include <string>
#include <functional>

namespace Text::Expression
{
  class BinaryOperatorToken : public IBinaryOperatorToken
  {
    public:
    using CallbackType = std::function<IValueToken*(IValueToken*, IValueToken*)>;

    virtual IValueToken* operator()(IValueToken* lhs, IValueToken* rhs) const override;
    virtual const std::string& GetIdentifier() const override;
    virtual const int& GetPrecedence() const override;
    virtual const Associativity& GetAssociativity() const override;

    virtual std::string ToString() const override;

    BinaryOperatorToken(const BinaryOperatorToken::CallbackType& callback, const std::string& identifier, int precedence, Associativity associativity);
    virtual ~BinaryOperatorToken() override = default;
    BinaryOperatorToken();
    BinaryOperatorToken(const BinaryOperatorToken& other);
    BinaryOperatorToken(BinaryOperatorToken&& other);
    BinaryOperatorToken& operator=(const BinaryOperatorToken& other);
    BinaryOperatorToken& operator=(BinaryOperatorToken&& other);

    private:
    CallbackType m_Callback;
    std::string m_Identifier;
    int m_Precedence;
    Associativity m_Associativity;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__BINARYOPERATORTOKEN_HPP__
