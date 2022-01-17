#ifndef __VARIABLETOKEN_HPP__
#define __VARIABLETOKEN_HPP__

#include "ValueToken.hpp"
#include "SymbolicToken.hpp"
#include <string>

template<class... Ts>
class VariableToken : public ValueToken<Ts...>, public SymbolicToken<std::string>
{
  public:
  virtual std::string ToString() const override { return this->GetIdentifier(); }

  template<class T>
  VariableToken<Ts...>& operator=(const T& value)
  {
    ValueToken<Ts...>::operator=(value);
    return *this;
  }

  VariableToken()
      : ValueToken<Ts...>()
      , SymbolicToken<std::string>()
  {}

  VariableToken(const std::string& identifier)
      : ValueToken<Ts...>()
      , SymbolicToken<std::string>(identifier)
  {}

  template<class T>
  explicit VariableToken(const std::string& identifier, const T& value)
      : ValueToken<Ts...>(value)
      , SymbolicToken<std::string>(identifier)
  {}

  virtual ~VariableToken() override = default;

  VariableToken(const VariableToken<Ts...>& other)
      : ValueToken<Ts...>(other)
      , SymbolicToken<std::string>(other)
  {}

  VariableToken(VariableToken<Ts...>&& other)
      : ValueToken<Ts...>(std::move(other))
      , SymbolicToken<std::string>(std::move(other))
  {}

  VariableToken<Ts...>& operator=(const VariableToken<Ts...>& other)
  {
    ValueToken<Ts...>::operator         =(other);
    SymbolicToken<std::string>::operator=(other);
    return *this;
  }

  VariableToken<Ts...>& operator=(VariableToken<Ts...>&& other)
  {
    ValueToken<Ts...>::operator         =(std::move(other));
    SymbolicToken<std::string>::operator=(std::move(other));
    return *this;
  }

  protected:
  virtual void ThrowOnUninitializedAccess() const override { throw std::runtime_error("Accessing uninitialized variable: " + GetIdentifier()); }
};

#endif // __VARIABLETOKEN_HPP__
