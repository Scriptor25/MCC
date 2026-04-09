#include <mcc/error.hpp>
#include <mcc/value.hpp>
#include <utility>

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
        ValuePtr position_x,
        ValuePtr position_y,
        ValuePtr position_z,
        ValuePtr path)
    : Value(where,
            name,
            type,
            FieldType_::ImmutableReference),
      PositionX(std::move(position_x)),
      PositionY(std::move(position_y)),
      PositionZ(std::move(position_z)),
      Path(std::move(path))
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
    case ResultType_::Value:
        x_value = x.Value;
        break;

    case ResultType_::Argument:
        with_argument = true;
        x_value       = x.Name;
        break;

    default:
        Error(Where, "x must be {} or {}, but is {}", ResultType_::Value, ResultType_::Argument, x.Type);
    }

    switch (y.Type)
    {
    case ResultType_::Value:
        y_value = y.Value;
        break;

    case ResultType_::Argument:
        with_argument = true;
        y_value       = y.Name;
        break;

    default:
        Error(Where, "y must be {} or {}, but is {}", ResultType_::Value, ResultType_::Argument, y.Type);
    }

    switch (z.Type)
    {
    case ResultType_::Value:
        z_value = z.Value;
        break;

    case ResultType_::Argument:
        with_argument = true;
        z_value       = z.Name;
        break;

    default:
        Error(Where, "z must be {} or {}, but is {}", ResultType_::Value, ResultType_::Argument, z.Type);
    }

    switch (path.Type)
    {
    case ResultType_::Value:
        path_value = path.Value;
        break;

    case ResultType_::Argument:
        with_argument = true;
        path_value    = path.Name;
        break;

    default:
        Error(Where, "path must be {} or {}, but is {}", ResultType_::Value, ResultType_::Argument, path.Type);
    }

    return {
        .Type          = ResultType_::Reference,
        .WithArgument  = with_argument,
        .ReferenceType = ReferenceType_::Block,
        .Target        = std::format("{} {} {}", x_value, y_value, z_value),
        .Path          = std::move(path_value),
    };
}
