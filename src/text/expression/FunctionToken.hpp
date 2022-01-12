#ifndef __FUNCTIONTOKEN_HPP__
#define __FUNCTIONTOKEN_HPP__

#include "TokenBase.hpp"
#include "SymbolicToken.hpp"
#include <string>
#include <functional>
#include <limits>
#include <exception>

template<class T>
class FunctionToken : public TokenBase<std::function<T(std::vector<T>)>>, public SymbolicToken<std::string>
{
  public:
  using InnerValueType = T;
  using CallbackType   = typename TokenBase<std::function<T(std::vector<T>)>>::ObjectType;

  static const std::size_t& GetArgumentCountMaxLimit() { return s_ArgumentsMaxLimit; }
  static void SetArgumentsMaxLimit(std::size_t value) { s_ArgumentsMaxLimit = value; }

  const std::size_t& GetMinArgumentCount() const { return m_MinArgumentCount; }
  const std::size_t& GetMaxArgumentCount() const { return m_MaxArgumentCount; }

  virtual std::string ToString() const override { return this->GetIdentifier(); }

  T operator()(std::vector<T>& args) const { return this->GetObject()(args); }

  FunctionToken()
      : TokenBase<CallbackType>()
      , SymbolicToken<std::string>()
      , m_MinArgumentCount(0u)
      , m_MaxArgumentCount(FunctionToken::s_ArgumentsMaxLimit)
  {}

  FunctionToken(const CallbackType& callback,
                const std::string& identifier,
                std::size_t minArguments = 0u,
                std::size_t maxArguments = FunctionToken::s_ArgumentsMaxLimit)
      : TokenBase<CallbackType>(callback)
      , SymbolicToken<std::string>(identifier)
      , m_MinArgumentCount(minArguments)
      , m_MaxArgumentCount(maxArguments)
  {
    if(m_MinArgumentCount > m_MaxArgumentCount)
    {
      throw std::invalid_argument("Invalid function argument count");
    }
  }

  virtual ~FunctionToken() override = default;

  FunctionToken(const FunctionToken<T>& other)
      : TokenBase<CallbackType>(other)
      , SymbolicToken<std::string>(other)
      , m_MinArgumentCount(other.m_MinArgumentCount)
      , m_MaxArgumentCount(other.m_MaxArgumentCount)
  {}

  FunctionToken(FunctionToken<T>&& other)
      : TokenBase<CallbackType>(std::move(other))
      , SymbolicToken<std::string>(std::move(other))
      , m_MinArgumentCount(std::move(other.m_MinArgumentCount))
      , m_MaxArgumentCount(std::move(other.m_MaxArgumentCount))
  {}

  FunctionToken<T>& operator=(const FunctionToken<T>& other)
  {
    TokenBase<CallbackType>::operator   =(other);
    SymbolicToken<std::string>::operator=(other);
    m_MinArgumentCount                  = other.m_MinArgumentCount;
    m_MaxArgumentCount                  = other.m_MaxArgumentCount;

    return *this;
  }

  FunctionToken<T>& operator=(FunctionToken<T>&& other)
  {
    TokenBase<CallbackType>::operator   =(std::move(other));
    SymbolicToken<std::string>::operator=(std::move(other));
    m_MinArgumentCount                  = std::move(other.m_MinArgumentCount);
    m_MaxArgumentCount                  = std::move(other.m_MaxArgumentCount);

    return *this;
  }

  private:
  using TokenBase<CallbackType>::GetObject;
  using TokenBase<CallbackType>::SetObject;

  static std::size_t s_ArgumentsMaxLimit;

  std::size_t m_MinArgumentCount;
  std::size_t m_MaxArgumentCount;
};

template<class T>
std::size_t FunctionToken<T>::s_ArgumentsMaxLimit = std::numeric_limits<std::size_t>::max();

#endif // __FUNCTIONTOKEN_HPP__
