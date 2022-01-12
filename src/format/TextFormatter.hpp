#ifndef __TEXTFORMATTER_HPP__
#define __TEXTFORMATTER_HPP__

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include "../Parser.hpp"

class TextFormatter : protected Parser
{
  public:
  static const char DefaultQualifier = '$';

  std::string Format(const std::string& text);

  char GetQualifier() const;
  void SetQualifier(char value);

  std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& GetMacros();

  void SetMacros(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& value);

  void SetMacros(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>&& value);

  void SetOnMissingIdentifier(const std::function<std::string(const std::string&, const std::vector<std::string>&)>& value);

  TextFormatter(char qualifier, const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& macros);

  TextFormatter(char qualifier, const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>&& macros);

  TextFormatter(char qualifier);

  TextFormatter(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& macros);

  TextFormatter(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>&& macros);

  TextFormatter();

  ~TextFormatter() override = default;

  private:
  char m_Qualifier;
  std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>> m_Macros;
  std::function<std::string(const std::string&, const std::vector<std::string>&)> m_OnMissingIdentifier;

  std::string ParseValue();
  std::string ParseExpression();
};

#endif // __TEXTFORMATTER_HPP__
