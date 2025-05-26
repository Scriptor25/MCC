#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::BooleanType::BooleanType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::BooleanType::String() const
{
    return "boolean";
}

std::ostream &mcc::BooleanType::Print(std::ostream &stream) const
{
    return stream << "boolean";
}

mcc::ConstantPtr mcc::BooleanType::GetNull(const SourceLocation &where) const
{
    return ConstantBoolean::Create(where, Context, false);
}

bool mcc::BooleanType::IsBoolean() const
{
    return true;
}
