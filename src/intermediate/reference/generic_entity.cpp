#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::GenericEntityReference::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ValuePtr &name,
    const std::string &path)
{
    return std::make_shared<GenericEntityReference>(where, type, name, path);
}

mcc::GenericEntityReference::GenericEntityReference(
    const SourceLocation &where,
    const TypePtr &type,
    const ValuePtr &name,
    const std::string &path)
    : Value(where, type, true),
      Name(name),
      Path(path)
{
    Name->Use();
}

mcc::GenericEntityReference::~GenericEntityReference()
{
    Name->Drop();
}

bool mcc::GenericEntityReference::RequireStack() const
{
    return false;
}

mcc::Result mcc::GenericEntityReference::GenerateResult() const
{
    auto name = Name->GenerateResultUnwrap();

    auto with_argument = name.WithArgument;
    std::string name_value;

    switch (name.Type)
    {
    case ResultType_Value:
        name_value = name.Value;
        break;

    case ResultType_Argument:
        with_argument = true;
        name_value = name.Name;
        break;

    default:
        Error(Where, "name must be {} or {}, but is {}", ResultType_Value, ResultType_Argument, name.Type);
    }

    return {
        .Type = ResultType_Reference,
        .WithArgument = with_argument,
        .ReferenceType = ReferenceType_Entity,
        .Target = name_value,
        .Path = Path,
    };
}
