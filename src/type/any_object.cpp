#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::AnyObjectType::AnyObjectType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::AnyObjectType::String() const
{
    return "object";
}

std::ostream &mcc::AnyObjectType::Print(std::ostream &stream) const
{
    return stream << "object";
}

mcc::ConstantPtr mcc::AnyObjectType::GetNull(const SourceLocation &where) const
{
    return ConstantObject::Create(where, Self.lock(), {});
}

bool mcc::AnyObjectType::HasSpecial(const TypePtr &other) const
{
    return other->IsObject();
}

bool mcc::AnyObjectType::IsAny() const
{
    return true;
}

bool mcc::AnyObjectType::IsObject() const
{
    return true;
}
