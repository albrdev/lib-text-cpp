#ifndef __TEXT_EXPRESSION_IUNARYOPERATORTOKEN_HPP__
#define __TEXT_EXPRESSION_IUNARYOPERATORTOKEN_HPP__

#include <functional>
#include "IOperatorToken.hpp"
#include "IValueToken.hpp"
#include "common/IIdentifiable.hpp"

namespace Text::Expression
{
  class IUnaryOperatorToken : public virtual IOperatorToken, public IIdentifiable<char>
  {
    public:
    virtual IValueToken* operator()(IValueToken*) const = 0;

    virtual ~IUnaryOperatorToken() override = default;

    protected:
    IUnaryOperatorToken() = default;

    private:
    IUnaryOperatorToken(const IUnaryOperatorToken&) = delete;
    IUnaryOperatorToken& operator=(const IUnaryOperatorToken&) = delete;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION_IUNARYOPERATORTOKEN_HPP__
