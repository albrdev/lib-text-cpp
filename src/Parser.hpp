#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>
#include "ParserBase.hpp"

class Parser : public ParserBase
{
  public:
  char GetDecimalPointCharacter();
  void SetDecimalPointCharacter(char value);

  Parser(const std::string& text, char decimalPointCharacter);
  Parser(char decimalPointCharacter);
  Parser(const std::string& text);
  Parser();
  Parser(const Parser& other);
  Parser(Parser&& other);
  virtual ~Parser() override = default;

  protected:
  static bool IsWhitespace(char character);
  static bool IsNumber(char character);
  static bool IsString(char character);
  static bool IsIdentifier(char character);

  Parser& ParseNumber(std::string& result);
  Parser& ParseString(std::string& result);
  Parser& ParseIdentifier(std::string& result);

  std::string ParseNumber();
  std::string ParseString();
  std::string ParseIdentifier();

  private:
  char m_DecimalPointCharacter;

  static char GetLocaleDecimalPointCharacter();
};

#endif // __PARSER_HPP__
