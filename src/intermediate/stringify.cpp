#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::StringifyValue::Create(const SourceLocation &where, const ValuePtr &value)
{
    return std::make_shared<StringifyValue>(where, value);
}

mcc::StringifyValue::StringifyValue(const SourceLocation &where, const ValuePtr &val)
    : Value(where, val->Type->Context.GetString(), false),
      Val(val)
{
    Val->Use();
}

mcc::StringifyValue::~StringifyValue()
{
    Val->Drop();
}

bool mcc::StringifyValue::RequireStack() const
{
    return Val->RequireStack();
}

mcc::Result mcc::StringifyValue::GenerateResult() const
{
    if (Val->Type->IsString())
    {
        return Val->GenerateResult();
    }

    std::string result;

    switch (auto value = Val->GenerateResultUnwrap(); value.Type)
    {
        case ResultType_Value:
            result = std::format("\"{}\"", value.Value);
            break;

        case ResultType_Storage:
            result = std::format("{{\"storage\":\"{}\",\"nbt\":\"{}\"}}", value.Location, value.Path);
            break;

        case ResultType_Argument:
            result = std::format("\"{}\"", value.Name);
            break;

        default:
            Error(
                Where,
                "value must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Argument,
                value.Type);
    }

    return {
        .Type = ResultType_Value,
        .Value = std::move(result),
        .NotNull = true,
    };
}
