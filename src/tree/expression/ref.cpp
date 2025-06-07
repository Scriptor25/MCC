#include <utility>
#include <mcc/expression.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::RefExpression::RefExpression(
    const SourceLocation &where,
    TypePtr type,
    const E_ReferenceType target_type,
    ExpressionPtr target_position_x,
    ExpressionPtr target_position_y,
    ExpressionPtr target_position_z,
    ExpressionPtr target_name,
    ExpressionPtr target_location,
    ExpressionPtr path)
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
        TargetLocation->Print(stream << "storage, ");
        break;
    }
    return Path->Print(stream << ", \"") << "\")";
}

mcc::ValuePtr mcc::RefExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    auto path = Path->GenerateValue(builder, frame);

    switch (TargetType)
    {
    // ... block <target x> <target y> <target z> <path>
    case ReferenceType_Block:
    {
        const auto target_position_x = TargetPositionX->GenerateValue(builder, frame);
        const auto target_position_y = TargetPositionY->GenerateValue(builder, frame);
        const auto target_position_z = TargetPositionZ->GenerateValue(builder, frame);
        return GenericBlockReference::Create(
            Where,
            Type,
            target_position_x,
            target_position_y,
            target_position_z,
            path);
    }

    // ... entity <target name> <path>
    case ReferenceType_Entity:
    {
        const auto target_name = TargetName->GenerateValue(builder, frame);
        return GenericEntityReference::Create(Where, Type, target_name, path);
    }

    // ... storage <target location> <path>
    case ReferenceType_Storage:
    {
        const auto target_location = TargetLocation->GenerateValue(builder, frame);
        return GenericStorageReference::Create(Where, Type, target_location, path, true);
    }
    }

    return nullptr;
}
