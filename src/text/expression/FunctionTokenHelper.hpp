#ifndef __TEXT_EXPRESSION__FUNCTIONTOKENHELPER_HPP__
#define __TEXT_EXPRESSION__FUNCTIONTOKENHELPER_HPP__

#include "IFunctionToken.hpp"

namespace Text::Expression
{
  class ExpressionPostfixParser;
  class ExpressionEvaluator;

  class FunctionTokenHelper : public IFunctionToken
  {
    friend class ExpressionPostfixParser;
    friend class ExpressionEvaluator;

    public:
    virtual IValueToken* operator()(const std::vector<IValueToken*>& args) const override;
    virtual const std::string& GetIdentifier() const override;
    virtual const std::size_t& GetMinArgumentCount() const override;
    virtual const std::size_t& GetMaxArgumentCount() const override;

    virtual std::string ToString() const override;

    FunctionTokenHelper(const IFunctionToken& rFunctionTokenInstance);
    virtual ~FunctionTokenHelper() override = default;
    FunctionTokenHelper(const FunctionTokenHelper& other);
    FunctionTokenHelper(FunctionTokenHelper&& other);

    protected:
    virtual bool Equals(const Common::IEquals& other) const override;

    private:
    const IFunctionToken& m_rFunctionTokenInstance;

    std::size_t m_ArgumentCount;
    int m_BracketBalance;
  };
} // namespace Text::Expression

#endif // __TEXT_EXPRESSION__FUNCTIONTOKENHELPER_HPP__
