#ifndef __TEXT_EXPRESSION_IVALUETOKEN_HPP__
#define __TEXT_EXPRESSION_IVALUETOKEN_HPP__

#include <typeinfo>
#include "IToken.hpp"

namespace text::expression
{
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
} // namespace text::expression

#endif // __TEXT_EXPRESSION_IVALUETOKEN_HPP__
