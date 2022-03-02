#ifndef __TEXT_EXPRESSION_IVARIABLETOKEN_HPP__
#define __TEXT_EXPRESSION_IVARIABLETOKEN_HPP__

#include "IValueToken.hpp"
#include "common/IIdentifiable.hpp"

namespace text::expression
{
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
} // namespace text::expression

#endif // __TEXT_EXPRESSION_IVARIABLETOKEN_HPP__
