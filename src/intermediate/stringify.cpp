#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::StringifyValue::Create(
        const SourceLocation &where,
        const std::string &name,
        const ValuePtr &target)
{
    auto self = std::make_shared<StringifyValue>(where, name, target);

    self->Self = self;
    self->Target->Use(self);

    return self;
}

mcc::StringifyValue::StringifyValue(
        const SourceLocation &where,
        const std::string &name,
        const ValuePtr &target)
    : Value(where,
            name,
            target->Type->Context.GetString(),
            FieldType_::Value),
      Target(target)
{
}

mcc::StringifyValue::~StringifyValue()
{
    Target->Drop(Self);
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
    case ResultType_::Value:
        target_value = std::format("\"{}\"", target.Value);
        break;

    case ResultType_::Reference:
        target_value =
                std::format("{{\"{}\":\"{}\",\"nbt\":\"{}\"}}", target.ReferenceType, target.Target, target.Path);
        break;

    case ResultType_::Argument:
        return {
            .Type = ResultType_::Argument,
            .Name = std::format("\"{}\"", target.Name),
        };

    default:
        Error(Where,
              "value must be {}, {} or {}, but is {}",
              ResultType_::Value,
              ResultType_::Reference,
              ResultType_::Argument,
              target.Type);
    }

    return {
        .Type         = ResultType_::Value,
        .WithArgument = target.WithArgument,
        .Value        = std::move(target_value),
        .NotNull      = true,
    };
}
