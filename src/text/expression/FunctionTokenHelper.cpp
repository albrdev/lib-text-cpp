#include "FunctionTokenHelper.hpp"

namespace Text::Expression
{
  IValueToken* FunctionTokenHelper::operator()(const std::vector<IValueToken*>& args) const { return m_rFunctionTokenInstance(args); }
  const std::string& FunctionTokenHelper::GetIdentifier() const { return m_rFunctionTokenInstance.GetIdentifier(); }
  const std::size_t& FunctionTokenHelper::GetMinArgumentCount() const { return m_rFunctionTokenInstance.GetMinArgumentCount(); }
  const std::size_t& FunctionTokenHelper::GetMaxArgumentCount() const { return m_rFunctionTokenInstance.GetMaxArgumentCount(); }

  std::string FunctionTokenHelper::ToString() const { return m_rFunctionTokenInstance.ToString(); }

  bool FunctionTokenHelper::Equals(const Common::IEquals& other) const { return m_rFunctionTokenInstance.Equals(other); }

  FunctionTokenHelper::FunctionTokenHelper(const IFunctionToken& rFunctionTokenInstance)
      : IFunctionToken()
      , m_rFunctionTokenInstance(rFunctionTokenInstance)
      , m_ArgumentCount(0u)
      , m_BracketBalance(0)
  {}

  FunctionTokenHelper::FunctionTokenHelper(const FunctionTokenHelper& other)
      : IToken()
      , IFunctionToken()
      , m_rFunctionTokenInstance(other.m_rFunctionTokenInstance)
      , m_ArgumentCount(other.m_ArgumentCount)
      , m_BracketBalance(other.m_BracketBalance)
  {}

  FunctionTokenHelper::FunctionTokenHelper(FunctionTokenHelper&& other)
      : IFunctionToken()
      , m_rFunctionTokenInstance(std::move(other.m_rFunctionTokenInstance))
      , m_ArgumentCount(std::move(other.m_ArgumentCount))
      , m_BracketBalance(std::move(other.m_BracketBalance))
  {}
} // namespace Text::Expression
