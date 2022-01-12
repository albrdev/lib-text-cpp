#ifndef __SYMBOLICTOKEN_HPP__
#define __SYMBOLICTOKEN_HPP__

#include "IToken.hpp"
#include <string>
#include <sstream>

template<class T>
class SymbolicToken : public virtual IToken
{
  public:
  const T& GetIdentifier() const { return m_Identifier; }

  virtual std::string ToString() const override
  {
    std::ostringstream oss;
    oss << GetIdentifier();
    return oss.str();
  }

  SymbolicToken<T>& operator=(const T& value)
  {
    this->m_Identifier = value;
    return *this;
  }

  SymbolicToken(const T& identifier)
      : IToken()
      , m_Identifier(identifier)
  {}

  SymbolicToken()
      : IToken()
      , m_Identifier()
  {}

  SymbolicToken(const SymbolicToken& other)
      : IToken()
      , m_Identifier(other.m_Identifier)
  {}

  SymbolicToken(SymbolicToken&& other)
      : IToken()
      , m_Identifier(std::move(other.m_Identifier))
  {}

  virtual ~SymbolicToken() override = default;

  SymbolicToken<T>& operator=(const SymbolicToken<T>& other)
  {
    this->m_Identifier = other.m_Identifier;
    return *this;
  }

  SymbolicToken<T>& operator=(SymbolicToken<T>&& other)
  {
    this->m_Identifier = std::move(other.m_Identifier);
    return *this;
  }

  private:
  T m_Identifier;
};

#endif // __SYMBOLICTOKEN_HPP__
