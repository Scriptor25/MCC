#include <utility>
#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::MemberReference::Create(
    const SourceLocation &where,
    const ValuePtr &object,
    const std::string &member)
{
    const auto object_type = std::dynamic_pointer_cast<StructType>(object->Type);
    Assert(!!object_type, where, "object must be of type object");

    auto type = object_type->Elements.at(member);
    return std::make_shared<MemberReference>(where, type, object, member);
}

mcc::MemberReference::MemberReference(
    const SourceLocation &where,
    const TypePtr &type,
    const ValuePtr &object,
    std::string member)
    : Value(where, type, object->IsMutable),
      Object(object),
      Member(std::move(member))
{
    Object->Use();
}

mcc::MemberReference::~MemberReference()
{
    Object->Drop();
}

bool mcc::MemberReference::RequireStack() const
{
    return Object->RequireStack();
}

mcc::Result mcc::MemberReference::GenerateResult(bool stringify) const
{
    auto object = Object->GenerateResult(false);
    Assert(object.Type == ResultType_Storage, Where, "object must be {}, but is {}", ResultType_Storage, object.Type);

    return {
        .Type = ResultType_Storage,
        .Location = object.Location,
        .Path = std::format("{}.{}", object.Path, Member),
    };
}
