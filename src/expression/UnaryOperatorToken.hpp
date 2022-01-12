#ifndef __UNARYOPERATORTOKEN_HPP__
#define __UNARYOPERATORTOKEN_HPP__

#include "TokenBase.hpp"
#include "OperatorToken.hpp"
#include "SymbolicToken.hpp"
#include <string>
#include <functional>

template<class T>
class UnaryOperatorToken : public TokenBase<std::function<T(T)>>, public OperatorToken, public SymbolicToken<char>
{
  public:
  using CallbackType = typename TokenBase<std::function<T(T)>>::ObjectType;

  virtual std::string ToString() const override { return std::string(1u, this->GetIdentifier()); }

  T operator()(T rhs) const { return this->GetObject()(rhs); }

  UnaryOperatorToken(const CallbackType& callback, const char identifier, int precedence, Associativity associativity)
      : TokenBase<CallbackType>(callback)
      , OperatorToken(precedence, associativity)
      , SymbolicToken<char>(identifier)
  {}

  UnaryOperatorToken()
      : TokenBase<CallbackType>()
      , OperatorToken()
      , SymbolicToken<char>()
  {}

  UnaryOperatorToken(const UnaryOperatorToken<T>& other)
      : TokenBase<CallbackType>(other)
      , OperatorToken(other)
      , SymbolicToken<char>(other)
  {}

  UnaryOperatorToken(UnaryOperatorToken<T>&& other)
      : TokenBase<CallbackType>(std::move(other))
      , OperatorToken(std::move(other))
      , SymbolicToken<char>(std::move(other))
  {}

  virtual ~UnaryOperatorToken() override = default;

  UnaryOperatorToken<T>& operator=(const UnaryOperatorToken<T>& other)
  {
    TokenBase<CallbackType>::operator=(other);
    OperatorToken::operator          =(other);
    SymbolicToken<char>::operator    =(other);

    return *this;
  }

  UnaryOperatorToken<T>& operator=(UnaryOperatorToken<T>&& other)
  {
    TokenBase<CallbackType>::operator=(std::move(other));
    OperatorToken::operator          =(std::move(other));
    SymbolicToken<char>::operator    =(std::move(other));

    return *this;
  }

  private:
  using TokenBase<CallbackType>::GetObject;
  using TokenBase<CallbackType>::SetObject;
};

#endif // __UNARYOPERATORTOKEN_HPP__
