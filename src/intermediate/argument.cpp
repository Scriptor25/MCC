#include <utility>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::ArgumentValue::Create(const SourceLocation &where, const TypePtr &type, const std::string &name)
{
    return std::make_shared<ArgumentValue>(where, type, name);
}

mcc::ArgumentValue::ArgumentValue(const SourceLocation &where, const TypePtr &type, std::string name)
    : Value(where, type, FieldType_Value),
      Name(std::move(name))
{
}

bool mcc::ArgumentValue::RequireStack() const
{
    return false;
}

mcc::Result mcc::ArgumentValue::GenerateResult() const
{
    const auto stringify = Type->IsString() || Type->IsFunction();

    return {
        .Type = ResultType_Argument,
        .WithArgument = true,
        .Name = std::format("{0}$({1}){0}", stringify ? "\"" : "", Name),
    };
}

mcc::Result mcc::ArgumentValue::GenerateResultUnwrap() const
{
    return {
        .Type = ResultType_Argument,
        .WithArgument = true,
        .Name = std::format("$({})", Name),
    };
}
