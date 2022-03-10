#ifndef __TEXT_EXPRESSION__VALUETOKEN_HPP__
#define __TEXT_EXPRESSION__VALUETOKEN_HPP__

#include <string>
#include <variant>
#include <sstream>
#include <typeinfo>
#include "IValueToken.hpp"

namespace Text::Expression
{
  template<class... Ts>
  class ValueToken : public virtual IValueToken
  {
    public:
    using ValueType = std::variant<Ts...>;

    template<class T>
    const auto& GetValue() const
    {
      if(!m_IsInitialized)
      {
        ThrowOnUninitializedAccess();
      }

      return std::get<T>(m_Value);
    }

    template<class T>
    auto& GetValue()
    {
      if(!m_IsInitialized)
      {
        ThrowOnUninitializedAccess();
      }

      return std::get<T>(m_Value);
    }

    template<class T>
    void SetValue(const T& value)
    {
      m_Value         = value;
      m_IsInitialized = true;
    }

    const std::type_info& GetType() const override { return std::visit(TypeIdVisitor(), m_Value); }

    const bool& IsInitialized() const { return m_IsInitialized; }

    virtual std::string ToString() const override
    {
      if(!m_IsInitialized)
      {
        ThrowOnUninitializedAccess();
      }

      return std::visit(ToStringVisitor(), m_Value);
    }

    template<class T>
    ValueToken<Ts...>& operator=(const T& value)
    {
      m_Value         = value;
      m_IsInitialized = true;
      return *this;
    }

    template<class T>
    explicit ValueToken(const T& value)
        : IValueToken()
        , m_Value(value)
        , m_IsInitialized(true)
    {}

    virtual ~ValueToken() override = default;

    ValueToken()
        : IValueToken()
        , m_Value()
        , m_IsInitialized(false)
    {}

    ValueToken(const ValueToken<Ts...>& other)
        : IToken()
        , IValueToken()
        , m_Value(other.m_Value)
        , m_IsInitialized(other.m_IsInitialized)
    {}

    ValueToken(ValueToken<Ts...>&& other)
        : IValueToken()
        , m_Value(std::move(other.m_Value))
        , m_IsInitialized(std::move(other.m_IsInitialized))
    {}

    ValueToken<Ts...>& operator=(const ValueToken<Ts...>& other)
    {
      m_Value         = (other.m_Value);
      m_IsInitialized = other.m_IsInitialized;
      return *this;
    }

    ValueToken<Ts...>& operator=(ValueToken<Ts...>&& other)
    {
      m_Value         = std::move(other.m_Value);
      m_IsInitialized = std::move(other.m_IsInitialized);
      return *this;
    }

    protected:
    virtual void ThrowOnUninitializedAccess() const { throw std::runtime_error("Accessing uninitialized value"); }

    private:
    struct TypeIdVisitor
    {
      const std::type_info& operator()(const auto& value) const { return typeid(value); }
    };

    struct ToStringVisitor
    {
      std::string operator()(const std::nullptr_t& value) const
      {
        static_cast<void>(value);
        return "null";
      }

      std::string operator()(const std::string& value) const { return value; }

      std::string operator()(const auto& value) const
      {
        std::ostringstream oss;
        oss << value;
        return oss.str();
      }
    };

    ValueType m_Value;
    bool m_IsInitialized;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__VALUETOKEN_HPP__
