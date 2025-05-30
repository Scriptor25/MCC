#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::StringifyValue::Create(const SourceLocation &where, const ValuePtr &target)
{
    return std::make_shared<StringifyValue>(where, target);
}

mcc::StringifyValue::StringifyValue(const SourceLocation &where, const ValuePtr &target)
    : Value(where, target->Type->Context.GetString(), false),
      Target(target)
{
    Target->Use();
}

mcc::StringifyValue::~StringifyValue()
{
    Target->Drop();
}

bool mcc::StringifyValue::RequireStack() const
{
    return Target->RequireStack();
}

mcc::Result mcc::StringifyValue::GenerateResult() const
{
    if (Target->Type->IsString())
        return Target->GenerateResult();

    auto target = Target->GenerateResultUnwrap();

    std::string target_value;
    switch (target.Type)
    {
    case ResultType_Value:
        target_value = std::format("\"{}\"", target.Value);
        break;

    case ResultType_Reference:
        target_value = std::format(
            "{{\"{}\":\"{}\",\"nbt\":\"{}\"}}",
            target.ReferenceType,
            target.Target,
            target.Path);
        break;

    case ResultType_Argument:
        return {
            .Type = ResultType_Argument,
            .Name = std::format("\"{}\"", target.Name),
        };

    default:
        Error(
            Where,
            "value must be {}, {} or {}, but is {}",
            ResultType_Value,
            ResultType_Reference,
            ResultType_Argument,
            target.Type);
    }

    return {
        .Type = ResultType_Value,
        .WithArgument = target.WithArgument,
        .Value = std::move(target_value),
        .NotNull = true,
    };
}
