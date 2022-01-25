#ifndef __IUNARYOPERATORTOKEN_HPP__
#define __IUNARYOPERATORTOKEN_HPP__

#include "IOperatorToken.hpp"
#include "IValueToken.hpp"
#include "common/IIdentifiable.hpp"

class IUnaryOperatorToken : public virtual IOperatorToken, public IIdentifiable<char>
{
  public:
  using CallbackType = typename TokenBase<std::function<IValueToken*(IValueToken*)>>::ObjectType;

  virtual IValueToken* operator()(IValueToken*) const = 0;

  virtual ~IUnaryOperatorToken() override = default;

  protected:
  IUnaryOperatorToken() = default;

  private:
  IUnaryOperatorToken(const IUnaryOperatorToken&) = delete;
  IUnaryOperatorToken& operator=(const IUnaryOperatorToken&) = delete;
};

#endif // __IUNARYOPERATORTOKEN_HPP__
