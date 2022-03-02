#ifndef __TEXT_EXPRESSION_IOPERATORTOKEN_HPP__
#define __TEXT_EXPRESSION_IOPERATORTOKEN_HPP__

#include "IToken.hpp"

namespace text::expression
{
  enum class Associativity : std::uint32_t
  {
    Left  = 1u << 0,
    Right = 1u << 1,
    Any   = Left | Right
  };

  inline std::uint32_t operator|(Associativity lhs, Associativity rhs) { return static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs); }
  inline std::uint32_t operator&(Associativity lhs, Associativity rhs) { return static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs); }

  class IOperatorToken : public virtual IToken
  {
    public:
    virtual const int& GetPrecedence() const              = 0;
    virtual const Associativity& GetAssociativity() const = 0;

    virtual ~IOperatorToken() override = default;

    protected:
    IOperatorToken() = default;

    private:
    IOperatorToken(const IOperatorToken&) = delete;
    IOperatorToken& operator=(const IOperatorToken&) = delete;
  };
} // namespace text::expression

#endif // __TEXT_EXPRESSION_IOPERATORTOKEN_HPP__
