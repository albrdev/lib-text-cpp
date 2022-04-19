#ifndef __TEXT_EXCEPTION_SYNTAXERROR_HPP__
#define __TEXT_EXCEPTION_SYNTAXERROR_HPP__

#include <stdexcept>
#include <string>

namespace Text::Exception
{
  class SyntaxError : public std::runtime_error
  {
    public:
    const std::size_t& GetIndex() const;

    SyntaxError(const std::string& message);
    SyntaxError(const char* message);
    SyntaxError(std::size_t index);
    SyntaxError(const std::string& message, std::size_t index);
    SyntaxError(const char* message, std::size_t index);
    SyntaxError();
    SyntaxError(const SyntaxError& other) noexcept;

    private:
    static std::string CreateMessage(const std::string& message, std::size_t index);

    constexpr static const char* k_DefaultMessage = "Syntax error";

    const std::size_t m_Index;
  };
} // namespace Text::Exception

#endif // __TEXT_EXCEPTION_SYNTAXERROR_HPP__
