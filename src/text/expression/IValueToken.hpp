#ifndef __TEXT_EXPRESSION__IVALUETOKEN_HPP__
#define __TEXT_EXPRESSION__IVALUETOKEN_HPP__

#include "IToken.hpp"

namespace Text::Expression
{
  class IValueToken : public virtual IToken
  {
    public:
    virtual const std::type_info& GetType() const = 0;

    virtual ~IValueToken() override = default;

    protected:
    IValueToken() = default;

    private:
    IValueToken(const IValueToken&)            = delete;
    IValueToken& operator=(const IValueToken&) = delete;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__IVALUETOKEN_HPP__
