#ifndef __TEXT_FORMATTING__TEXTFORMATTER_HPP__
#define __TEXT_FORMATTING__TEXTFORMATTER_HPP__

#include "text/parsing/Parser.hpp"

#include <unordered_map>
#include <vector>

namespace Text::Formatting
{
  class TextFormatter : protected Parsing::Parser
  {
    public:
    static const char DefaultQualifier = '$';

    std::string Format(const std::string& text);

    char GetQualifier() const;
    void SetQualifier(char value);

    std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& GetMacros();
    void SetMacros(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& value);

    void SetOnMissingIdentifier(const std::function<std::string(const std::string&, const std::vector<std::string>&)>& value);

    TextFormatter(char qualifier, const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& macros);
    TextFormatter(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& macros);
    TextFormatter(char qualifier);
    ~TextFormatter() override = default;
    TextFormatter();
    TextFormatter(const TextFormatter& other);
    TextFormatter(TextFormatter&& other);

    private:
    char m_Qualifier;
    std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>> m_Macros;
    std::function<std::string(const std::string&, const std::vector<std::string>&)> m_OnMissingIdentifier;

    std::string ParseValue();
    std::string ParseExpression();
  };
} // namespace Text::Formatting

#endif // __TEXT_FORMATTING__TEXTFORMATTER_HPP__
