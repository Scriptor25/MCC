#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::StringType::StringType(Context &context)
    : Type(context)
{
}

std::string mcc::StringType::String() const
{
    return "string";
}

std::ostream &mcc::StringType::Print(std::ostream &stream) const
{
    return stream << "string";
}

mcc::ConstantPtr mcc::StringType::GetNull(const SourceLocation &where) const
{
    return ConstantString::Create(where, Types, {});
}

bool mcc::StringType::HasSpecialization(const TypePtr &other) const
{
    return false;
}

bool mcc::StringType::IsString() const
{
    return true;
}
