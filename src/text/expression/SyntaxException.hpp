#ifndef __SYNTAXEXCEPTION_HPP__
#define __SYNTAXEXCEPTION_HPP__

#include <string>
#include <exception>

class SyntaxException : public std::runtime_error
{
  public:
  const std::size_t& GetIndex() const { return m_Index; }

  SyntaxException()
      : std::runtime_error(k_DefaultMessage)
      , m_Index(-1)
  {}

  SyntaxException(const std::string& message)
      : std::runtime_error(message)
      , m_Index(-1)
  {}

  SyntaxException(std::size_t index)
      : std::runtime_error(CreateMessage(k_DefaultMessage, index))
      , m_Index(index)
  {}

  SyntaxException(const std::string& message, std::size_t index)
      : std::runtime_error(CreateMessage(message, index))
      , m_Index(index)
  {}

  private:
  static std::string CreateMessage(const std::string& message, std::size_t index) { return message + "(Index: " + std::to_string(index) + ")"; }

  constexpr static const char* k_DefaultMessage = "Syntax error";

  const std::size_t m_Index;
};

#endif // __SYNTAXEXCEPTION_HPP__
