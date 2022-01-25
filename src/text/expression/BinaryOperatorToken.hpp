#ifndef __BINARYOPERATORTOKEN_HPP__
#define __BINARYOPERATORTOKEN_HPP__

#include "TokenBase.hpp"
#include "IBinaryOperatorToken.hpp"
#include "IValueToken.hpp"
#include <string>
#include <functional>

class BinaryOperatorToken : public IBinaryOperatorToken, public TokenBase<std::function<IValueToken*(IValueToken*, IValueToken*)>>
{
  public:
  virtual IValueToken* operator()(IValueToken* lhs, IValueToken* rhs) const override { return this->GetObject()(lhs, rhs); }
  virtual const std::string& GetIdentifier() const override { return m_Identifier; }
  virtual const int& GetPrecedence() const override { return m_Precedence; }
  virtual const Associativity& GetAssociativity() const override { return m_Associativity; }

  virtual std::string ToString() const override { return this->GetIdentifier(); }

  BinaryOperatorToken(const CallbackType& callback, const std::string& identifier, int precedence, Associativity associativity)
      : TokenBase<CallbackType>(callback)
      , m_Identifier(identifier)
      , m_Precedence(precedence)
      , m_Associativity(associativity)
  {}

  BinaryOperatorToken()
      : TokenBase<CallbackType>()
      , m_Identifier()
      , m_Precedence(0)
      , m_Associativity(Associativity::Any)
  {}

  BinaryOperatorToken(const BinaryOperatorToken& other)
      : TokenBase<CallbackType>(other)
      , m_Identifier(other.m_Identifier)
      , m_Precedence(other.m_Precedence)
      , m_Associativity(other.m_Associativity)
  {}

  BinaryOperatorToken(BinaryOperatorToken&& other)
      : TokenBase<CallbackType>(std::move(other))
      , m_Identifier(std::move(other.m_Identifier))
      , m_Precedence(std::move(other.m_Precedence))
      , m_Associativity(std::move(other.m_Associativity))
  {}

  virtual ~BinaryOperatorToken() override = default;

  BinaryOperatorToken& operator=(const BinaryOperatorToken& other)
  {
    TokenBase<CallbackType>::operator=(other);
    m_Identifier                     = other.m_Identifier;
    m_Precedence                     = other.m_Precedence;
    m_Associativity                  = other.m_Associativity;

    return *this;
  }

  BinaryOperatorToken& operator=(BinaryOperatorToken&& other)
  {
    TokenBase<CallbackType>::operator=(std::move(other));
    m_Identifier                     = std::move(other.m_Identifier);
    m_Precedence                     = std::move(other.m_Precedence);
    m_Associativity                  = std::move(other.m_Associativity);

    return *this;
  }

  private:
  using TokenBase<CallbackType>::GetObject;
  using TokenBase<CallbackType>::SetObject;

  std::string m_Identifier;
  int m_Precedence;
  Associativity m_Associativity;
};

#endif // __BINARYOPERATORTOKEN_HPP__
