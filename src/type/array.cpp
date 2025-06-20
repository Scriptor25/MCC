#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ArrayType::ArrayType(TypeContext &context, TypePtr elements)
    : Type(context),
      Elements(std::move(elements))
{
}

std::string mcc::ArrayType::String() const
{
    return Elements->String() + "[]";
}

std::ostream &mcc::ArrayType::Print(std::ostream &stream) const
{
    return Elements->Print(stream) << "[]";
}

mcc::ConstantPtr mcc::ArrayType::GetNull(const SourceLocation &where) const
{
    return ConstantArray::Create(where, Self.lock(), {}, false);
}

bool mcc::ArrayType::HasSpecial(const TypePtr &other) const
{
    if (other->IsAny() || !other->IsArray())
        return false;

    const auto other_array = std::dynamic_pointer_cast<ArrayType>(other);
    return SameOrSpecial(other_array->Elements, Elements);
}

bool mcc::ArrayType::IsArray() const
{
    return true;
}
