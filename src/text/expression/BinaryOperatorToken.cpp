#include "BinaryOperatorToken.hpp"

namespace Text::Expression
{
  IValueToken* BinaryOperatorToken::operator()(IValueToken* lhs, IValueToken* rhs) const { return m_Callback(lhs, rhs); }
  const std::string& BinaryOperatorToken::GetIdentifier() const { return m_Identifier; }
  const int& BinaryOperatorToken::GetPrecedence() const { return m_Precedence; }
  const Associativity& BinaryOperatorToken::GetAssociativity() const { return m_Associativity; }

  std::string BinaryOperatorToken::ToString() const { return this->GetIdentifier(); }

  BinaryOperatorToken::BinaryOperatorToken(const std::string& identifier,
                                           const BinaryOperatorToken::CallbackType& callback,
                                           int precedence,
                                           Associativity associativity)
      : IBinaryOperatorToken()
      , m_Callback(callback)
      , m_Identifier(identifier)
      , m_Precedence(precedence)
      , m_Associativity(associativity)
  {}

  BinaryOperatorToken::BinaryOperatorToken()
      : IBinaryOperatorToken()
      , m_Callback()
      , m_Identifier()
      , m_Precedence(0)
      , m_Associativity(Associativity::Any)
  {}

  BinaryOperatorToken::BinaryOperatorToken(const BinaryOperatorToken& other)
      : IToken()
      , IOperatorToken()
      , IBinaryOperatorToken()
      , m_Callback(other.m_Callback)
      , m_Identifier(other.m_Identifier)
      , m_Precedence(other.m_Precedence)
      , m_Associativity(other.m_Associativity)
  {}

  BinaryOperatorToken::BinaryOperatorToken(BinaryOperatorToken&& other)
      : IBinaryOperatorToken()
      , m_Callback(std::move(other.m_Callback))
      , m_Identifier(std::move(other.m_Identifier))
      , m_Precedence(std::move(other.m_Precedence))
      , m_Associativity(std::move(other.m_Associativity))
  {}

  BinaryOperatorToken& BinaryOperatorToken::operator=(const BinaryOperatorToken& other)
  {
    m_Callback      = other.m_Callback;
    m_Identifier    = other.m_Identifier;
    m_Precedence    = other.m_Precedence;
    m_Associativity = other.m_Associativity;

    return *this;
  }

  BinaryOperatorToken& BinaryOperatorToken::operator=(BinaryOperatorToken&& other)
  {
    m_Callback      = std::move(other.m_Callback);
    m_Identifier    = std::move(other.m_Identifier);
    m_Precedence    = std::move(other.m_Precedence);
    m_Associativity = std::move(other.m_Associativity);

    return *this;
  }
} // namespace Text::Expression
