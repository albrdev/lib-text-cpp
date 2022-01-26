#include "UnaryOperatorToken.hpp"

IValueToken* UnaryOperatorToken::operator()(IValueToken* rhs) const { return this->GetObject()(rhs); }
const char& UnaryOperatorToken::GetIdentifier() const { return m_Identifier; }
const int& UnaryOperatorToken::GetPrecedence() const { return m_Precedence; }
const Associativity& UnaryOperatorToken::GetAssociativity() const { return m_Associativity; }

std::string UnaryOperatorToken::ToString() const { return std::string(1u, this->GetIdentifier()); }

UnaryOperatorToken::UnaryOperatorToken(const UnaryOperatorToken::CallbackType& callback, const char identifier, int precedence, Associativity associativity)
    : IUnaryOperatorToken()
    , TokenBase<UnaryOperatorToken::CallbackType>(callback)
    , m_Identifier(identifier)
    , m_Precedence(precedence)
    , m_Associativity(associativity)
{}

UnaryOperatorToken::UnaryOperatorToken()
    : IUnaryOperatorToken()
    , TokenBase<UnaryOperatorToken::CallbackType>()
    , m_Identifier()
    , m_Precedence(0)
    , m_Associativity(Associativity::Any)
{}

UnaryOperatorToken::UnaryOperatorToken(const UnaryOperatorToken& other)
    : IUnaryOperatorToken()
    , TokenBase<UnaryOperatorToken::CallbackType>(other)
    , m_Identifier(other.m_Identifier)
    , m_Precedence(other.m_Precedence)
    , m_Associativity(other.m_Associativity)
{}

UnaryOperatorToken::UnaryOperatorToken(UnaryOperatorToken&& other)
    : IUnaryOperatorToken()
    , TokenBase<UnaryOperatorToken::CallbackType>(std::move(other))
    , m_Identifier(std::move(other.m_Identifier))
    , m_Precedence(std::move(other.m_Precedence))
    , m_Associativity(std::move(other.m_Associativity))
{}

UnaryOperatorToken& UnaryOperatorToken::operator=(const UnaryOperatorToken& other)
{
  TokenBase<UnaryOperatorToken::CallbackType>::operator=(other);
  m_Identifier                                         = other.m_Identifier;
  m_Precedence                                         = other.m_Precedence;
  m_Associativity                                      = other.m_Associativity;

  return *this;
}

UnaryOperatorToken& UnaryOperatorToken::operator=(UnaryOperatorToken&& other)
{
  TokenBase<UnaryOperatorToken::CallbackType>::operator=(std::move(other));
  m_Identifier                                         = std::move(other.m_Identifier);
  m_Precedence                                         = std::move(other.m_Precedence);
  m_Associativity                                      = std::move(other.m_Associativity);

  return *this;
}
