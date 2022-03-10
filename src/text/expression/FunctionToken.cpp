#include "FunctionToken.hpp"
#include <cstddef>
#include <limits>

namespace Text::Expression
{
  const std::size_t& FunctionToken::GetArgumentCountMaxLimit() { return s_ArgumentsMaxLimit; }
  void FunctionToken::SetArgumentsMaxLimit(std::size_t value) { s_ArgumentsMaxLimit = value; }

  IValueToken* FunctionToken::operator()(const std::vector<IValueToken*>& args) const { return m_Callback(args); }
  const std::string& FunctionToken::GetIdentifier() const { return m_Identifier; }
  const std::size_t& FunctionToken::GetMinArgumentCount() const { return m_MinArgumentCount; }
  const std::size_t& FunctionToken::GetMaxArgumentCount() const { return m_MaxArgumentCount; }

  std::string FunctionToken::ToString() const { return this->GetIdentifier(); }

  FunctionToken::FunctionToken(const FunctionToken::CallbackType& callback, const std::string& identifier, std::size_t minArguments, std::size_t maxArguments)
      : IFunctionToken()
      , m_Callback(callback)
      , m_Identifier(identifier)
      , m_MinArgumentCount(minArguments)
      , m_MaxArgumentCount(maxArguments)
      , m_ArgumentCount(0u)
      , m_BracketBalance(0)
  {
    if(m_MinArgumentCount > m_MaxArgumentCount)
    {
      throw std::invalid_argument("Invalid function argument count");
    }
  }

  FunctionToken::FunctionToken()
      : IFunctionToken()
      , m_Callback()
      , m_Identifier()
      , m_MinArgumentCount(0u)
      , m_MaxArgumentCount(FunctionToken::s_ArgumentsMaxLimit)
      , m_ArgumentCount(0u)
      , m_BracketBalance(0)
  {}

  FunctionToken::FunctionToken(const FunctionToken& other)
      : IToken()
      , IFunctionToken()
      , m_Callback(other.m_Callback)
      , m_Identifier(other.m_Identifier)
      , m_MinArgumentCount(other.m_MinArgumentCount)
      , m_MaxArgumentCount(other.m_MaxArgumentCount)
      , m_ArgumentCount(other.m_ArgumentCount)
      , m_BracketBalance(other.m_BracketBalance)
  {}

  FunctionToken::FunctionToken(FunctionToken&& other)
      : IFunctionToken()
      , m_Callback(std::move(other.m_Callback))
      , m_Identifier(std::move(other.m_Identifier))
      , m_MinArgumentCount(std::move(other.m_MinArgumentCount))
      , m_MaxArgumentCount(std::move(other.m_MaxArgumentCount))
      , m_ArgumentCount(std::move(other.m_ArgumentCount))
      , m_BracketBalance(std::move(other.m_BracketBalance))
  {}

  FunctionToken& FunctionToken::operator=(const FunctionToken& other)
  {
    m_Callback         = other.m_Callback;
    m_Identifier       = other.m_Identifier;
    m_MinArgumentCount = other.m_MinArgumentCount;
    m_MaxArgumentCount = other.m_MaxArgumentCount;
    m_ArgumentCount    = other.m_ArgumentCount;
    m_BracketBalance   = other.m_BracketBalance;

    return *this;
  }

  FunctionToken& FunctionToken::operator=(FunctionToken&& other)
  {
    m_Callback         = std::move(other.m_Callback);
    m_Identifier       = std::move(other.m_Identifier);
    m_MinArgumentCount = std::move(other.m_MinArgumentCount);
    m_MaxArgumentCount = std::move(other.m_MaxArgumentCount);
    m_ArgumentCount    = std::move(other.m_ArgumentCount);
    m_BracketBalance   = std::move(other.m_BracketBalance);

    return *this;
  }

  std::size_t FunctionToken::s_ArgumentsMaxLimit = std::numeric_limits<std::size_t>::max();
} // namespace Text::Expression
