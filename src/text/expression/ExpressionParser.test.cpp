#include "ExpressionParser.hpp"
#include "text/exception/SyntaxError.hpp"

#include <cmath>
#include <cstdlib>
#include <exception>
#include <limits>
#include <ratio>
#include <sstream>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

using namespace ::testing;
using namespace Text::Expression;

using UnaryOperator  = UnaryOperatorToken;
using BinaryOperator = BinaryOperatorToken;
using Function       = FunctionToken;

using ValueType = double;
using Value     = ValueToken<std::nullptr_t, std::string, std::uint64_t, ValueType>;
using Variable  = VariableToken<std::nullptr_t, std::string, std::uint64_t, ValueType>;

template<class T>
constexpr static ValueType __ratio()
{
  return static_cast<ValueType>(T::num) / static_cast<ValueType>(T::den);
}

static std::unordered_map<char, IUnaryOperatorToken*> __unaryOperators;
static std::unordered_map<std::string, IBinaryOperatorToken*> __binaryOperators;
static std::unordered_map<std::string, IFunctionToken*> __functions;

static std::unordered_map<std::string, std::unique_ptr<Variable>> __variableCache;
static std::unordered_map<std::string, std::unique_ptr<Variable>> __temporaryVariableCache;
static std::unordered_map<std::string, IVariableToken*> __variables;

static Value* __numberConverter(const std::string& value)
{
  std::istringstream iss(value);
  ValueType result;
  iss >> result;
  return new Value(result);
}

static IValueToken* __stringConverter(const std::string& value) { return new Value(value); }

static Value* __onNewVariable(const std::string& identifier)
{
  auto tmpNew                          = std::make_unique<Variable>(identifier);
  auto result                          = tmpNew.get();
  __temporaryVariableCache[identifier] = std::move(tmpNew);
  __variables[identifier]              = result;
  return result;
}

static std::vector<Value> __results;
static Value* __ans(const std::vector<IValueToken*>& args)
{
  if(__results.empty())
  {
    throw;
  }

  if(args.empty())
  {
    return new Value(__results.back());
  }

  int index = static_cast<int>(args[0]->As<Value*>()->GetValue<ValueType>());
  if(index < 0)
  {
    index = static_cast<int>(__results.size()) + index;
  }

  if(index < 0)
  {
    index = 0;
  }
  else if(static_cast<std::size_t>(index) >= __results.size())
  {
    index = static_cast<int>(__results.size()) - 1;
  }

  return new Value(__results.at(static_cast<std::size_t>(index)));
}

static UnaryOperator __unaryOperator_Plus(
    '+',
    [](IValueToken* rhs) { return new Value(std::abs(rhs->As<Value*>()->GetValue<ValueType>())); },
    4,
    Associativity::Right);

static UnaryOperator __unaryOperator_Minus(
    '-',
    [](IValueToken* rhs) { return new Value(-rhs->As<Value*>()->GetValue<ValueType>()); },
    4,
    Associativity::Right);

static UnaryOperator __unaryOperator_Not(
    '!',
    [](IValueToken* rhs) { return new Value(static_cast<ValueType>(!rhs->As<Value*>()->GetValue<ValueType>())); },

    4,
    Associativity::Right);

static UnaryOperator __unaryOperator_TwosComplement(
    '~',
    [](IValueToken* rhs) { return new Value(static_cast<ValueType>(~static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>()))); },

    4,
    Associativity::Right);

static BinaryOperator __binaryOperator_Addition(
    "+",
    [](IValueToken* lhs, IValueToken* rhs) {
      if(lhs->As<Value*>()->GetType() == typeid(ValueType) && rhs->As<Value*>()->GetType() == typeid(ValueType))
      {
        return new Value(lhs->As<Value*>()->GetValue<ValueType>() + rhs->As<Value*>()->GetValue<ValueType>());
      }
      else
      {
        return new Value(lhs->ToString() + rhs->ToString());
      }
    },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_Subtraction(
    "-",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(lhs->As<Value*>()->GetValue<ValueType>() - rhs->As<Value*>()->GetValue<ValueType>()); },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_Multiplication(
    "*",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(lhs->As<Value*>()->GetValue<ValueType>() * rhs->As<Value*>()->GetValue<ValueType>()); },

    2,
    Associativity::Left);

static BinaryOperator __binaryOperator_Division(
    "/",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(lhs->As<Value*>()->GetValue<ValueType>() / rhs->As<Value*>()->GetValue<ValueType>()); },

    2,
    Associativity::Left);

static BinaryOperator __binaryOperator_Remainder(
    "%",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(
          static_cast<ValueType>(static_cast<long>(lhs->As<Value*>()->GetValue<ValueType>()) % static_cast<long>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    2,
    Associativity::Left);

static BinaryOperator __binaryOperator_Exponentiation(
    "**",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(std::pow(lhs->As<Value*>()->GetValue<ValueType>(), rhs->As<Value*>()->GetValue<ValueType>())); },

    3,
    Associativity::Right);

static BinaryOperator __binaryOperator_TruncatedDivision(
    "//",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(std::trunc(lhs->As<Value*>()->GetValue<ValueType>() / rhs->As<Value*>()->GetValue<ValueType>()));
    },

    2,
    Associativity::Right);

static BinaryOperator __binaryOperator_Or(
    "|",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(static_cast<ValueType>(static_cast<std::uint64_t>(lhs->As<Value*>()->GetValue<ValueType>()) |
                                              static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_And(
    "&",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(static_cast<ValueType>(static_cast<std::uint64_t>(lhs->As<Value*>()->GetValue<ValueType>()) &
                                              static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_Xor(
    "^",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(static_cast<ValueType>(static_cast<std::uint64_t>(lhs->As<Value*>()->GetValue<ValueType>()) ^
                                              static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    2,
    Associativity::Left);

static BinaryOperator __binaryOperator_LeftShift(
    "<<",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(static_cast<ValueType>(static_cast<std::uint64_t>(lhs->As<Value*>()->GetValue<ValueType>())
                                              << static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_RightShift(
    ">>",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(static_cast<ValueType>(static_cast<std::uint64_t>(lhs->As<Value*>()->GetValue<ValueType>()) >>
                                              static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_Assignment = BinaryOperator(
    "=",
    [](IValueToken* lhs, IValueToken* rhs) {
      Variable* variable = lhs->As<Variable*>();
      if(variable == nullptr)
      {
        throw Text::Exception::SyntaxError("Assignment of non-variable type: " + lhs->ToString() + " (" + lhs->GetTypeInfo().name() + ")");
      }

      bool isNewVariable = !variable->IsInitialized();

      Value* rhsValue = rhs->As<Value*>();
      if(rhs->GetType() == typeid(ValueType))
      {
        (*variable) = rhsValue->GetValue<ValueType>();
      }
      else if(rhs->GetType() == typeid(std::string))
      {
        (*variable) = rhsValue->GetValue<std::string>();
      }
      else if(rhs->GetType() == typeid(std::nullptr_t))
      {
        (*variable) = rhsValue->GetValue<std::nullptr_t>();
      }
      else
      {
        throw Text::Exception::SyntaxError("Assignment from unsupported type: " + rhs->ToString() + " (" + rhs->GetType().name() + ")");
      }

      if(isNewVariable)
      {
        auto variableIterator = __temporaryVariableCache.extract(variable->GetIdentifier());
        __variableCache.insert(std::move(variableIterator));
      }
      return variable;
    },

    4,
    Associativity::Right);

static Function __function_Ans("ans", __ans, 0u, 1u);

static Function __function_Random(
    "random",
    [](const std::vector<IValueToken*>& args) {
      static_cast<void>(args);
      return new Value(static_cast<ValueType>(std::rand()));
    },
    0u,
    0u);

static Function __function_Abs(
    "abs",
    [](const std::vector<IValueToken*>& args) { return new Value(std::abs(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Neg(
    "neg",
    [](const std::vector<IValueToken*>& args) { return new Value(-std::abs(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Pow(
    "math.pow",
    [](const std::vector<IValueToken*>& args) {
      return new Value(std::pow(args[0]->As<Value*>()->GetValue<ValueType>(), args[1]->As<Value*>()->GetValue<ValueType>()));
    },
    2u,
    2u);

static Function __function_Math_Root(
    "math.root",
    [](const std::vector<IValueToken*>& args) {
      return new Value(std::pow(args[0]->As<Value*>()->GetValue<ValueType>(), 1.0 / args[1]->As<Value*>()->GetValue<ValueType>()));
    },
    2u,
    2u);

static Function __function_Math_Sqrt(
    "math.sqrt",
    [](const std::vector<IValueToken*>& args) { return new Value(std::sqrt(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Log(
    "math.log",
    [](const std::vector<IValueToken*>& args) { return new Value(std::log(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Log2(
    "math.log2",
    [](const std::vector<IValueToken*>& args) { return new Value(std::log2(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Log10(
    "math.log10",
    [](const std::vector<IValueToken*>& args) { return new Value(std::log10(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Sin(
    "math.sin",
    [](const std::vector<IValueToken*>& args) { return new Value(std::sin(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Cos(
    "math.cos",
    [](const std::vector<IValueToken*>& args) { return new Value(std::cos(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Tan(
    "math.tan",
    [](const std::vector<IValueToken*>& args) { return new Value(std::tan(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Min(
    "min",
    [](const std::vector<IValueToken*>& args) {
      ValueType result = std::numeric_limits<ValueType>::max();
      for(const auto& i : args)
      {
        if(i->As<Value*>()->GetValue<ValueType>() < result)
        {
          result = i->As<Value*>()->GetValue<ValueType>();
        }
      }

      return new Value(result);
    },
    1u);

static Function __function_Max(
    "max",
    [](const std::vector<IValueToken*>& args) {
      ValueType result = std::numeric_limits<ValueType>::min();
      for(const auto& i : args)
      {
        if(i->As<Value*>()->GetValue<ValueType>() > result)
        {
          result = i->As<Value*>()->GetValue<ValueType>();
        }
      }

      return new Value(result);
    },
    1u);

static Function __function_Math_Mean(
    "math.mean",
    [](const std::vector<IValueToken*>& args) {
      ValueType result = 0.0;
      for(const auto& i : args)
      {
        result += i->As<Value*>()->GetValue<ValueType>();
      }

      return new Value(result / static_cast<ValueType>(args.size()));
    },
    1u);

static Function __function_StrLen(
    "strlen",
    [](const std::vector<IValueToken*>& args) { return new Value(static_cast<ValueType>(args[0]->As<Value*>()->GetValue<std::string>().length())); },
    1u,
    1u);

static Variable __variable_Null("null", nullptr);
static Variable __variable_Giga("G", __ratio<std::giga>());
static Variable __variable_Mega("M", __ratio<std::mega>());
static Variable __variable_Kilo("k", __ratio<std::kilo>());
static Variable __variable_Hecto("h", __ratio<std::hecto>());
static Variable __variable_Deca("da", __ratio<std::deca>());
static Variable __variable_Deci("d", __ratio<std::deci>());
static Variable __variable_Centi("c", __ratio<std::centi>());
static Variable __variable_Milli("m", __ratio<std::milli>());
static Variable __variable_Micro("u", __ratio<std::micro>());
static Variable __variable_Nano("n", __ratio<std::nano>());
static Variable __variable_Math_Pi("math.pi", M_PI);
static Variable __variable_Math_E("math.e", M_E);
static Variable __variable_Phys_C("phys.c", 299792458.0);
static Variable __variable_Phys_Au("phys.au", 149597870700.0);

static BinaryOperatorToken __binaryOperator_JuxtapositionOperator1(
    "",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(lhs->As<Value*>()->GetValue<ValueType>() * rhs->As<Value*>()->GetValue<ValueType>()); },
    1,
    Associativity::Left);

static BinaryOperatorToken __binaryOperator_JuxtapositionOperator2(
    "",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(lhs->As<Value*>()->GetValue<ValueType>() * rhs->As<Value*>()->GetValue<ValueType>()); },
    3,
    Associativity::Left);

static ExpressionParser createInstance()
{
  __unaryOperators.clear();
  __binaryOperators.clear();
  __functions.clear();
  __variables.clear();
  __variableCache.clear();
  __temporaryVariableCache.clear();

  ExpressionParser instance;
  instance.SetOnParseNumberCallback(__numberConverter);
  instance.SetOnParseStringCallback(__stringConverter);
  instance.SetOnUnknownIdentifierCallback(__onNewVariable);
  instance.SetJuxtapositionOperator(&__binaryOperator_JuxtapositionOperator1);

  instance.SetUnaryOperators(&__unaryOperators);
  instance.SetBinaryOperators(&__binaryOperators);
  instance.SetVariables(&__variables);
  instance.SetFunctions(&__functions);

  __unaryOperators[__unaryOperator_Plus.GetIdentifier()]           = &__unaryOperator_Plus;
  __unaryOperators[__unaryOperator_Minus.GetIdentifier()]          = &__unaryOperator_Minus;
  __unaryOperators[__unaryOperator_Not.GetIdentifier()]            = &__unaryOperator_Not;
  __unaryOperators[__unaryOperator_TwosComplement.GetIdentifier()] = &__unaryOperator_TwosComplement;

  __binaryOperators[__binaryOperator_Addition.GetIdentifier()]          = &__binaryOperator_Addition;
  __binaryOperators[__binaryOperator_Subtraction.GetIdentifier()]       = &__binaryOperator_Subtraction;
  __binaryOperators[__binaryOperator_Multiplication.GetIdentifier()]    = &__binaryOperator_Multiplication;
  __binaryOperators[__binaryOperator_Division.GetIdentifier()]          = &__binaryOperator_Division;
  __binaryOperators[__binaryOperator_Remainder.GetIdentifier()]         = &__binaryOperator_Remainder;
  __binaryOperators[__binaryOperator_Exponentiation.GetIdentifier()]    = &__binaryOperator_Exponentiation;
  __binaryOperators[__binaryOperator_TruncatedDivision.GetIdentifier()] = &__binaryOperator_TruncatedDivision;
  __binaryOperators[__binaryOperator_Or.GetIdentifier()]                = &__binaryOperator_Or;
  __binaryOperators[__binaryOperator_And.GetIdentifier()]               = &__binaryOperator_And;
  __binaryOperators[__binaryOperator_Xor.GetIdentifier()]               = &__binaryOperator_Xor;
  __binaryOperators[__binaryOperator_LeftShift.GetIdentifier()]         = &__binaryOperator_LeftShift;
  __binaryOperators[__binaryOperator_RightShift.GetIdentifier()]        = &__binaryOperator_RightShift;
  __binaryOperators[__binaryOperator_Assignment.GetIdentifier()]        = &__binaryOperator_Assignment;

  __functions[__function_Ans.GetIdentifier()]        = &__function_Ans;
  __functions[__function_Random.GetIdentifier()]     = &__function_Random;
  __functions[__function_Abs.GetIdentifier()]        = &__function_Abs;
  __functions[__function_Math_Pow.GetIdentifier()]   = &__function_Math_Pow;
  __functions[__function_Math_Root.GetIdentifier()]  = &__function_Math_Root;
  __functions[__function_Math_Sqrt.GetIdentifier()]  = &__function_Math_Sqrt;
  __functions[__function_Math_Log.GetIdentifier()]   = &__function_Math_Log;
  __functions[__function_Math_Log2.GetIdentifier()]  = &__function_Math_Log2;
  __functions[__function_Math_Log10.GetIdentifier()] = &__function_Math_Log10;
  __functions[__function_Math_Sin.GetIdentifier()]   = &__function_Math_Sin;
  __functions[__function_Math_Cos.GetIdentifier()]   = &__function_Math_Cos;
  __functions[__function_Math_Tan.GetIdentifier()]   = &__function_Math_Tan;
  __functions[__function_Min.GetIdentifier()]        = &__function_Min;
  __functions[__function_Max.GetIdentifier()]        = &__function_Max;
  __functions[__function_Math_Mean.GetIdentifier()]  = &__function_Math_Mean;
  __functions[__function_StrLen.GetIdentifier()]     = &__function_StrLen;

  __variables[__variable_Null.GetIdentifier()]    = &__variable_Null;
  __variables[__variable_Giga.GetIdentifier()]    = &__variable_Giga;
  __variables[__variable_Mega.GetIdentifier()]    = &__variable_Mega;
  __variables[__variable_Kilo.GetIdentifier()]    = &__variable_Kilo;
  __variables[__variable_Hecto.GetIdentifier()]   = &__variable_Hecto;
  __variables[__variable_Deca.GetIdentifier()]    = &__variable_Deca;
  __variables[__variable_Deci.GetIdentifier()]    = &__variable_Deci;
  __variables[__variable_Centi.GetIdentifier()]   = &__variable_Centi;
  __variables[__variable_Milli.GetIdentifier()]   = &__variable_Milli;
  __variables[__variable_Micro.GetIdentifier()]   = &__variable_Micro;
  __variables[__variable_Nano.GetIdentifier()]    = &__variable_Nano;
  __variables[__variable_Math_Pi.GetIdentifier()] = &__variable_Math_Pi;
  __variables[__variable_Math_E.GetIdentifier()]  = &__variable_Math_E;
  __variables[__variable_Phys_C.GetIdentifier()]  = &__variable_Phys_C;
  __variables[__variable_Phys_Au.GetIdentifier()] = &__variable_Phys_Au;

  return instance;
}

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
      auto instance = createInstance();
      auto actual   = instance.Evaluate("!1");
      auto expected = static_cast<double>(!1.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("!0");
      auto expected = static_cast<double>(!0.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("!-1");
      auto expected = static_cast<double>(!-1.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Two's complement
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("~1");
      auto expected = static_cast<ValueType>(~1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("~0");
      auto expected = static_cast<ValueType>(~0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }
  }

  TEST(ExpressionParser, ArithmeticBinaryOperators)
  {
    // Addition
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 + 3");
      auto expected = 10.0 + 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 + 3");
      auto expected = (-10.0) + 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 + -3");
      auto expected = 10.0 + (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 + -3");
      auto expected = (-10.0) + (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Subtraction
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 - 3");
      auto expected = 10.0 - 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 - 3");
      auto expected = (-10.0) - 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 - -3");
      auto expected = 10.0 - (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 - -3");
      auto expected = (-10.0) - (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Multiplication
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 * 3");
      auto expected = 10.0 * 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 * 3");
      auto expected = (-10.0) * 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 * -3");
      auto expected = 10.0 * (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 * -3");
      auto expected = (-10.0) * (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Division
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 / 3");
      auto expected = 10.0 / 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 / 3");
      auto expected = (-10.0) / 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 / -3");
      auto expected = 10.0 / (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 / -3");
      auto expected = (-10.0) / (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Truncated division
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 // 3");
      auto expected = std::trunc(10.0 / 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 // 3");
      auto expected = std::trunc((-10.0) / 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 // -3");
      auto expected = std::trunc(10.0 / (-3.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 // -3");
      auto expected = std::trunc((-10.0) / (-3.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Exponentiation
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 ** 3");
      auto expected = std::pow(10.0, 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 ** 3");
      auto expected = std::pow(-10.0, 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("10 ** -3");
      auto expected = std::pow(10.0, -3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-10 ** -3");
      auto expected = std::pow(-10.0, -3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("9 ** 3");
      auto expected = std::pow(9.0, 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-9 ** 3");
      auto expected = std::pow(-9.0, 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("9 ** -3");
      auto expected = std::pow(9.0, -3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-9 ** -3");
      auto expected = std::pow(-9.0, -3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Bitwise
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("0 | 0");
      auto expected = static_cast<ValueType>(0ul | 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 | 0");
      auto expected = static_cast<ValueType>(1ul | 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 | 1");
      auto expected = static_cast<ValueType>(1ul | 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("0 & 0");
      auto expected = static_cast<ValueType>(0ul & 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 & 0");
      auto expected = static_cast<ValueType>(1ul & 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 & 1");
      auto expected = static_cast<ValueType>(1ul & 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("0 ^ 0");
      auto expected = static_cast<ValueType>(0ul ^ 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 ^ 0");
      auto expected = static_cast<ValueType>(1ul ^ 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 ^ 1");
      auto expected = static_cast<ValueType>(1ul ^ 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 << 0");
      auto expected = static_cast<ValueType>(1ul << 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 << 1");
      auto expected = static_cast<ValueType>(1ul << 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 << 2");
      auto expected = static_cast<ValueType>(1ul << 2ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 >> 0");
      auto expected = static_cast<ValueType>(1ul >> 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("2 >> 1");
      auto expected = static_cast<ValueType>(2ul >> 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("4 >> 2");
      auto expected = static_cast<ValueType>(4ul >> 2ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("(1 | 0) & 1");
      auto expected = static_cast<ValueType>((1ul | 0ul) & 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("(1 | 0) & 0");
      auto expected = static_cast<ValueType>((1ul | 0ul) & 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("(1 & 1) | 0");
      auto expected = static_cast<ValueType>((1ul & 1ul) | 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("(1 & 0) | 1");
      auto expected = static_cast<ValueType>((1ul & 0ul) | 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("(1 ^ 0) & 1");
      auto expected = static_cast<ValueType>((1ul ^ 0ul) & 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("(1 ^ 1) | 1");
      auto expected = static_cast<ValueType>((1ul ^ 1ul) | 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }
  }

  TEST(ExpressionParser, Functions)
  {
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("random()");
      ASSERT_GE(actual->As<Value*>()->GetValue<ValueType>(), 0.0);
      ASSERT_LE(actual->As<Value*>()->GetValue<ValueType>(), static_cast<double>(RAND_MAX));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("math.mean(1, 2, 3)");
      auto expected = (1.0 + 2.0 + 3.0) / 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("math.log10(math.mean(1000, 1000, 1000, 1000, 1000))");
      auto expected = std::log10((1000.0 + 1000.0 + 1000.0 + 1000.0 + 1000.0) / 5.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("math.mean(min(50, 0, -50), max(-100, 0, 100))");
      auto expected = (std::min(std::min(50.0, 0.0), -50.0) + std::max(std::max(-100.0, 0.0), 100.0)) / 2.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance  = createInstance();
      using expected = Text::Exception::SyntaxError;
      ASSERT_THROW(instance.Evaluate("random"), expected);
    }
  }

  TEST(ExpressionParser, Strings)
  {
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("\"abc123\"");
      auto expected = std::string("abc123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("\"abc\" + \"123\"");
      auto expected = std::string("abc") + std::string("123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("\"abc\" + 123");
      auto expected = std::string("abc") + std::string("123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("123 + \"abc\"");
      auto expected = std::string("123") + std::string("abc");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("strlen(\"abc123\")");
      auto expected = static_cast<double>(std::string("abc123").length());
      ASSERT_EQ(actual->As<Value*>()->GetValue<double>(), expected);
    }

    {
      auto instance  = createInstance();
      using expected = Text::Exception::SyntaxError;
      ASSERT_THROW(instance.Evaluate("\"abc123"), expected);
    }

    {
      auto instance  = createInstance();
      using expected = Text::Exception::SyntaxError;
      ASSERT_THROW(instance.Evaluate("\"abc123\'"), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("\'abc123\'");
      auto expected = std::string("abc123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("\'abc\' + \'123\'");
      auto expected = std::string("abc") + std::string("123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("\'abc\' + 123");
      auto expected = std::string("abc") + std::string("123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("123 + \'abc\'");
      auto expected = std::string("123") + std::string("abc");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("strlen(\'abc123\')");
      auto expected = static_cast<double>(std::string("abc123").length());
      ASSERT_EQ(actual->As<Value*>()->GetValue<double>(), expected);
    }

    {
      auto instance  = createInstance();
      using expected = Text::Exception::SyntaxError;
      ASSERT_THROW(instance.Evaluate("\'abc123"), expected);
    }

    {
      auto instance  = createInstance();
      using expected = Text::Exception::SyntaxError;
      ASSERT_THROW(instance.Evaluate("\'abc123\""), expected);
    }
  }

  TEST(ExpressionParser, Variables)
  {
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var = 1");
      auto var      = 1.0;
      auto expected = var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var = -1");
      auto var      = -1.0;
      auto expected = var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var = 1 + 1");
      auto var      = 1.0;
      auto expected = var + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var = -1 + 1");
      auto var      = -1.0;
      auto expected = var + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var = (1 + 1)");
      auto var      = 1.0 + 1.0;
      auto expected = var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var = -(1 + 1)");
      auto var      = -(1.0 + 1.0);
      auto expected = var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var = (1 + 1) * 2");
      auto var      = (1.0 + 1.0);
      auto expected = var * 2.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);

      auto actual2   = instance.Evaluate("var * 5");
      auto expected2 = var * 5.0;
      ASSERT_EQ(actual2->As<Value*>()->GetValue<ValueType>(), expected2);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var = (10**2**3) + var");
      auto var      = std::pow(10.0, std::pow(2.0, 3.0));
      auto expected = var + var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var = -(1 + 1) * 4 + -var");
      auto var      = -(1.0 + 1.0);
      auto expected = (var * 4.0) + (-var);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto instance = createInstance();
      auto x        = 10.0;
      auto y        = 5.0;
      Variable var_x("x", x);
      Variable var_y("y", y);
      __variables[var_x.GetIdentifier()] = &var_x;
      __variables[var_y.GetIdentifier()] = &var_y;
      auto actual                        = instance.Evaluate("x * x - y * y");
      auto expected                      = (x * x) - (y * y);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["x"]->As<Variable*>()->GetValue<ValueType>(), x);
      ASSERT_EQ(__variables["y"]->As<Variable*>()->GetValue<ValueType>(), y);
    }

    {
      auto instance = createInstance();
      auto x        = 10.0;
      auto y        = 5.0;
      instance.Evaluate("x = 10");
      instance.Evaluate("y = 5");
      auto actual   = instance.Evaluate("x * x - y * y");
      auto expected = (x * x) - (y * y);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["x"]->As<Variable*>()->GetValue<ValueType>(), x);
      ASSERT_EQ(__variables["y"]->As<Variable*>()->GetValue<ValueType>(), y);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var = var = 1");
      auto var      = 1.0;
      auto expected = var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var2 = var1 = 1");
      auto var1     = 1.0;
      auto var2     = var1;
      auto expected = var2;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var1"]->As<Variable*>()->GetValue<ValueType>(), var1);
      ASSERT_EQ(__variables["var2"]->As<Variable*>()->GetValue<ValueType>(), var2);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var2 = var1 = 1 + 1");
      auto var1     = 1.0;
      auto var2     = var1;
      auto expected = var2 + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var1"]->As<Variable*>()->GetValue<ValueType>(), var1);
      ASSERT_EQ(__variables["var2"]->As<Variable*>()->GetValue<ValueType>(), var2);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var2 = (var1 = 1) + 1");
      auto var1     = 1.0;
      auto var2     = var1;
      auto expected = var2 + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var1"]->As<Variable*>()->GetValue<ValueType>(), var1);
      ASSERT_EQ(__variables["var2"]->As<Variable*>()->GetValue<ValueType>(), var2);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var2 = (var1 = 1 + 1)");
      auto var1     = 1.0;
      auto var2     = var1 + 1.0;
      auto expected = var2;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var1"]->As<Variable*>()->GetValue<ValueType>(), var1);
      ASSERT_EQ(__variables["var2"]->As<Variable*>()->GetValue<ValueType>(), var2);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("var2 = ((var1 = 1) + 1)");
      auto var1     = 1.0;
      auto var2     = var1 + 1.0;
      auto expected = var2;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var1"]->As<Variable*>()->GetValue<ValueType>(), var1);
      ASSERT_EQ(__variables["var2"]->As<Variable*>()->GetValue<ValueType>(), var2);
    }
  }

  TEST(ExpressionParser, Comments)
  {
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 + 1 #");
      auto expected = 1.0 + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 + 1 # comment");
      auto expected = 1.0 + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 # + 1");
      auto expected = 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }
  }

  TEST(ExpressionParser, Ans)
  {
    auto instance = createInstance();

    auto actual1   = instance.Evaluate("1 + 1");
    auto expected1 = 1.0 + 1.0;
    ASSERT_EQ(actual1->As<Value*>()->GetValue<ValueType>(), expected1);
    __results.push_back(*actual1->As<Value*>());

    auto actual2   = instance.Evaluate("ans() + 1");
    auto expected2 = expected1 + 1.0;
    ASSERT_EQ(actual2->As<Value*>()->GetValue<ValueType>(), expected2);
    __results.push_back(*actual2->As<Value*>());

    auto actual3   = instance.Evaluate("ans() + ans()");
    auto expected3 = expected2 + expected2;
    ASSERT_EQ(actual3->As<Value*>()->GetValue<ValueType>(), expected3);
    __results.push_back(*actual3->As<Value*>());

    auto actual4   = instance.Evaluate("ans(0) + ans(0)");
    auto expected4 = expected1 + expected1;
    ASSERT_EQ(actual4->As<Value*>()->GetValue<ValueType>(), expected4);
    __results.push_back(*actual4->As<Value*>());

    auto actual5   = instance.Evaluate("5 + 5");
    auto expected5 = 5.0 + 5.0;
    ASSERT_EQ(actual5->As<Value*>()->GetValue<ValueType>(), expected5);
    __results.push_back(*actual5->As<Value*>());

    {
      auto actual   = instance.Evaluate("10 + ans(0)");
      auto expected = 10.0 + expected1;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = instance.Evaluate("10 + ans(1)");
      auto expected = 10.0 + expected2;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = instance.Evaluate("10 + ans(2)");
      auto expected = 10.0 + expected3;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = instance.Evaluate("10 + ans(3)");
      auto expected = 10.0 + expected4;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = instance.Evaluate("10 + ans(4)");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = instance.Evaluate("10 + ans()");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = instance.Evaluate("10 + ans(-1)");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = instance.Evaluate("10 + ans(-2)");
      auto expected = 10.0 + expected4;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = instance.Evaluate("10 + ans(-3)");
      auto expected = 10.0 + expected3;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = instance.Evaluate("10 + ans(-4)");
      auto expected = 10.0 + expected2;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = instance.Evaluate("10 + ans(-5)");
      auto expected = 10.0 + expected1;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }
  }

  TEST(ExpressionParser, Misc)
  {
    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("void");
      ASSERT_FALSE(actual->As<Variable*>()->IsInitialized());
    }

    {
      auto instance  = createInstance();
      using expected = std::runtime_error;
      ASSERT_THROW(instance.Evaluate("void = void + 1"), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("null");
      auto expected = nullptr;
      ASSERT_TRUE(actual->As<Variable*>()->IsInitialized());
      ASSERT_EQ(actual->As<Variable*>()->GetValue<std::nullptr_t>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("12345");
      auto expected = 12345.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-(-5)");
      auto expected = -(-5.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("3 + --4");
      auto expected = 3.0 + (-(-4.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("4 + 5 * (5 + 2)");
      auto expected = 4.0 + (5.0 * (5.0 + 2.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("-(10**3)");
      auto expected = -std::pow(10.0, 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 + -(10**3)");
      auto expected = 1.0 + (-std::pow(10.0, 3.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1 + 10**2**3");
      auto expected = 1.0 + std::pow(10.0, std::pow(2.0, 3.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      instance.SetJuxtapositionOperator(&__binaryOperator_JuxtapositionOperator1);
      auto actual   = instance.Evaluate("6 / 2(1 + 2)");
      auto expected = (6.0 / 2.0) * (1.0 + 2.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      instance.SetJuxtapositionOperator(&__binaryOperator_JuxtapositionOperator2);
      auto actual   = instance.Evaluate("6 / 2(1 + 2)");
      auto expected = 6.0 / (2.0 * (1.0 + 2.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      instance.SetJuxtapositionOperator(nullptr);
      using expected = Text::Exception::SyntaxError;
      ASSERT_THROW(instance.Evaluate("6 / 2(1 + 2)"), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("3 + 4 * 2 / (1 - 5) ** 2 ** 3");
      auto expected = 3.0 + (4.0 * (2.0 / std::pow(1.0 - 5.0, std::pow(2.0, 3.0))));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("2**(1da)**-5");
      auto expected = std::pow(2.0, std::pow(__ratio<std::deca>(), -5.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("1da**3 * (10 - 2 * 2)");
      auto expected = std::pow(__ratio<std::deca>(), 3.0) * (10.0 - (2.0 * 2.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("math.sin(max(2, 3) / 3 * math.pi)");
      auto expected = std::sin((std::max(2.0, 3.0) / 3.0) * M_PI);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("(phys.au / phys.c) / 60 # How long light travels from the Sun to Earth");
      auto expected = (149597870700.0 / 299792458.0) / 60.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Evaluate("384402000 / phys.c # How long light travels from the Moon to Earth");
      auto expected = 384402000.0 / 299792458.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }
  }
} // namespace UnitTest
