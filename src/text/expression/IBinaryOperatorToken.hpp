#ifndef __IBINARYOPERATORTOKEN_HPP__
#define __IBINARYOPERATORTOKEN_HPP__

#include <functional>
#include "IOperatorToken.hpp"
#include "IValueToken.hpp"
#include "common/IIdentifiable.hpp"

class IBinaryOperatorToken : public virtual IOperatorToken, public IIdentifiable<std::string>
{
  public:
  virtual IValueToken* operator()(IValueToken*, IValueToken*) const = 0;

  virtual ~IBinaryOperatorToken() override = default;

  protected:
  IBinaryOperatorToken() = default;

  private:
  IBinaryOperatorToken(const IBinaryOperatorToken&) = delete;
  IBinaryOperatorToken& operator=(const IBinaryOperatorToken&) = delete;
};

#endif // __IBINARYOPERATORTOKEN_HPP__
