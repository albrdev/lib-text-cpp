#ifndef __GENERICTOKEN_HPP__
#define __GENERICTOKEN_HPP__

#include "TokenBase.hpp"
#include <string>
#include <sstream>

template<class T>
class GenericToken : public TokenBase<T>
{
  public:
  virtual std::string ToString() const override
  {
    std::ostringstream oss;
    oss << this->GetObject();
    return oss.str();
  }

  GenericToken(const T& value)
      : TokenBase<T>(value)
  {}

  virtual ~GenericToken() override = default;

  GenericToken(const GenericToken<T>& other)
      : TokenBase<T>(other)
  {}

  GenericToken(GenericToken<T>&& other)
      : TokenBase<T>(std::move(other))
  {}

  GenericToken<T>& operator=(const GenericToken<T>& other)
  {
    TokenBase<T>::operator=(other);
    return *this;
  }

  GenericToken<T>& operator=(GenericToken<T>&& other)
  {
    TokenBase<T>::operator=(std::move(other));
    return *this;
  }
};

#endif // __GENERICTOKEN_HPP__
