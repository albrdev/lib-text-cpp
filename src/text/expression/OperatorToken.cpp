#include "OperatorToken.hpp"

const int& OperatorToken::GetPrecedence() const { return m_Precedence; }
const Associativity& OperatorToken::GetAssociativity() const { return m_Associativity; }

OperatorToken::OperatorToken(int precedence, Associativity associativity)
    : m_Precedence(precedence)
    , m_Associativity(associativity)
{}

OperatorToken::OperatorToken()
    : m_Precedence(0)
    , m_Associativity(Associativity::Any)
{}

OperatorToken::OperatorToken(const OperatorToken& other)
    : m_Precedence(other.m_Precedence)
    , m_Associativity(other.m_Associativity)
{}

OperatorToken::OperatorToken(OperatorToken&& other)
    : m_Precedence(std::move(other.m_Precedence))
    , m_Associativity(std::move(other.m_Associativity))
{}

OperatorToken& OperatorToken::operator=(const OperatorToken& value)
{
  m_Precedence    = value.m_Precedence;
  m_Associativity = value.m_Associativity;

  return *this;
}

OperatorToken& OperatorToken::operator=(OperatorToken&& value)
{
  m_Precedence    = std::move(value.m_Precedence);
  m_Associativity = std::move(value.m_Associativity);

  return *this;
}
