#ifndef __TEXT_EXPRESSION_IFUNCTIONTOKEN_HPP__
#define __TEXT_EXPRESSION_IFUNCTIONTOKEN_HPP__

#include <vector>
#include <functional>
#include "IToken.hpp"
#include "IValueToken.hpp"
#include "common/IIdentifiable.hpp"

namespace Text::Expression
{
  class IFunctionToken : public virtual IToken, public IIdentifiable<std::string>
  {
    public:
    virtual IValueToken* operator()(const std::vector<IValueToken*>&) const = 0;

    virtual const std::size_t& GetMinArgumentCount() const = 0;
    virtual const std::size_t& GetMaxArgumentCount() const = 0;

    virtual ~IFunctionToken() override = default;

    protected:
    IFunctionToken() = default;

    private:
    IFunctionToken(const IFunctionToken&) = delete;
    IFunctionToken& operator=(const IFunctionToken&) = delete;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION_IFUNCTIONTOKEN_HPP__
