#include "Parser.hpp"
#include <clocale>
#include "text/exception/SyntaxException.hpp"

namespace Text::Parsing
{
  Parser& Parser::ParseNumber(std::string& result)
  {
    (void)Get(result, [this](char c) { return Parser::IsNumber(c) || c == GetDecimalPointCharacter(); });
    return *this;
  }

  Parser& Parser::ParseIntermediate(std::string& result)
  {
    char delimiter = GetCurrent();
    (void)Next();

    char previous = '\0';
    (void)Get(result, [this, delimiter, &previous](char chr) {
      const bool state = chr != delimiter || previous == '\\';
      previous         = chr;
      return state;
    });

    if(GetCurrent() != delimiter)
    {
      throw Exception::SyntaxException("Unterminated sequence: " + result, GetIndex() - result.length());
    }

    (void)Next();
    return *this;
  }

  Parser& Parser::ParseIdentifier(std::string& result)
  {
    (void)Get(result, [](char c) { return Parser::IsIdentifier(c) || std::isdigit(c) != 0 || c == '.'; });
    return *this;
  }

  std::string Parser::ParseNumber()
  {
    std::string result;
    ParseNumber(result);
    return result;
  }

  std::string Parser::ParseIntermediate()
  {
    std::string result;
    ParseIntermediate(result);
    return result;
  }

  std::string Parser::ParseIdentifier()
  {
    std::string result;
    ParseIdentifier(result);
    return result;
  }

  char Parser::GetDecimalPointCharacter() { return m_DecimalPointCharacter; }
  void Parser::SetDecimalPointCharacter(char value) { m_DecimalPointCharacter = value; }

  bool Parser::IsWhitespace(char character) { return std::isspace(character) != 0; }
  bool Parser::IsNumber(char character) { return std::isdigit(character) != 0; }
  bool Parser::IsString(char character) { return character == '\'' || character == '\"'; }
  bool Parser::IsIdentifier(char character) { return std::isalpha(character) != 0 || character == '_'; }

  char Parser::GetLocaleDecimalPointCharacter() { return std::use_facet<std::numpunct<char>>(std::locale()).decimal_point(); }

  Parser::Parser(const std::string& text, char decimalPointCharacter)
      : ParserBase(text)
      , m_DecimalPointCharacter(decimalPointCharacter)
  {}

  Parser::Parser(char decimalPointCharacter)
      : ParserBase()
      , m_DecimalPointCharacter(decimalPointCharacter)
  {}

  Parser::Parser(const std::string& text)
      : ParserBase(text)
      , m_DecimalPointCharacter(GetLocaleDecimalPointCharacter())
  {}

  Parser::Parser()
      : ParserBase()
      , m_DecimalPointCharacter(GetLocaleDecimalPointCharacter())
  {}

  Parser::Parser(const Parser& other)
      : ParserBase(other)
      , m_DecimalPointCharacter(other.m_DecimalPointCharacter)
  {}

  Parser::Parser(Parser&& other)
      : ParserBase(std::move(other))
      , m_DecimalPointCharacter(std::move(other.m_DecimalPointCharacter))
  {}
} // namespace Text::Parsing
