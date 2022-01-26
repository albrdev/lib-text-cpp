cmake_minimum_required(VERSION 3.14)

target_sources(${LIBRARY_TEXT}
  PUBLIC
    IToken.hpp
    IValueToken.hpp
    IVariableToken.hpp
    IOperatorToken.hpp
    IUnaryOperatorToken.hpp
    IBinaryOperatorToken.hpp
    IFunctionToken.hpp

    TokenBase.hpp
    GenericToken.hpp
    ValueToken.hpp
    VariableToken.hpp
    UnaryOperatorToken.hpp
    BinaryOperatorToken.hpp
    FunctionToken.hpp
    FunctionTokenHelper.hpp
    SyntaxException.hpp

    Token.hpp
    ExpressionTokenizer.hpp
    ExpressionPostfixParser.hpp
    ExpressionEvaluator.hpp
    ExpressionParser.hpp

  PRIVATE
    UnaryOperatorToken.cpp
    BinaryOperatorToken.cpp
    FunctionToken.cpp
    FunctionTokenHelper.cpp
    SyntaxException.cpp

    ExpressionTokenizer.cpp
    ExpressionPostfixParser.cpp
    ExpressionEvaluator.cpp
    ExpressionParser.cpp
)

target_sources(${UNITTEST_TEXT}
  PRIVATE
    ExpressionParser.test.cpp
)
