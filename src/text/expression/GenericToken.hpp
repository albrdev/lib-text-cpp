#ifndef __TEXT_EXPRESSION__GENERICTOKEN_HPP__
#define __TEXT_EXPRESSION__GENERICTOKEN_HPP__

#include "TokenBase.hpp"
#include <string>
#include <sstream>

namespace Text::Expression
{
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

    GenericToken<T>& operator=(const T& value)
    {
      TokenBase<T>::operator=(value);
      return *this;
    }

    GenericToken(const T& value)
        : TokenBase<T>(value)
    {}

    virtual ~GenericToken() override = default;

    GenericToken()
        : TokenBase<T>()
    {}

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
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__GENERICTOKEN_HPP__
