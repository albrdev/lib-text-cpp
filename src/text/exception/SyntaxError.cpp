#include "SyntaxError.hpp"

namespace Text::Exception
{
  const std::size_t& SyntaxError::GetIndex() const { return m_Index; }

  SyntaxError::SyntaxError(const std::string& message)
      : std::runtime_error(message)
      , m_Index(-1)
  {}

  SyntaxError::SyntaxError(const char* message)
      : std::runtime_error(message)
      , m_Index(-1)
  {}

  SyntaxError::SyntaxError(std::size_t index)
      : std::runtime_error(CreateMessage(k_DefaultMessage, index))
      , m_Index(index)
  {}

  SyntaxError::SyntaxError(const std::string& message, std::size_t index)
      : std::runtime_error(CreateMessage(message, index))
      , m_Index(index)
  {}

  SyntaxError::SyntaxError(const char* message, std::size_t index)
      : std::runtime_error(CreateMessage(message, index))
      , m_Index(index)
  {}

  SyntaxError::SyntaxError()
      : std::runtime_error(k_DefaultMessage)
      , m_Index(-1)
  {}

  SyntaxError::SyntaxError(const SyntaxError& other) noexcept
      : std::runtime_error(other)
      , m_Index(other.m_Index)
  {}

  std::string SyntaxError::CreateMessage(const std::string& message, std::size_t index) { return message + " (Index: " + std::to_string(index) + ")"; }
} // namespace Text::Exception
