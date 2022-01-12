#ifndef __ITOKEN_HPP__
#define __ITOKEN_HPP__

#include "IOutput.hpp"

class IToken : public IOutput
{
  public:
  virtual ~IToken() override = default;

  protected:
  IToken() = default;

  private:
  IToken(const IToken&) = delete;
  IToken& operator=(const IToken&) = delete;
};

#endif // __ITOKEN_HPP__
