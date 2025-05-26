#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::StringType::StringType(TypeContext &context)
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
    return ConstantString::Create(where, Context, "");
}

bool mcc::StringType::IsString() const
{
    return true;
}
