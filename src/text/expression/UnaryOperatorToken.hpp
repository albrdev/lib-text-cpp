#ifndef __TEXT_EXPRESSION_UNARYOPERATORTOKEN_HPP__
#define __TEXT_EXPRESSION_UNARYOPERATORTOKEN_HPP__

#include "TokenBase.hpp"
#include "IUnaryOperatorToken.hpp"
#include "IValueToken.hpp"
#include <string>
#include <functional>

namespace text::expression
{
  class UnaryOperatorToken : public IUnaryOperatorToken, public TokenBase<std::function<IValueToken*(IValueToken*)>>
  {
    public:
    using CallbackType = TokenBase<std::function<IValueToken*(IValueToken*)>>::ObjectType;

    virtual IValueToken* operator()(IValueToken* rhs) const override;
    virtual const char& GetIdentifier() const override;
    virtual const int& GetPrecedence() const override;
    virtual const Associativity& GetAssociativity() const override;

    virtual std::string ToString() const override;

    UnaryOperatorToken(const UnaryOperatorToken::CallbackType& callback, const char identifier, int precedence, Associativity associativity);
    virtual ~UnaryOperatorToken() override = default;
    UnaryOperatorToken();
    UnaryOperatorToken(const UnaryOperatorToken& other);
    UnaryOperatorToken(UnaryOperatorToken&& other);
    UnaryOperatorToken& operator=(const UnaryOperatorToken& other);
    UnaryOperatorToken& operator=(UnaryOperatorToken&& other);

    private:
    using TokenBase<UnaryOperatorToken::CallbackType>::GetObject;
    using TokenBase<UnaryOperatorToken::CallbackType>::SetObject;

    char m_Identifier;
    int m_Precedence;
    Associativity m_Associativity;
  };
} // namespace text::expression

#endif // __TEXT_EXPRESSION_UNARYOPERATORTOKEN_HPP__
