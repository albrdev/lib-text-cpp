#ifndef __TEXT_EXPRESSION__IFUNCTIONTOKEN_HPP__
#define __TEXT_EXPRESSION__IFUNCTIONTOKEN_HPP__

#include "IToken.hpp"
#include "common/IIdentifiable.hpp"

#include <vector>

namespace Text::Expression
{
  class IValueToken;

  class IFunctionToken : public virtual IToken, public Common::IIdentifiable<std::string>
  {
    public:
    virtual IValueToken* operator()(const std::vector<IValueToken*>&) const = 0;

    virtual const std::size_t& GetMinArgumentCount() const = 0;
    virtual const std::size_t& GetMaxArgumentCount() const = 0;

    virtual ~IFunctionToken() override = default;

    protected:
    IFunctionToken() = default;

    private:
    IFunctionToken(const IFunctionToken&)            = delete;
    IFunctionToken& operator=(const IFunctionToken&) = delete;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__IFUNCTIONTOKEN_HPP__
