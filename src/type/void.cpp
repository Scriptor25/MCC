#include <mcc/type.hpp>

mcc::VoidType::VoidType(TypeContext &context)
    : Type(context)
{
}

std::string mcc::VoidType::String() const
{
    return "void";
}

std::ostream &mcc::VoidType::Print(std::ostream &stream) const
{
    return stream << "void";
}

mcc::ConstantPtr mcc::VoidType::GetNull(const SourceLocation &where) const
{
    return nullptr;
}

bool mcc::VoidType::IsVoid() const
{
    return true;
}
