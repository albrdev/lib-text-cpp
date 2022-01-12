cmake_minimum_required(VERSION 3.14)

target_sources(${LIBRARY_TEXT}
  PUBLIC
    IToken.hpp
    Token.hpp
    TokenBase.hpp
    SymbolicToken.hpp
    GenericToken.hpp
    ValueToken.hpp
    VariableToken.hpp
    OperatorToken.hpp
    UnaryOperatorToken.hpp
    BinaryOperatorToken.hpp
    FunctionToken.hpp
    FunctionTokenHelper.hpp
    Token.hpp
    ExpressionTokenizer.hpp
    ExpressionPostfixParser.hpp
    ExpressionEvaluator.hpp
    ExpressionParser.hpp
    SyntaxException.hpp

  PRIVATE
    OperatorToken.cpp
)

target_sources(${UNITTEST_TEXT}
  PRIVATE
    ExpressionParser.test.cpp
)
