#ifndef __FUNCTIONTOKEN_HPP__
#define __FUNCTIONTOKEN_HPP__

#include "TokenBase.hpp"
#include "IFunctionToken.hpp"
#include "IValueToken.hpp"
#include <string>
#include <functional>
#include <limits>
#include <exception>

class FunctionToken : public IFunctionToken, public TokenBase<std::function<IValueToken*(const std::vector<IValueToken*>&)>>
{
  public:
  static const std::size_t& GetArgumentCountMaxLimit() { return s_ArgumentsMaxLimit; }
  static void SetArgumentsMaxLimit(std::size_t value) { s_ArgumentsMaxLimit = value; }

  virtual IValueToken* operator()(const std::vector<IValueToken*>& args) const override { return this->GetObject()(args); }
  virtual const std::string& GetIdentifier() const override { return m_Identifier; }
  virtual const std::size_t& GetMinArgumentCount() const override { return m_MinArgumentCount; }
  virtual const std::size_t& GetMaxArgumentCount() const override { return m_MaxArgumentCount; }

  virtual std::string ToString() const override { return this->GetIdentifier(); }

  FunctionToken(const CallbackType& callback,
                const std::string& identifier,
                std::size_t minArguments = 0u,
                std::size_t maxArguments = FunctionToken::s_ArgumentsMaxLimit)
      : IFunctionToken()
      , TokenBase<CallbackType>(callback)
      , m_Identifier(identifier)
      , m_MinArgumentCount(minArguments)
      , m_MaxArgumentCount(maxArguments)
  {
    if(m_MinArgumentCount > m_MaxArgumentCount)
    {
      throw std::invalid_argument("Invalid function argument count");
    }
  }

  virtual ~FunctionToken() override = default;

  FunctionToken()
      : IFunctionToken()
      , TokenBase<CallbackType>()
      , m_Identifier()
      , m_MinArgumentCount(0u)
      , m_MaxArgumentCount(FunctionToken::s_ArgumentsMaxLimit)
  {}

  FunctionToken(const FunctionToken& other)
      : IFunctionToken()
      , TokenBase<CallbackType>(other)
      , m_Identifier(other.m_Identifier)
      , m_MinArgumentCount(other.m_MinArgumentCount)
      , m_MaxArgumentCount(other.m_MaxArgumentCount)
  {}

  FunctionToken(FunctionToken&& other)
      : IFunctionToken()
      , TokenBase<CallbackType>(std::move(other))
      , m_Identifier(std::move(other.m_Identifier))
      , m_MinArgumentCount(std::move(other.m_MinArgumentCount))
      , m_MaxArgumentCount(std::move(other.m_MaxArgumentCount))
  {}

  FunctionToken& operator=(const FunctionToken& other)
  {
    TokenBase<CallbackType>::operator=(other);
    m_Identifier                     = other.m_Identifier;
    m_MinArgumentCount               = other.m_MinArgumentCount;
    m_MaxArgumentCount               = other.m_MaxArgumentCount;

    return *this;
  }

  FunctionToken& operator=(FunctionToken&& other)
  {
    TokenBase<CallbackType>::operator=(std::move(other));
    m_Identifier                     = std::move(other.m_Identifier);
    m_MinArgumentCount               = std::move(other.m_MinArgumentCount);
    m_MaxArgumentCount               = std::move(other.m_MaxArgumentCount);

    return *this;
  }

  private:
  using TokenBase<CallbackType>::GetObject;
  using TokenBase<CallbackType>::SetObject;

  static std::size_t s_ArgumentsMaxLimit;

  std::string m_Identifier;
  std::size_t m_MinArgumentCount;
  std::size_t m_MaxArgumentCount;
};

std::size_t FunctionToken::s_ArgumentsMaxLimit = std::numeric_limits<std::size_t>::max();

#endif // __FUNCTIONTOKEN_HPP__
