#include "UnaryOperatorToken.hpp"

namespace Text::Expression
{
  IValueToken* UnaryOperatorToken::operator()(IValueToken* rhs) const { return m_Callback(rhs); }
  const char& UnaryOperatorToken::GetIdentifier() const { return m_Identifier; }
  const int& UnaryOperatorToken::GetPrecedence() const { return m_Precedence; }
  const Associativity& UnaryOperatorToken::GetAssociativity() const { return m_Associativity; }

  std::string UnaryOperatorToken::ToString() const { return std::string(1u, this->GetIdentifier()); }

  bool UnaryOperatorToken::Equals(const Common::IEquals& other) const
  {
    auto tmpObject = dynamic_cast<decltype(this)>(&other);
    return (tmpObject != nullptr) &&
           ((m_Identifier == tmpObject->m_Identifier) && (m_Precedence == tmpObject->m_Precedence) && (m_Associativity == tmpObject->m_Associativity));
  }

  UnaryOperatorToken::UnaryOperatorToken(const char identifier, const UnaryOperatorToken::CallbackType& callback, int precedence, Associativity associativity)
      : IUnaryOperatorToken()
      , m_Callback(callback)
      , m_Identifier(identifier)
      , m_Precedence(precedence)
      , m_Associativity(associativity)
  {}

  UnaryOperatorToken::UnaryOperatorToken()
      : IUnaryOperatorToken()
      , m_Callback()
      , m_Identifier()
      , m_Precedence(0)
      , m_Associativity(Associativity::Any)
  {}

  UnaryOperatorToken::UnaryOperatorToken(const UnaryOperatorToken& other)
      : IToken()
      , IOperatorToken()
      , IUnaryOperatorToken()
      , m_Callback(other.m_Callback)
      , m_Identifier(other.m_Identifier)
      , m_Precedence(other.m_Precedence)
      , m_Associativity(other.m_Associativity)
  {}

  UnaryOperatorToken::UnaryOperatorToken(UnaryOperatorToken&& other)
      : IUnaryOperatorToken()
      , m_Callback(std::move(other.m_Callback))
      , m_Identifier(std::move(other.m_Identifier))
      , m_Precedence(std::move(other.m_Precedence))
      , m_Associativity(std::move(other.m_Associativity))
  {}

  UnaryOperatorToken& UnaryOperatorToken::operator=(const UnaryOperatorToken& other)
  {
    m_Callback      = other.m_Callback;
    m_Identifier    = other.m_Identifier;
    m_Precedence    = other.m_Precedence;
    m_Associativity = other.m_Associativity;

    return *this;
  }

  UnaryOperatorToken& UnaryOperatorToken::operator=(UnaryOperatorToken&& other)
  {
    m_Callback      = std::move(other.m_Callback);
    m_Identifier    = std::move(other.m_Identifier);
    m_Precedence    = std::move(other.m_Precedence);
    m_Associativity = std::move(other.m_Associativity);

    return *this;
  }
} // namespace Text::Expression
