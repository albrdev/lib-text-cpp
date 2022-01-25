#ifndef __IVARIABLETOKEN_HPP__
#define __IVARIABLETOKEN_HPP__

#include "IValueToken.hpp"
#include "common/IIdentifiable.hpp"

class IVariableToken : public virtual IValueToken, public IIdentifiable<std::string>
{
  public:
  virtual ~IVariableToken() override = default;

  protected:
  IVariableToken() = default;

  private:
  IVariableToken(const IVariableToken&) = delete;
  IVariableToken& operator=(const IVariableToken&) = delete;
};

#endif // __IVARIABLETOKEN_HPP__
