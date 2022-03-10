#ifndef __TEXT_EXPRESSION__GENERICTOKEN_HPP__
#define __TEXT_EXPRESSION__GENERICTOKEN_HPP__

#include <string>
#include <sstream>

namespace Text::Expression
{
  template<class T>
  class GenericToken : public virtual IToken
  {
    public:
    virtual std::string ToString() const override
    {
      std::ostringstream oss;
      oss << m_Object;
      return oss.str();
    }

    operator const T&() const { return m_Object; }
    operator T&() { return m_Object; }

    GenericToken<T>& operator=(const T& value)
    {
      m_Object = value;
      return *this;
    }

    GenericToken(const T& value)
        : m_Object(value)
    {}

    virtual ~GenericToken() override = default;

    GenericToken()
        : m_Object()
    {}

    GenericToken(const GenericToken<T>& other)
        : m_Object(other.m_Object)
    {}

    GenericToken(GenericToken<T>&& other)
        : m_Object(std::move(other.m_Object))
    {}

    GenericToken<T>& operator=(const GenericToken<T>& other)
    {
      m_Object = other.m_Object;
      return *this;
    }

    GenericToken<T>& operator=(GenericToken<T>&& other)
    {
      m_Object = std::move(other.m_Object);
      return *this;
    }

    private:
    T m_Object;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__GENERICTOKEN_HPP__
