#ifndef __UNARYOPERATORTOKEN_HPP__
#define __UNARYOPERATORTOKEN_HPP__

#include "TokenBase.hpp"
#include "IUnaryOperatorToken.hpp"
#include "IValueToken.hpp"
#include <string>
#include <functional>

class UnaryOperatorToken : public IUnaryOperatorToken, public TokenBase<std::function<IValueToken*(IValueToken*)>>
{
  public:
  virtual IValueToken* operator()(IValueToken* rhs) const override { return this->GetObject()(rhs); }
  virtual const char& GetIdentifier() const override { return m_Identifier; }
  virtual const int& GetPrecedence() const override { return m_Precedence; }
  virtual const Associativity& GetAssociativity() const override { return m_Associativity; }

  virtual std::string ToString() const override { return std::string(1u, this->GetIdentifier()); }

  UnaryOperatorToken(const CallbackType& callback, const char identifier, int precedence, Associativity associativity)
      : IUnaryOperatorToken()
      , TokenBase<CallbackType>(callback)
      , m_Identifier(identifier)
      , m_Precedence(precedence)
      , m_Associativity(associativity)
  {}

  UnaryOperatorToken()
      : IUnaryOperatorToken()
      , TokenBase<CallbackType>()
      , m_Identifier()
      , m_Precedence(0)
      , m_Associativity(Associativity::Any)
  {}

  UnaryOperatorToken(const UnaryOperatorToken& other)
      : IUnaryOperatorToken()
      , TokenBase<CallbackType>(other)
      , m_Identifier(other.m_Identifier)
      , m_Precedence(other.m_Precedence)
      , m_Associativity(other.m_Associativity)
  {}

  UnaryOperatorToken(UnaryOperatorToken&& other)
      : IUnaryOperatorToken()
      , TokenBase<CallbackType>(std::move(other))
      , m_Identifier(std::move(other.m_Identifier))
      , m_Precedence(std::move(other.m_Precedence))
      , m_Associativity(std::move(other.m_Associativity))
  {}

  virtual ~UnaryOperatorToken() override = default;

  UnaryOperatorToken& operator=(const UnaryOperatorToken& other)
  {
    TokenBase<CallbackType>::operator=(other);
    m_Identifier                     = other.m_Identifier;
    m_Precedence                     = other.m_Precedence;
    m_Associativity                  = other.m_Associativity;

    return *this;
  }

  UnaryOperatorToken& operator=(UnaryOperatorToken&& other)
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

  char m_Identifier;
  int m_Precedence;
  Associativity m_Associativity;
};

#endif // __UNARYOPERATORTOKEN_HPP__
