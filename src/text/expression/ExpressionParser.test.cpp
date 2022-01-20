#include <gtest/gtest.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <ratio>
#include <limits>
#include <exception>
#include <mpreal.h>
#include "ExpressionParser.hpp"

using ArithmeticType     = double;
using ArithmeticValue    = ExpressionParser<std::uint64_t, ArithmeticType>::ValueType;
using ArithmeticVariable = ExpressionParser<std::uint64_t, ArithmeticType>::VariableType;

using BitwiseType     = std::uint64_t;
using BitwiseValue    = ExpressionParser<BitwiseType>::ValueType;
using BitwiseVariable = ExpressionParser<BitwiseType>::VariableType;

static ArithmeticValue* arithmeticNumberConverter(const std::string& value)
{
  std::istringstream iss(value);
  ArithmeticType result;
  iss >> result;
  return new ArithmeticValue(result);
}

static BitwiseValue* bitwiseNumberConverter(const std::string& value)
{
  std::istringstream iss(value);
  BitwiseType result;
  iss >> result;
  return new BitwiseValue(result);
}

std::vector<ArithmeticValue> arithmeticResults;
ArithmeticValue* arithmeticAns(const std::vector<ArithmeticValue*>& args)
{
  if(arithmeticResults.empty())
  {
    throw;
  }

  if(args.empty())
  {
    return new ArithmeticValue(arithmeticResults.back());
  }

  int index = static_cast<int>(args[0]->GetValue<ArithmeticType>());
  if(index < 0)
  {
    index = static_cast<int>(arithmeticResults.size()) + index;
  }

  if(index < 0)
  {
    index = 0;
  }
  else if(static_cast<std::size_t>(index) >= arithmeticResults.size())
  {
    index = static_cast<int>(arithmeticResults.size()) - 1;
  }

  return new ArithmeticValue(arithmeticResults.at(static_cast<std::size_t>(index)));
}

std::vector<BitwiseValue> bitwiseResults;
BitwiseValue* bitwiseAns(const std::vector<BitwiseValue*>& args)
{
  if(bitwiseResults.empty())
  {
    throw;
  }

  if(args.empty())
  {
    return new BitwiseValue(bitwiseResults.back());
  }

  int index = static_cast<int>(args[0]->GetValue<BitwiseType>());
  if(index < 0)
  {
    index = static_cast<int>(bitwiseResults.size()) + index;
  }

  if(index < 0)
  {
    index = 0;
  }
  else if(static_cast<std::size_t>(index) >= bitwiseResults.size())
  {
    index = static_cast<int>(bitwiseResults.size()) - 1;
  }

  return new BitwiseValue(bitwiseResults.at(static_cast<std::size_t>(index)));
}

template<class T>
constexpr static double ratio()
{
  return static_cast<double>(T::num) / static_cast<double>(T::den);
}

inline std::unordered_map<std::string, std::unique_ptr<ArithmeticVariable>> arithmeticVariableCache;
inline std::unordered_map<std::string, std::unique_ptr<ArithmeticVariable>> arithmeticNewVariableCache;
inline std::unordered_map<std::string, ArithmeticVariable*> arithmeticVariables;

inline std::unordered_map<std::string, std::unique_ptr<BitwiseVariable>> bitwiseVariableCache;
inline std::unordered_map<std::string, std::unique_ptr<BitwiseVariable>> bitwiseNewVariableCache;
inline std::unordered_map<std::string, BitwiseVariable*> bitwiseVariables;

template<class T>
void arithmeticAddVariable(const std::string identifier, const T& value)
{
  auto newVariable                    = std::make_unique<ArithmeticVariable>(identifier, value);
  auto tmp                            = newVariable.get();
  arithmeticVariableCache[identifier] = std::move(newVariable);
  arithmeticVariables[identifier]     = tmp;
}

ArithmeticValue* arithmeticOnNewVariable(const std::string identifier)
{
  auto newVariable                       = std::make_unique<ArithmeticVariable>(identifier);
  auto result                            = newVariable.get();
  arithmeticNewVariableCache[identifier] = std::move(newVariable);
  arithmeticVariables[identifier]        = result;
  return result;
}

BitwiseValue* bitwiseOnNewVariable(const std::string identifier)
{
  auto newVariable                    = std::make_unique<BitwiseVariable>(identifier);
  auto result                         = newVariable.get();
  bitwiseNewVariableCache[identifier] = std::move(newVariable);
  bitwiseVariables[identifier]        = result;
  return result;
}

template<class... Ts>
ExpressionParser<Ts...> createInstance()
{
  arithmeticVariables.clear();
  arithmeticVariableCache.clear();
  arithmeticNewVariableCache.clear();

  ExpressionParser<Ts...> instance(arithmeticNumberConverter);
  instance.SetVariables(&arithmeticVariables);
  instance.SetOnUnknownIdentifierCallback(arithmeticOnNewVariable);
  instance.SetJuxtapositionOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(a->GetValue<ArithmeticType>() * b->GetValue<ArithmeticType>()); },
      3,
      Associativity::Right);

  instance.AddUnaryOperator([](ArithmeticValue* value) { return new ArithmeticValue(std::abs(value->GetValue<ArithmeticType>())); },
                            '+',
                            4,
                            Associativity::Right);
  instance.AddUnaryOperator([](ArithmeticValue* value) { return new ArithmeticValue(-value->GetValue<ArithmeticType>()); }, '-', 4, Associativity::Right);
  instance.AddUnaryOperator([](ArithmeticValue* value) { return new ArithmeticValue(static_cast<ArithmeticType>(!value->GetValue<ArithmeticType>())); },
                            '!',
                            4,
                            Associativity::Right);

  instance.AddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) {
        if(a->GetType() == typeid(ArithmeticType) && b->GetType() == typeid(ArithmeticType))
        {
          return new ArithmeticValue(a->GetValue<ArithmeticType>() + b->GetValue<ArithmeticType>());
        }
        else
        {
          return new ArithmeticValue(a->ToString() + b->ToString());
        }
      },
      "+",
      1,
      Associativity::Left);
  instance.AddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(a->GetValue<ArithmeticType>() - b->GetValue<ArithmeticType>()); },
      "-",
      1,
      Associativity::Left);
  instance.AddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(a->GetValue<ArithmeticType>() * b->GetValue<ArithmeticType>()); },
      "*",
      2,
      Associativity::Left);
  instance.AddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(a->GetValue<ArithmeticType>() / b->GetValue<ArithmeticType>()); },
      "/",
      2,
      Associativity::Left);
  instance.AddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) {
        return new ArithmeticValue(
            static_cast<ArithmeticType>(static_cast<long>(a->GetValue<ArithmeticType>()) % static_cast<long>(b->GetValue<ArithmeticType>())));
      },
      "%",
      2,
      Associativity::Left);
  instance.AddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(std::pow(a->GetValue<ArithmeticType>(), b->GetValue<ArithmeticType>())); },
      "^",
      3,
      Associativity::Right);

  instance.AddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(std::pow(a->GetValue<ArithmeticType>(), b->GetValue<ArithmeticType>())); },
      "**",
      3,
      Associativity::Right);

  instance.AddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(std::trunc(a->GetValue<ArithmeticType>() / b->GetValue<ArithmeticType>())); },
      "//",
      2,
      Associativity::Right);

  instance.AddBinaryOperator(
      [](ArithmeticValue* lhs, ArithmeticValue* rhs) {
        ArithmeticVariable* variable = lhs->AsPointer<ArithmeticVariable>();
        if(variable == nullptr)
        {
          throw SyntaxException("Assignment of non-variable type: " + lhs->ToString() + " (" + lhs->GetType().name() + ")");
        }

        bool isNewVariable = !variable->IsInitialized();

        if(rhs->GetType() == typeid(ArithmeticType))
        {
          (*variable) = rhs->GetValue<ArithmeticType>();
        }
        else if(rhs->GetType() == typeid(std::string))
        {
          (*variable) = rhs->GetValue<std::string>();
        }
        else if(rhs->GetType() == typeid(std::nullptr_t))
        {
          (*variable) = rhs->GetValue<std::nullptr_t>();
        }
        else
        {
          throw SyntaxException("Assignment from unsupported type: " + rhs->ToString() + " (" + rhs->GetType().name() + ")");
        }

        if(isNewVariable)
        {
          auto variableIterator = arithmeticNewVariableCache.extract(variable->GetIdentifier());
          arithmeticVariableCache.insert(std::move(variableIterator));
        }

        return variable;
      },
      "=",
      4,
      Associativity::Right);

  instance.AddConstant(nullptr, "null");

  instance.AddConstant(ratio<std::giga>(), "G");
  instance.AddConstant(ratio<std::mega>(), "M");
  instance.AddConstant(ratio<std::kilo>(), "k");
  instance.AddConstant(ratio<std::hecto>(), "h");
  instance.AddConstant(ratio<std::deca>(), "da");

  instance.AddConstant(ratio<std::deci>(), "d");
  instance.AddConstant(ratio<std::centi>(), "c");
  instance.AddConstant(ratio<std::milli>(), "m");
  instance.AddConstant(ratio<std::micro>(), "u");
  instance.AddConstant(ratio<std::nano>(), "n");

  instance.AddConstant(M_PI, "math.pi");
  instance.AddConstant(M_E, "math.e");

  instance.AddConstant(299792458.0, "phys.c");
  instance.AddConstant(149597870700.0, "phys.au");

  instance.AddFunction(arithmeticAns, "ans");

  instance.AddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        static_cast<void>(args);
        return new ArithmeticValue(static_cast<ArithmeticType>(std::rand()));
      },
      "random");

  instance.AddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::abs(args[0]->GetValue<ArithmeticType>())); }, "abs");
  instance.AddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(-std::abs(args[0]->GetValue<ArithmeticType>())); }, "neg");

  instance.AddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        return new ArithmeticValue(std::pow(args[0]->GetValue<ArithmeticType>(), args[1]->GetValue<ArithmeticType>()));
      },
      "math.pow");
  instance.AddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        return new ArithmeticValue(std::pow(args[0]->GetValue<ArithmeticType>(), 1.0 / args[1]->GetValue<ArithmeticType>()));
      },
      "math.root");
  instance.AddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::sqrt(args[0]->GetValue<ArithmeticType>())); },
                       "math.sqrt");

  instance.AddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::log(args[0]->GetValue<ArithmeticType>())); },
                       "math.log");
  instance.AddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::log2(args[0]->GetValue<ArithmeticType>())); },
                       "math.log2");
  instance.AddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::log10(args[0]->GetValue<ArithmeticType>())); },
                       "math.log10");

  instance.AddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::sin(args[0]->GetValue<ArithmeticType>())); },
                       "math.sin");
  instance.AddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::cos(args[0]->GetValue<ArithmeticType>())); },
                       "math.cos");
  instance.AddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::tan(args[0]->GetValue<ArithmeticType>())); },
                       "math.tan");

  instance.AddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        ArithmeticType result = std::numeric_limits<ArithmeticType>::max();
        for(const auto& i : args)
        {
          if(i->GetValue<ArithmeticType>() < result)
          {
            result = i->GetValue<ArithmeticType>();
          }
        }

        return new ArithmeticValue(result);
      },
      "min");

  instance.AddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        ArithmeticType result = std::numeric_limits<ArithmeticType>::min();
        for(const auto& i : args)
        {
          if(i->GetValue<ArithmeticType>() > result)
          {
            result = i->GetValue<ArithmeticType>();
          }
        }

        return new ArithmeticValue(result);
      },
      "max");

  instance.AddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        ArithmeticType result = 0.0;
        for(const auto& i : args)
        {
          result += i->GetValue<ArithmeticType>();
        }

        return new ArithmeticValue(result / static_cast<ArithmeticType>(args.size()));
      },
      "math.mean");

  instance.AddFunction(
      [](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(static_cast<ArithmeticType>(args[0]->GetValue<std::string>().length())); },
      "str.len");

  return instance;
}

template<class... Ts>
ExpressionParser<Ts...> createInstance2()
{
  bitwiseVariables.clear();
  bitwiseVariableCache.clear();
  bitwiseNewVariableCache.clear();

  ExpressionParser<Ts...> instance(bitwiseNumberConverter);
  instance.SetVariables(&bitwiseVariables);
  instance.SetOnUnknownIdentifierCallback(bitwiseOnNewVariable);
  /*instance.SetJuxtapositionOperator(
      [](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() & b->GetValue<BitwiseType>()); },
      1,
      Associativity::Right);*/

  instance.AddUnaryOperator([](BitwiseValue* value) { return new BitwiseValue(static_cast<BitwiseType>(!value->GetValue<BitwiseType>())); },
                            '!',
                            4,
                            Associativity::Right);
  instance.AddUnaryOperator([](BitwiseValue* value) { return new BitwiseValue(~value->GetValue<BitwiseType>()); }, '~', 4, Associativity::Right);

  instance.AddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() | b->GetValue<BitwiseType>()); },
                             "|",
                             1,
                             Associativity::Left);
  instance.AddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() & b->GetValue<BitwiseType>()); },
                             "&",
                             1,
                             Associativity::Left);
  instance.AddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() ^ b->GetValue<BitwiseType>()); },
                             "^",
                             2,
                             Associativity::Left);
  instance.AddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() | b->GetValue<BitwiseType>()); },
                             "+",
                             1,
                             Associativity::Left);
  instance.AddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() & b->GetValue<BitwiseType>()); },
                             "*",
                             1,
                             Associativity::Left);
  instance.AddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() << b->GetValue<BitwiseType>()); },
                             "<<",
                             1,
                             Associativity::Left);
  instance.AddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() >> b->GetValue<BitwiseType>()); },
                             ">>",
                             1,
                             Associativity::Left);

  instance.AddBinaryOperator(
      [](BitwiseValue* lhs, BitwiseValue* rhs) {
        BitwiseVariable* variable = lhs->AsPointer<BitwiseVariable>();
        if(variable == nullptr)
        {
          throw SyntaxException("Assignment of non-variable type: " + lhs->ToString() + " (" + lhs->GetType().name() + ")");
        }

        bool isNewVariable = !variable->IsInitialized();

        if(rhs->GetType() == typeid(BitwiseType))
        {
          (*variable) = rhs->GetValue<BitwiseType>();
        }
        else
        {
          throw SyntaxException("Assignment from unsupported type: " + rhs->ToString() + " (" + rhs->GetType().name() + ")");
        }

        if(isNewVariable)
        {
          auto variableIterator = bitwiseNewVariableCache.extract(variable->GetIdentifier());
          bitwiseVariableCache.insert(std::move(variableIterator));
        }

        return variable;
      },
      "=",
      4,
      Associativity::Right);

  instance.AddConstant(static_cast<BitwiseType>(std::numeric_limits<std::uint8_t>::max()), "byte.max");
  instance.AddConstant(static_cast<BitwiseType>(std::numeric_limits<std::uint16_t>::max()), "ushort.max");
  instance.AddConstant(static_cast<BitwiseType>(std::numeric_limits<std::uint32_t>::max()), "uint.max");
  instance.AddConstant(static_cast<BitwiseType>(std::numeric_limits<std::uint64_t>::max()), "ulong.max");

  instance.AddFunction(bitwiseAns, "ans");

  instance.AddFunction(
      [](const std::vector<BitwiseValue*>& args) {
        static_cast<void>(args);
        return new BitwiseValue(static_cast<std::uint64_t>(std::rand()));
      },
      "random");

  return instance;
}

using namespace ::testing;

namespace UnitTest
{
  class ExpressionParser : public Test
  {
    public:
    virtual void SetUp() { std::srand(static_cast<unsigned int>(std::time(nullptr))); }

    virtual void TearDown() {}
  };

  TEST(ExpressionParser, ArithmeticUnaryOperators)
  {
    // Not
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("!1");
      auto expected         = static_cast<double>(!1.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("!0");
      auto expected         = static_cast<double>(!0.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("!-1");
      auto expected         = static_cast<double>(!-1.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, ArithmeticBinaryOperators)
  {
    // Addition
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 + 3");
      auto expected         = 10.0 + 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 + 3");
      auto expected         = (-10.0) + 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 + -3");
      auto expected         = 10.0 + (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 + -3");
      auto expected         = (-10.0) + (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Subtraction
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 - 3");
      auto expected         = 10.0 - 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 - 3");
      auto expected         = (-10.0) - 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 - -3");
      auto expected         = 10.0 - (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 - -3");
      auto expected         = (-10.0) - (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Multiplication
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 * 3");
      auto expected         = 10.0 * 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 * 3");
      auto expected         = (-10.0) * 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 * -3");
      auto expected         = 10.0 * (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 * -3");
      auto expected         = (-10.0) * (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Division
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 / 3");
      auto expected         = 10.0 / 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 / 3");
      auto expected         = (-10.0) / 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 / -3");
      auto expected         = 10.0 / (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 / -3");
      auto expected         = (-10.0) / (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Truncated division
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 // 3");
      auto expected         = std::trunc(10.0 / 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 // 3");
      auto expected         = std::trunc((-10.0) / 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 // -3");
      auto expected         = std::trunc(10.0 / (-3.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 // -3");
      auto expected         = std::trunc((-10.0) / (-3.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Exponentiation 1
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 ^ 3");
      auto expected         = std::pow(10.0, 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 ^ 3");
      auto expected         = std::pow(-10.0, 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 ^ -3");
      auto expected         = std::pow(10.0, -3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 ^ -3");
      auto expected         = std::pow(-10.0, -3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("9 ^ 3");
      auto expected         = std::pow(9.0, 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-9 ^ 3");
      auto expected         = std::pow(-9.0, 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("9 ^ -3");
      auto expected         = std::pow(9.0, -3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-9 ^ -3");
      auto expected         = std::pow(-9.0, -3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Exponentiation 2
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 ** 2");
      auto expected         = std::pow(10.0, 2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 ** 2");
      auto expected         = std::pow(-10.0, 2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 ** -2");
      auto expected         = std::pow(10.0, -2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 ** -2");
      auto expected         = std::pow(-10.0, -2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("9 ** 2");
      auto expected         = std::pow(9.0, 2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-9 ** 2");
      auto expected         = std::pow(-9.0, 2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("9 ** -2");
      auto expected         = std::pow(9.0, -2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-9 ** -2");
      auto expected         = std::pow(-9.0, -2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, Functions)
  {
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("random()");
      ASSERT_GE(actual.GetValue<ArithmeticType>(), 0.0);
      ASSERT_LE(actual.GetValue<ArithmeticType>(), static_cast<double>(RAND_MAX));
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("math.mean(1, 2, 3)");
      auto expected         = (1.0 + 2.0 + 3.0) / 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("math.log10(math.mean(1000, 1000, 1000, 1000, 1000))");
      auto expected         = std::log10((1000.0 + 1000.0 + 1000.0 + 1000.0 + 1000.0) / 5.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("math.mean(min(50, 0, -50), max(-100, 0, 100))");
      auto expected         = (std::min(std::min(50.0, 0.0), -50.0) + std::max(std::max(-100.0, 0.0), 100.0)) / 2.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, Strings)
  {
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("\"abc123\"");
      auto expected         = std::string("abc123");
      ASSERT_EQ(actual.GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("\"abc\" + \"123\"");
      auto expected         = std::string("abc") + std::string("123");
      ASSERT_EQ(actual.GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("\"abc\" + 123");
      auto expected         = std::string("abc") + std::string("123");
      ASSERT_EQ(actual.GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("123 + \"abc\"");
      auto expected         = std::string("123") + std::string("abc");
      ASSERT_EQ(actual.GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("str.len(\"abc123\")");
      auto expected         = static_cast<double>(std::string("abc123").length());
      ASSERT_EQ(actual.GetValue<double>(), expected);
    }
  }

  TEST(ExpressionParser, Variables)
  {
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = 1");
      auto var              = 1.0;
      auto expected         = var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = -1");
      auto var              = -1.0;
      auto expected         = var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = 1 + 1");
      auto var              = 1.0;
      auto expected         = var + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = -1 + 1");
      auto var              = -1.0;
      auto expected         = var + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = (1 + 1)");
      auto var              = 1.0 + 1.0;
      auto expected         = var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = -(1 + 1)");
      auto var              = -(1.0 + 1.0);
      auto expected         = var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = (1 + 1) * 2");
      auto var              = (1.0 + 1.0);
      auto expected         = var * 2.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);

      auto actual2   = expressionParser.Evaluate("var * 5");
      auto expected2 = var * 5.0;
      ASSERT_EQ(actual2.GetValue<ArithmeticType>(), expected2);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = (10^2^3) + var");
      auto var              = std::pow(10.0, std::pow(2.0, 3.0));
      auto expected         = var + var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = -(1 + 1) * 4 + -var");
      auto var              = -(1.0 + 1.0);
      auto expected         = (var * 4.0) + (-var);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto x                = 10.0;
      auto y                = 5.0;
      arithmeticAddVariable("x", x);
      arithmeticAddVariable("y", y);
      auto actual   = expressionParser.Evaluate("x * x - y * y");
      auto expected = (x * x) - (y * y);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["x"]->GetValue<ArithmeticType>(), x);
      ASSERT_EQ(arithmeticVariables["y"]->GetValue<ArithmeticType>(), y);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto x                = 10.0;
      auto y                = 5.0;
      expressionParser.Evaluate("x = 10");
      expressionParser.Evaluate("y = 5");
      auto actual   = expressionParser.Evaluate("x * x - y * y");
      auto expected = (x * x) - (y * y);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["x"]->GetValue<ArithmeticType>(), x);
      ASSERT_EQ(arithmeticVariables["y"]->GetValue<ArithmeticType>(), y);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = var = 1");
      auto var              = 1.0;
      auto expected         = var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var2 = var1 = 1");
      auto var1             = 1.0;
      auto var2             = var1;
      auto expected         = var2;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var1"]->GetValue<ArithmeticType>(), var1);
      ASSERT_EQ(arithmeticVariables["var2"]->GetValue<ArithmeticType>(), var2);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var2 = var1 = 1 + 1");
      auto var1             = 1.0;
      auto var2             = var1;
      auto expected         = var2 + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var1"]->GetValue<ArithmeticType>(), var1);
      ASSERT_EQ(arithmeticVariables["var2"]->GetValue<ArithmeticType>(), var2);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var2 = (var1 = 1) + 1");
      auto var1             = 1.0;
      auto var2             = var1;
      auto expected         = var2 + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var1"]->GetValue<ArithmeticType>(), var1);
      ASSERT_EQ(arithmeticVariables["var2"]->GetValue<ArithmeticType>(), var2);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var2 = (var1 = 1 + 1)");
      auto var1             = 1.0;
      auto var2             = var1 + 1.0;
      auto expected         = var2;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var1"]->GetValue<ArithmeticType>(), var1);
      ASSERT_EQ(arithmeticVariables["var2"]->GetValue<ArithmeticType>(), var2);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var2 = ((var1 = 1) + 1)");
      auto var1             = 1.0;
      auto var2             = var1 + 1.0;
      auto expected         = var2;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var1"]->GetValue<ArithmeticType>(), var1);
      ASSERT_EQ(arithmeticVariables["var2"]->GetValue<ArithmeticType>(), var2);
    }
  }

  TEST(ExpressionParser, Comments)
  {
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1 + 1 #");
      auto expected         = 1.0 + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1 + 1 # comment");
      auto expected         = 1.0 + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1 # + 1");
      auto expected         = 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, Ans)
  {
    auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();

    auto actual1   = expressionParser.Evaluate("1 + 1");
    auto expected1 = 1.0 + 1.0;
    ASSERT_EQ(actual1.GetValue<ArithmeticType>(), expected1);
    arithmeticResults.push_back(actual1);

    auto actual2   = expressionParser.Evaluate("ans() + 1");
    auto expected2 = expected1 + 1.0;
    ASSERT_EQ(actual2.GetValue<ArithmeticType>(), expected2);
    arithmeticResults.push_back(actual2);

    auto actual3   = expressionParser.Evaluate("ans() + ans()");
    auto expected3 = expected2 + expected2;
    ASSERT_EQ(actual3.GetValue<ArithmeticType>(), expected3);
    arithmeticResults.push_back(actual3);

    auto actual4   = expressionParser.Evaluate("ans(0) + ans(0)");
    auto expected4 = expected1 + expected1;
    ASSERT_EQ(actual4.GetValue<ArithmeticType>(), expected4);
    arithmeticResults.push_back(actual4);

    auto actual5   = expressionParser.Evaluate("5 + 5");
    auto expected5 = 5.0 + 5.0;
    ASSERT_EQ(actual5.GetValue<ArithmeticType>(), expected5);
    arithmeticResults.push_back(actual5);

    {
      auto actual   = expressionParser.Evaluate("10 + ans(0)");
      auto expected = 10.0 + expected1;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(1)");
      auto expected = 10.0 + expected2;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(2)");
      auto expected = 10.0 + expected3;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(3)");
      auto expected = 10.0 + expected4;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(4)");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans()");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-1)");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-2)");
      auto expected = 10.0 + expected4;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-3)");
      auto expected = 10.0 + expected3;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-4)");
      auto expected = 10.0 + expected2;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-5)");
      auto expected = 10.0 + expected1;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, Misc)
  {
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("void");
      ASSERT_FALSE(actual.IsInitialized());
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      using expected        = std::runtime_error;
      ASSERT_THROW(expressionParser.Evaluate("void = void + 1"), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("null");
      auto expected         = nullptr;
      ASSERT_EQ(actual.GetValue<std::nullptr_t>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("12345");
      auto expected         = 12345.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-(-5)");
      auto expected         = -(-5.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("3 + --4");
      auto expected         = 3.0 + (-(-4.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("4 + 5 * (5 + 2)");
      auto expected         = 4.0 + (5.0 * (5.0 + 2.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-(10^3)");
      auto expected         = -std::pow(10.0, 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1 + -(10^3)");
      auto expected         = 1.0 + (-std::pow(10.0, 3.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1 + 10^2^3");
      auto expected         = 1.0 + std::pow(10.0, std::pow(2.0, 3.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("3 + 4 * 2 / (1 - 5) ^ 2 ^ 3");
      auto expected         = 3.0 + (4.0 * (2.0 / std::pow(1.0 - 5.0, std::pow(2.0, 3.0))));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("2^1da^-5");
      auto expected         = std::pow(2.0, std::pow(ratio<std::deca>(), -5.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1da^3 * (10 - 2 * 2)");
      auto expected         = std::pow(ratio<std::deca>(), 3.0) * (10.0 - (2.0 * 2.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("math.sin(max(2, 3) / 3 * math.pi)");
      auto expected         = std::sin((std::max(2.0, 3.0) / 3.0) * M_PI);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("(phys.au / phys.c) / 60 # How long light travels from the Sun to Earth");
      auto expected         = (149597870700.0 / 299792458.0) / 60.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("384402000 / phys.c # How long light travels from the Moon to Earth");
      auto expected         = 384402000.0 / 299792458.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, BitwiseOperators)
  {
    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("12345");
      auto expected         = 12345ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("~1");
      auto expected         = ~1;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("~0");
      auto expected         = ~0;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("0 | 0");
      auto expected         = 0ul | 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 | 0");
      auto expected         = 1ul | 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 | 1");
      auto expected         = 1ul | 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("0 & 0");
      auto expected         = 0ul & 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 & 0");
      auto expected         = 1ul & 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 & 1");
      auto expected         = 1ul & 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("0 ^ 0");
      auto expected         = 0ul ^ 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 ^ 0");
      auto expected         = 1ul ^ 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 ^ 1");
      auto expected         = 1ul ^ 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 + 2");
      auto expected         = 1ul | 2ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 * 2");
      auto expected         = 1ul & 2ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 << 0");
      auto expected         = 1ul << 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 << 1");
      auto expected         = 1ul << 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 << 2");
      auto expected         = 1ul << 2ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 >> 0");
      auto expected         = 1ul >> 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("2 >> 1");
      auto expected         = 2ul >> 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("4 >> 2");
      auto expected         = 4ul >> 2ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 | 0) & 1");
      auto expected         = (1ul | 0ul) & 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 | 0) & 0");
      auto expected         = (1ul | 0ul) & 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 & 1) | 0");
      auto expected         = (1ul & 1ul) | 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 & 0) | 1");
      auto expected         = (1ul & 0ul) | 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 ^ 0) & 1");
      auto expected         = (1ul ^ 0ul) & 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 ^ 1) | 1");
      auto expected         = (1ul ^ 1ul) | 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }
  }
} // namespace UnitTest
