#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::ArgumentValue::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type)
{
    auto self  = std::make_shared<ArgumentValue>(where, name, type);
    self->Self = self;
    return self;
}

mcc::ArgumentValue::ArgumentValue(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type)
    : Value(where,
            name,
            type,
            FieldType_::Value)
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
        .Type         = ResultType_::Argument,
        .WithArgument = true,
        .Name         = std::format("{0}$({1}){0}", stringify ? "\"" : "", Name),
    };
}

mcc::Result mcc::ArgumentValue::GenerateResultUnwrap() const
{
    return {
        .Type         = ResultType_::Argument,
        .WithArgument = true,
        .Name         = std::format("$({})", Name),
    };
}
