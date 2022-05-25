#ifndef __TEXT_EXPRESSION__FUNCTIONTOKEN_HPP__
#define __TEXT_EXPRESSION__FUNCTIONTOKEN_HPP__

#include "IFunctionToken.hpp"
#include "IValueToken.hpp"

#include <functional>
#include <string>

namespace Text::Expression
{
  class FunctionTokenHelper;

  class FunctionToken : public IFunctionToken
  {
    friend class FunctionTokenHelper;

    public:
    using CallbackType = std::function<IValueToken*(const std::vector<IValueToken*>&)>;

    static const std::size_t& GetArgumentCountMaxLimit();
    static void SetArgumentsMaxLimit(std::size_t value);

    virtual IValueToken* operator()(const std::vector<IValueToken*>& args) const override;
    virtual const std::string& GetIdentifier() const override;
    virtual const std::size_t& GetMinArgumentCount() const override;
    virtual const std::size_t& GetMaxArgumentCount() const override;

    virtual std::string ToString() const override;

    FunctionToken(const std::string& identifier,
                  const FunctionToken::CallbackType& callback,
                  std::size_t minArguments = 0u,
                  std::size_t maxArguments = FunctionToken::s_ArgumentsMaxLimit);
    virtual ~FunctionToken() override = default;
    FunctionToken();
    FunctionToken(const FunctionToken& other);
    FunctionToken(FunctionToken&& other);
    FunctionToken& operator=(const FunctionToken& other);
    FunctionToken& operator=(FunctionToken&& other);

    protected:
    virtual bool Equals(const Common::IEquals& other) const override;

    private:
    static std::size_t s_ArgumentsMaxLimit;

    CallbackType m_Callback;
    std::string m_Identifier;
    std::size_t m_MinArgumentCount;
    std::size_t m_MaxArgumentCount;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__FUNCTIONTOKEN_HPP__
