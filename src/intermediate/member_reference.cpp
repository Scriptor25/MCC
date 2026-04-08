#include <mcc/error.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::MemberReference::Create(
        const SourceLocation &where,
        const ValuePtr &object,
        const std::string &member)
{
    const auto object_type = std::dynamic_pointer_cast<ObjectType>(object->Type);
    Assert(!!object_type, where, "object must be of type object");

    auto type = object_type->Elements.at(member);

    auto self = std::make_shared<MemberReference>(where, type, object, member);

    self->Self = self;
    self->Object->Use(self);

    return self;
}

mcc::MemberReference::MemberReference(
        const SourceLocation &where,
        const TypePtr &type,
        const ValuePtr &object,
        std::string member)
    : Value(where,
            type,
            object->FieldType),
      Object(object),
      Member(std::move(member))
{
}

mcc::MemberReference::~MemberReference()
{
    Object->Drop(Self);
}

bool mcc::MemberReference::RequireStack() const
{
    return Object->RequireStack();
}

mcc::Result mcc::MemberReference::GenerateResult() const
{
    auto object = Object->GenerateResult();
    Assert(object.Type == ResultType_Reference,
           Where,
           "object must be {}, but is {}",
           ResultType_Reference,
           object.Type);

    return {
        .Type          = ResultType_Reference,
        .WithArgument  = object.WithArgument,
        .ReferenceType = object.ReferenceType,
        .Target        = object.Target,
        .Path          = std::format("{}.{}", object.Path, Member),
    };
}
