#ifndef __TEXT_EXPRESSION__VARIABLETOKEN_HPP__
#define __TEXT_EXPRESSION__VARIABLETOKEN_HPP__

#include "IVariableToken.hpp"
#include "ValueToken.hpp"

namespace Text::Expression
{
  template<class... Ts>
  class VariableToken : public IVariableToken, public ValueToken<Ts...>
  {
    public:
    virtual const std::string& GetIdentifier() const override { return m_Identifier; }

    template<class T>
    VariableToken<Ts...>& operator=(const T& value)
    {
      ValueToken<Ts...>::operator=(value);
      return *this;
    }

    VariableToken()
        : IVariableToken()
        , ValueToken<Ts...>()
        , m_Identifier()
    {}

    VariableToken(const std::string& identifier)
        : IVariableToken()
        , ValueToken<Ts...>()
        , m_Identifier(identifier)
    {}

    template<class T>
    explicit VariableToken(const std::string& identifier, const T& value)
        : IVariableToken()
        , ValueToken<Ts...>(value)
        , m_Identifier(identifier)
    {}

    virtual ~VariableToken() override = default;

    VariableToken(const VariableToken<Ts...>& other)
        : IVariableToken()
        , ValueToken<Ts...>(other)
        , m_Identifier(other.m_Identifier)
    {}

    VariableToken(VariableToken<Ts...>&& other)
        : IVariableToken()
        , ValueToken<Ts...>(std::move(other))
        , m_Identifier(std::move(other.m_Identifier))
    {}

    VariableToken<Ts...>& operator=(const VariableToken<Ts...>& other)
    {
      ValueToken<Ts...>::operator=(other);
      m_Identifier               = other.m_Identifier;
      return *this;
    }

    VariableToken<Ts...>& operator=(VariableToken<Ts...>&& other)
    {
      ValueToken<Ts...>::operator=(std::move(other));
      m_Identifier               = std::move(other.m_Identifier);
      return *this;
    }

    protected:
    virtual bool Equals(const Common::IEquals& other) const override
    {
      auto tmpObject = dynamic_cast<decltype(this)>(&other);
      return ValueToken<Ts...>::Equals(other) && (m_Identifier == tmpObject->m_Identifier);
    }

    virtual void ThrowOnUninitializedAccess() const override { throw std::runtime_error("Accessing uninitialized variable: " + GetIdentifier()); }

    private:
    std::string m_Identifier;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__VARIABLETOKEN_HPP__
