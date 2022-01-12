#ifndef __OPERATORTOKEN_HPP__
#define __OPERATORTOKEN_HPP__

#include "IToken.hpp"

enum class Associativity : std::uint32_t
{
  Left  = 1u << 0,
  Right = 1u << 1,
  Any   = Left | Right
};

inline std::uint32_t operator|(Associativity lhs, Associativity rhs) { return static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs); }
inline std::uint32_t operator&(Associativity lhs, Associativity rhs) { return static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs); }

class OperatorToken : public virtual IToken
{
  public:
  const int& GetPrecedence() const;
  const Associativity& GetAssociativity() const;

  OperatorToken(int precedence, Associativity associativity);
  OperatorToken();
  virtual ~OperatorToken() = default;
  OperatorToken(const OperatorToken& other);
  OperatorToken(OperatorToken&& other);

  OperatorToken& operator=(const OperatorToken& value);
  OperatorToken& operator=(OperatorToken&& value);

  private:
  int m_Precedence;
  Associativity m_Associativity;
};

#endif // __OPERATORTOKEN_HPP__
