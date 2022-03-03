#include "BinaryOperatorToken.hpp"

namespace Text::Expression
{
  IValueToken* BinaryOperatorToken::operator()(IValueToken* lhs, IValueToken* rhs) const { return this->GetObject()(lhs, rhs); }
  const std::string& BinaryOperatorToken::GetIdentifier() const { return m_Identifier; }
  const int& BinaryOperatorToken::GetPrecedence() const { return m_Precedence; }
  const Associativity& BinaryOperatorToken::GetAssociativity() const { return m_Associativity; }

  std::string BinaryOperatorToken::ToString() const { return this->GetIdentifier(); }

  BinaryOperatorToken::BinaryOperatorToken(const BinaryOperatorToken::CallbackType& callback,
                                           const std::string& identifier,
                                           int precedence,
                                           Associativity associativity)
      : IBinaryOperatorToken()
      , TokenBase<BinaryOperatorToken::CallbackType>(callback)
      , m_Identifier(identifier)
      , m_Precedence(precedence)
      , m_Associativity(associativity)
  {}

  BinaryOperatorToken::BinaryOperatorToken()
      : IBinaryOperatorToken()
      , TokenBase<BinaryOperatorToken::CallbackType>()
      , m_Identifier()
      , m_Precedence(0)
      , m_Associativity(Associativity::Any)
  {}

  BinaryOperatorToken::BinaryOperatorToken(const BinaryOperatorToken& other)
      : IToken()
      , IOperatorToken()
      , IBinaryOperatorToken()
      , TokenBase<BinaryOperatorToken::CallbackType>(other)
      , m_Identifier(other.m_Identifier)
      , m_Precedence(other.m_Precedence)
      , m_Associativity(other.m_Associativity)
  {}

  BinaryOperatorToken::BinaryOperatorToken(BinaryOperatorToken&& other)
      : IBinaryOperatorToken()
      , TokenBase<BinaryOperatorToken::CallbackType>(std::move(other))
      , m_Identifier(std::move(other.m_Identifier))
      , m_Precedence(std::move(other.m_Precedence))
      , m_Associativity(std::move(other.m_Associativity))
  {}

  BinaryOperatorToken& BinaryOperatorToken::operator=(const BinaryOperatorToken& other)
  {
    TokenBase<BinaryOperatorToken::CallbackType>::operator=(other);
    m_Identifier                                          = other.m_Identifier;
    m_Precedence                                          = other.m_Precedence;
    m_Associativity                                       = other.m_Associativity;

    return *this;
  }

  BinaryOperatorToken& BinaryOperatorToken::operator=(BinaryOperatorToken&& other)
  {
    TokenBase<BinaryOperatorToken::CallbackType>::operator=(std::move(other));
    m_Identifier                                          = std::move(other.m_Identifier);
    m_Precedence                                          = std::move(other.m_Precedence);
    m_Associativity                                       = std::move(other.m_Associativity);

    return *this;
  }
} // namespace Text::Expression
