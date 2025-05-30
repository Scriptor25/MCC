#include <utility>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::RefExpression::RefExpression(
    const SourceLocation &where,
    TypePtr type,
    const ReferenceTypeE target_type,
    ExpressionPtr target_position_x,
    ExpressionPtr target_position_y,
    ExpressionPtr target_position_z,
    ExpressionPtr target_name,
    ResourceLocation target_location,
    std::string path)
    : Expression(where),
      Type(std::move(type)),
      TargetType(target_type),
      TargetPositionX(std::move(target_position_x)),
      TargetPositionY(std::move(target_position_y)),
      TargetPositionZ(std::move(target_position_z)),
      TargetName(std::move(target_name)),
      TargetLocation(std::move(target_location)),
      Path(std::move(path))
{
}

std::ostream &mcc::RefExpression::Print(std::ostream &stream) const
{
    Type->Print(stream << "ref<") << ">(";
    switch (TargetType)
    {
    case ReferenceType_Block:
        TargetPositionZ->Print(TargetPositionY->Print(TargetPositionX->Print(stream << "block, ") << ", ") << ", ");
        break;
    case ReferenceType_Entity:
        TargetName->Print(stream << "entity, ");
        break;
    case ReferenceType_Storage:
        TargetLocation.Print(stream << "storage, ");
        break;
    }
    return stream << ", \"" << Path << "\")";
}

mcc::ValuePtr mcc::RefExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    switch (TargetType)
    {
    // ... block <target x> <target y> <target z> <path>
    case ReferenceType_Block:
    {
        const auto position_x = TargetPositionX->GenerateValue(builder, frame);
        const auto position_y = TargetPositionY->GenerateValue(builder, frame);
        const auto position_z = TargetPositionZ->GenerateValue(builder, frame);
        return GenericBlockReference::Create(Where, Type, position_x, position_y, position_z, Path);
    }
    // ... entity <target name> <path>
    case ReferenceType_Entity:
    {
        const auto name = TargetName->GenerateValue(builder, frame);
        return GenericEntityReference::Create(Where, Type, name, Path);
    }
    // ... storage <target location> <path>
    case ReferenceType_Storage:
        return GenericStorageReference::Create(Where, Type, TargetLocation, Path);
    }

    Error(Where, "TODO");
}
