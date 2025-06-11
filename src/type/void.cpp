#include <mcc/error.hpp>
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
    Error(where, "cannot get null value for type void");
}

bool mcc::VoidType::HasSpecial(const TypePtr &other) const
{
    return false;
}

bool mcc::VoidType::IsVoid() const
{
    return true;
}
