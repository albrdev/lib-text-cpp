#ifndef __SYNTAXEXCEPTION_HPP__
#define __SYNTAXEXCEPTION_HPP__

#include <string>
#include <stdexcept>

class SyntaxException : public std::runtime_error
{
  public:
  const std::size_t& GetIndex() const;

  SyntaxException(const std::string& message);
  SyntaxException(std::size_t index);
  SyntaxException(const std::string& message, std::size_t index);
  SyntaxException();

  private:
  static std::string CreateMessage(const std::string& message, std::size_t index);

  constexpr static const char* k_DefaultMessage = "Syntax error";

  const std::size_t m_Index;
};

#endif // __SYNTAXEXCEPTION_HPP__
