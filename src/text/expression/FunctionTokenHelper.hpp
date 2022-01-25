#ifndef __FUNCTIONTOKENHELPER_HPP__
#define __FUNCTIONTOKENHELPER_HPP__

#include "IFunctionToken.hpp"
#include <string>

class FunctionTokenHelper : public virtual IToken
{
  public:
  virtual std::string ToString() const override { return m_rInstance.ToString(); }

  const IFunctionToken& GetFunction() const { return m_rInstance; }
  const int& GetBracketBalance() const { return m_BracketBalance; }
  void IncrementBracketBalance() { m_BracketBalance++; }
  void DecrementBracketBalance() { m_BracketBalance--; }
  const std::size_t& GetArgumentCount() const { return m_ArgumentCount; }
  void IncrementArgumentCount() { m_ArgumentCount++; }
  void DecrementArgumentCount() { m_ArgumentCount--; }

  FunctionTokenHelper(const IFunctionToken& instance)
      : IToken()
      , m_rInstance(instance)
      , m_ArgumentCount(0u)
      , m_BracketBalance(0)
  {}

  virtual ~FunctionTokenHelper() override = default;

  FunctionTokenHelper(const FunctionTokenHelper& other)
      : IToken()
      , m_rInstance(other.m_rInstance)
      , m_ArgumentCount(other.m_ArgumentCount)
      , m_BracketBalance(other.m_BracketBalance)
  {}

  FunctionTokenHelper(FunctionTokenHelper&& other)
      : IToken()
      , m_rInstance(std::move(other.m_rInstance))
      , m_ArgumentCount(std::move(other.m_ArgumentCount))
      , m_BracketBalance(std::move(other.m_BracketBalance))
  {}

  FunctionTokenHelper& operator=(const FunctionTokenHelper& other)
  {
    //m_rInstance      = other.m_rInstance;
    m_ArgumentCount  = other.m_ArgumentCount;
    m_BracketBalance = other.m_BracketBalance;

    return *this;
  }

  FunctionTokenHelper& operator=(FunctionTokenHelper&& other)
  {
    //m_rInstance      = std::move(other.m_rInstance);
    m_ArgumentCount  = std::move(other.m_ArgumentCount);
    m_BracketBalance = std::move(other.m_BracketBalance);

    return *this;
  }

  private:
  const IFunctionToken& m_rInstance;
  std::size_t m_ArgumentCount;
  int m_BracketBalance;
};

#endif // __FUNCTIONTOKENHELPER_HPP__
