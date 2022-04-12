#ifndef __TEXT_EXPRESSION__GENERICTOKEN_HPP__
#define __TEXT_EXPRESSION__GENERICTOKEN_HPP__

#include <sstream>
#include <string>

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

    protected:
    virtual bool Equals(const Common::IEquals& other) const override
    {
      auto tmpObject = dynamic_cast<decltype(this)>(&other);
      return (tmpObject != nullptr) && (m_Object == tmpObject->m_Object);
    }

    private:
    T m_Object;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__GENERICTOKEN_HPP__
