#ifndef __TEXT_EXPRESSION__IVARIABLETOKEN_HPP__
#define __TEXT_EXPRESSION__IVARIABLETOKEN_HPP__

#include "IValueToken.hpp"
#include "common/IIdentifiable.hpp"

namespace Text::Expression
{
  class IVariableToken : public virtual IValueToken, public Common::IIdentifiable<std::string>
  {
    public:
    virtual ~IVariableToken() override = default;

    protected:
    IVariableToken() = default;

    private:
    IVariableToken(const IVariableToken&) = delete;
    IVariableToken& operator=(const IVariableToken&) = delete;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__IVARIABLETOKEN_HPP__
