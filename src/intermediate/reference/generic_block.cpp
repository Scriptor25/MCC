#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::GenericBlockReference::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const ValuePtr &position_x,
        const ValuePtr &position_y,
        const ValuePtr &position_z,
        const ValuePtr &path)
{
    auto self = std::make_shared<GenericBlockReference>(where, name, type, position_x, position_y, position_z, path);

    self->Self = self;
    self->PositionX->Use(self);
    self->PositionY->Use(self);
    self->PositionZ->Use(self);

    return self;
}

mcc::GenericBlockReference::GenericBlockReference(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const ValuePtr &position_x,
        const ValuePtr &position_y,
        const ValuePtr &position_z,
        const ValuePtr &path)
    : Value(where,
            name,
            type,
            FieldType_ImmutableReference),
      PositionX(position_x),
      PositionY(position_y),
      PositionZ(position_z),
      Path(path)
{
}

mcc::GenericBlockReference::~GenericBlockReference()
{
    PositionX->Drop(Self);
    PositionY->Drop(Self);
    PositionZ->Drop(Self);
}

bool mcc::GenericBlockReference::RequireStack() const
{
    return false;
}

mcc::Result mcc::GenericBlockReference::GenerateResult() const
{
    auto x    = PositionX->GenerateResultUnwrap();
    auto y    = PositionY->GenerateResultUnwrap();
    auto z    = PositionZ->GenerateResultUnwrap();
    auto path = Path->GenerateResultUnwrap();

    auto with_argument = x.WithArgument || y.WithArgument || z.WithArgument || path.WithArgument;
    std::string x_value, y_value, z_value, path_value;

    switch (x.Type)
    {
    case ResultType_Value:
        x_value = x.Value;
        break;

    case ResultType_Argument:
        with_argument = true;
        x_value       = x.Name;
        break;

    default:
        Error(Where, "x must be {} or {}, but is {}", ResultType_Value, ResultType_Argument, x.Type);
    }

    switch (y.Type)
    {
    case ResultType_Value:
        y_value = y.Value;
        break;

    case ResultType_Argument:
        with_argument = true;
        y_value       = y.Name;
        break;

    default:
        Error(Where, "y must be {} or {}, but is {}", ResultType_Value, ResultType_Argument, y.Type);
    }

    switch (z.Type)
    {
    case ResultType_Value:
        z_value = z.Value;
        break;

    case ResultType_Argument:
        with_argument = true;
        z_value       = z.Name;
        break;

    default:
        Error(Where, "z must be {} or {}, but is {}", ResultType_Value, ResultType_Argument, z.Type);
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
        .ReferenceType = ReferenceType_Block,
        .Target        = std::format("{} {} {}", x_value, y_value, z_value),
        .Path          = std::move(path_value),
    };
}
