#include "SyntaxException.hpp"

namespace Text::Exception
{
  const std::size_t& SyntaxException::GetIndex() const { return m_Index; }

  SyntaxException::SyntaxException(const std::string& message)
      : std::runtime_error(message)
      , m_Index(-1)
  {}

  SyntaxException::SyntaxException(std::size_t index)
      : std::runtime_error(CreateMessage(k_DefaultMessage, index))
      , m_Index(index)
  {}

  SyntaxException::SyntaxException(const std::string& message, std::size_t index)
      : std::runtime_error(CreateMessage(message, index))
      , m_Index(index)
  {}

  SyntaxException::SyntaxException()
      : std::runtime_error(k_DefaultMessage)
      , m_Index(-1)
  {}

  std::string SyntaxException::CreateMessage(const std::string& message, std::size_t index) { return message + " (Index: " + std::to_string(index) + ")"; }
} // namespace Text::Exception
