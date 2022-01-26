#ifndef __FUNCTIONTOKENHELPER_HPP__
#define __FUNCTIONTOKENHELPER_HPP__

#include "IFunctionToken.hpp"
#include <string>

class FunctionTokenHelper : public virtual IToken
{
  public:
  const IFunctionToken& GetFunction() const;
  const int& GetBracketBalance() const;
  void IncrementBracketBalance();
  void DecrementBracketBalance();
  const std::size_t& GetArgumentCount() const;
  void IncrementArgumentCount();
  void DecrementArgumentCount();

  virtual std::string ToString() const override;

  FunctionTokenHelper(const IFunctionToken& instance);
  virtual ~FunctionTokenHelper() override = default;

  private:
  const IFunctionToken& m_rInstance;
  std::size_t m_ArgumentCount;
  int m_BracketBalance;
};

#endif // __FUNCTIONTOKENHELPER_HPP__
