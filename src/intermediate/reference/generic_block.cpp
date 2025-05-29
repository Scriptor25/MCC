#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::GenericBlockReference::Create(const SourceLocation &where, const TypePtr &type, const ValuePtr &position_x, const ValuePtr &position_y, const ValuePtr &position_z, const std::string &path)
{
    return std::make_shared<GenericBlockReference>(where, type, position_x, position_y, position_z, path);
}

mcc::GenericBlockReference::GenericBlockReference(const SourceLocation &where, const TypePtr &type, const ValuePtr &position_x, const ValuePtr &position_y, const ValuePtr &position_z, const std::string &path)
    : Value(where, type, true), PositionX(position_x), PositionY(position_y), PositionZ(position_z), Path(path)
{
    PositionX->Use();
    PositionY->Use();
    PositionZ->Use();
}

mcc::GenericBlockReference::~GenericBlockReference()
{
    PositionX->Drop();
    PositionY->Drop();
    PositionZ->Drop();
}

bool mcc::GenericBlockReference::RequireStack() const
{
    return false;
}

mcc::Result mcc::GenericBlockReference::GenerateResult() const
{
    auto x = PositionX->GenerateResultUnwrap();
    auto y = PositionY->GenerateResultUnwrap();
    auto z = PositionZ->GenerateResultUnwrap();

    auto with_argument = x.WithArgument || y.WithArgument || z.WithArgument;
    std::string x_value, y_value, z_value;

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

    return {
        .Type          = ResultType_Reference,
        .WithArgument  = with_argument,
        .ReferenceType = ReferenceType_Block,
        .Target        = std::format("{} {} {}", x_value, y_value, z_value),
        .Path          = Path,
    };
}
