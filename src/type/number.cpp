#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::NumberType::NumberType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::NumberType::String() const
{
    return "number";
}

std::ostream &mcc::NumberType::Print(std::ostream &stream) const
{
    return stream << "number";
}

mcc::ConstantPtr mcc::NumberType::GetNull(const SourceLocation &where) const
{
    return ConstantNumber::Create(where, Context, 0);
}

bool mcc::NumberType::HasSpecial(const TypePtr &other) const
{
    return false;
}

bool mcc::NumberType::IsNumber() const
{
    return true;
}
