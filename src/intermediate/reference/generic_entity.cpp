#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::GenericEntityReference::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const ValuePtr &name_val,
        const ValuePtr &path)
{
    auto self = std::make_shared<GenericEntityReference>(where, name, type, name_val, path);

    self->Self = self;
    self->NameVal->Use(self);

    return self;
}

mcc::GenericEntityReference::GenericEntityReference(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const ValuePtr &name_val,
        const ValuePtr &path)
    : Value(where,
            name,
            type,
            FieldType_MutableReference),
      NameVal(name_val),
      Path(path)
{
}

mcc::GenericEntityReference::~GenericEntityReference()
{
    NameVal->Drop(Self);
}

bool mcc::GenericEntityReference::RequireStack() const
{
    return false;
}

mcc::Result mcc::GenericEntityReference::GenerateResult() const
{
    auto name = NameVal->GenerateResultUnwrap();
    auto path = Path->GenerateResultUnwrap();

    auto with_argument = name.WithArgument || path.WithArgument;
    std::string name_value, path_value;

    switch (name.Type)
    {
    case ResultType_Value:
        name_value = name.Value;
        break;

    case ResultType_Argument:
        with_argument = true;
        name_value    = name.Name;
        break;

    default:
        Error(Where, "name must be {} or {}, but is {}", ResultType_Value, ResultType_Argument, name.Type);
    }

    switch (path.Type)
    {
    case ResultType_Value:
        path_value = path.Value;
        break;

    case ResultType_Argument:
        with_argument = true;
        path_value    = path.Name;
        break;

    default:
        Error(Where, "path must be {} or {}, but is {}", ResultType_Value, ResultType_Argument, path.Type);
    }

    return {
        .Type          = ResultType_Reference,
        .WithArgument  = with_argument,
        .ReferenceType = ReferenceType_Entity,
        .Target        = name_value,
        .Path          = path_value,
    };
}
