#ifndef __BINARYOPERATORTOKEN_HPP__
#define __BINARYOPERATORTOKEN_HPP__

#include "TokenBase.hpp"
#include "OperatorToken.hpp"
#include "SymbolicToken.hpp"
#include <string>
#include <functional>

template<class T>
class BinaryOperatorToken : public TokenBase<std::function<T(T, T)>>, public OperatorToken, public SymbolicToken<std::string>
{
  public:
  using CallbackType = typename TokenBase<std::function<T(T, T)>>::ObjectType;

  virtual std::string ToString() const override { return this->GetIdentifier(); }

  T operator()(T lhs, T rhs) const { return this->GetObject()(lhs, rhs); }

  BinaryOperatorToken(const CallbackType& callback, const std::string& identifier, int precedence, Associativity associativity)
      : TokenBase<CallbackType>(callback)
      , OperatorToken(precedence, associativity)
      , SymbolicToken<std::string>(identifier)
  {}

  BinaryOperatorToken()
      : TokenBase<CallbackType>()
      , OperatorToken()
      , SymbolicToken<std::string>()
  {}

  BinaryOperatorToken(const BinaryOperatorToken<T>& other)
      : TokenBase<CallbackType>(other)
      , OperatorToken(other)
      , SymbolicToken<std::string>(other)
  {}

  BinaryOperatorToken(BinaryOperatorToken<T>&& other)
      : TokenBase<CallbackType>(std::move(other))
      , OperatorToken(std::move(other))
      , SymbolicToken<std::string>(std::move(other))
  {}

  virtual ~BinaryOperatorToken() override = default;

  BinaryOperatorToken<T>& operator=(const BinaryOperatorToken<T>& other)
  {
    TokenBase<CallbackType>::operator   =(other);
    OperatorToken::operator             =(other);
    SymbolicToken<std::string>::operator=(other);

    return *this;
  }

  BinaryOperatorToken<T>& operator=(BinaryOperatorToken<T>&& other)
  {
    TokenBase<CallbackType>::operator   =(std::move(other));
    OperatorToken::operator             =(std::move(other));
    SymbolicToken<std::string>::operator=(std::move(other));

    return *this;
  }

  private:
  using TokenBase<CallbackType>::GetObject;
  using TokenBase<CallbackType>::SetObject;
};

#endif // __BINARYOPERATORTOKEN_HPP__
