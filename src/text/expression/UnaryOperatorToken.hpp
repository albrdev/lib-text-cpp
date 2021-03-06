#ifndef __TEXT_EXPRESSION__UNARYOPERATORTOKEN_HPP__
#define __TEXT_EXPRESSION__UNARYOPERATORTOKEN_HPP__

#include "IUnaryOperatorToken.hpp"

#include <functional>

namespace Text::Expression
{
  class UnaryOperatorToken : public IUnaryOperatorToken
  {
    public:
    using CallbackType = std::function<IValueToken*(IValueToken*)>;

    virtual IValueToken* operator()(IValueToken* rhs) const override;
    virtual const char& GetIdentifier() const override;
    virtual const int& GetPrecedence() const override;
    virtual const Associativity& GetAssociativity() const override;

    virtual std::string ToString() const override;

    UnaryOperatorToken(const char identifier, const UnaryOperatorToken::CallbackType& callback, int precedence, Associativity associativity);
    virtual ~UnaryOperatorToken() override = default;
    UnaryOperatorToken();
    UnaryOperatorToken(const UnaryOperatorToken& other);
    UnaryOperatorToken(UnaryOperatorToken&& other);
    UnaryOperatorToken& operator=(const UnaryOperatorToken& other);
    UnaryOperatorToken& operator=(UnaryOperatorToken&& other);

    protected:
    virtual bool Equals(const Common::IEquals& other) const override;

    private:
    CallbackType m_Callback;
    char m_Identifier;
    int m_Precedence;
    Associativity m_Associativity;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__UNARYOPERATORTOKEN_HPP__
