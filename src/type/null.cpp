#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::NullType::NullType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::NullType::String() const
{
    return "null";
}

std::ostream &mcc::NullType::Print(std::ostream &stream) const
{
    return stream << "null";
}

mcc::ConstantPtr mcc::NullType::GetNull(const SourceLocation &where) const
{
    return ConstantNull::Create(where, Context);
}

bool mcc::NullType::IsNull() const
{
    return true;
}
