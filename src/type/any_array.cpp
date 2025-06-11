#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::AnyArrayType::AnyArrayType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::AnyArrayType::String() const
{
    return "array";
}

std::ostream &mcc::AnyArrayType::Print(std::ostream &stream) const
{
    return stream << "array";
}

mcc::ConstantPtr mcc::AnyArrayType::GetNull(const SourceLocation &where) const
{
    return ConstantArray::Create(where, Self.lock(), {}, false);
}

bool mcc::AnyArrayType::HasSpecial(const TypePtr &other) const
{
    return other->IsArray();
}

bool mcc::AnyArrayType::IsAny() const
{
    return true;
}

bool mcc::AnyArrayType::IsArray() const
{
    return true;
}
