#ifndef __IFUNCTIONTOKEN_HPP__
#define __IFUNCTIONTOKEN_HPP__

#include <vector>
#include <functional>
#include "IToken.hpp"
#include "IValueToken.hpp"
#include "common/IIdentifiable.hpp"

class IFunctionToken : public virtual IToken, public IIdentifiable<std::string>
{
  public:
  virtual IValueToken* operator()(const std::vector<IValueToken*>&) const = 0;

  virtual const std::size_t& GetMinArgumentCount() const = 0;
  virtual const std::size_t& GetMaxArgumentCount() const = 0;

  virtual ~IFunctionToken() override = default;

  protected:
  IFunctionToken() = default;

  private:
  IFunctionToken(const IFunctionToken&) = delete;
  IFunctionToken& operator=(const IFunctionToken&) = delete;
};

#endif // __IFUNCTIONTOKEN_HPP__
