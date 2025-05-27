#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::GenericBlockReference::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ValuePtr &position_x,
    const ValuePtr &position_y,
    const ValuePtr &position_z,
    const std::string &path)
{
    return std::make_shared<GenericBlockReference>(where, type, position_x, position_y, position_z, path);
}

mcc::GenericBlockReference::GenericBlockReference(
    const SourceLocation &where,
    const TypePtr &type,
    const ValuePtr &position_x,
    const ValuePtr &position_y,
    const ValuePtr &position_z,
    const std::string &path)
    : Value(where, type, true),
      PositionX(position_x),
      PositionY(position_y),
      PositionZ(position_z),
      Path(path)
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

    Assert(x.Type == ResultType_Value, Where, "x must be {}, but is {}", ResultType_Value, x.Type);
    Assert(y.Type == ResultType_Value, Where, "y must be {}, but is {}", ResultType_Value, y.Type);
    Assert(z.Type == ResultType_Value, Where, "z must be {}, but is {}", ResultType_Value, z.Type);

    return {
        .Type = ResultType_Reference,
        .ReferenceType = ReferenceType_Block,
        .Target = std::format("{} {} {}", x.Value, y.Value, z.Value),
        .Path = Path,
    };
}
