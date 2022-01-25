#ifndef __IVALUETOKEN_HPP__
#define __IVALUETOKEN_HPP__

#include <typeinfo>
#include "IToken.hpp"

class IValueToken : public virtual IToken
{
  public:
  virtual const std::type_info& GetType() const = 0;

  virtual ~IValueToken() override = default;

  protected:
  IValueToken() = default;

  private:
  IValueToken(const IValueToken&) = delete;
  IValueToken& operator=(const IValueToken&) = delete;
};

#endif // __IVALUETOKEN_HPP__
