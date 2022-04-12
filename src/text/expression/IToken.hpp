#ifndef __TEXT_EXPRESSION__ITOKEN_HPP__
#define __TEXT_EXPRESSION__ITOKEN_HPP__

#include "common/IEquals.hpp"
#include "common/IOutput.hpp"
#include "common/IType.hpp"

namespace Text::Expression
{
  class IToken : public Common::IType, public Common::IOutput, public Common::IEquals
  {
    public:
    bool operator==(const IToken& rhs) const { return Equals(rhs); }
    bool operator!=(const IToken& rhs) const { return !Equals(rhs); }

    virtual ~IToken() override = default;

    protected:
    IToken() = default;

    private:
    IToken(const IToken&) = delete;
    IToken& operator=(const IToken&) = delete;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__ITOKEN_HPP__
