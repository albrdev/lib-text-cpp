#ifndef __TEXT_EXPRESSION__ITOKEN_HPP__
#define __TEXT_EXPRESSION__ITOKEN_HPP__

#include "common/IType.hpp"
#include "common/IOutput.hpp"

namespace Text::Expression
{
  class IToken : public IType, public IOutput
  {
    public:
    virtual ~IToken() override = default;

    protected:
    IToken() = default;

    private:
    IToken(const IToken&) = delete;
    IToken& operator=(const IToken&) = delete;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__ITOKEN_HPP__
