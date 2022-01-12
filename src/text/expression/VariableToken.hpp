#ifndef __VARIABLETOKEN_HPP__
#define __VARIABLETOKEN_HPP__

#include "ValueToken.hpp"
#include "SymbolicToken.hpp"
#include <string>

template<class T>
class VariableToken : public ValueToken<T>, public SymbolicToken<std::string>
{
  public:
  virtual std::string ToString() const override { return this->GetIdentifier(); }

  VariableToken<T>& operator=(const std::nullptr_t value)
  {
    ValueToken<T>::operator=(value);
    return *this;
  }

  VariableToken<T>& operator=(const std::string& value)
  {
    ValueToken<T>::operator=(value);
    return *this;
  }

  VariableToken<T>& operator=(const T& value)
  {
    ValueToken<T>::operator=(value);
    return *this;
  }

  VariableToken()
      : ValueToken<T>()
      , SymbolicToken<std::string>()
  {}

  VariableToken(const std::string& identifier)
      : ValueToken<T>()
      , SymbolicToken<std::string>(identifier)
  {}

  explicit VariableToken(const std::string& identifier, const std::nullptr_t value)
      : ValueToken<T>(value)
      , SymbolicToken<std::string>(identifier)
  {}

  explicit VariableToken(const std::string& identifier, const std::string& value)
      : ValueToken<T>(value)
      , SymbolicToken<std::string>(identifier)
  {}

  explicit VariableToken(const std::string& identifier, const T& value)
      : ValueToken<T>(value)
      , SymbolicToken<std::string>(identifier)
  {}

  virtual ~VariableToken() override = default;

  VariableToken(const VariableToken<T>& other)
      : ValueToken<T>(other)
      , SymbolicToken<std::string>(other)
  {}

  VariableToken(VariableToken<T>&& other)
      : ValueToken<T>(std::move(other))
      , SymbolicToken<std::string>(std::move(other))
  {}

  VariableToken<T>& operator=(const VariableToken<T>& other)
  {
    ValueToken<T>::operator             =(other);
    SymbolicToken<std::string>::operator=(other);
    return *this;
  }

  VariableToken<T>& operator=(VariableToken<T>&& other)
  {
    ValueToken<T>::operator             =(std::move(other));
    SymbolicToken<std::string>::operator=(std::move(other));
    return *this;
  }

  protected:
  virtual void ThrowOnUninitializedAccess() const override { throw std::runtime_error("Accessing uninitialized variable: " + GetIdentifier()); }
};

#endif // __VARIABLETOKEN_HPP__
