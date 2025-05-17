#include <mcc/constant.hpp>
#include <mcc/format.hpp>

mcc::StringNode::StringNode(SourceLocation where, std::string value)
    : FormatNode(std::move(where)),
      Value(std::move(value))
{
}

std::ostream &mcc::StringNode::Print(std::ostream &stream) const
{
    return stream << Value;
}

bool mcc::StringNode::IsConstant() const
{
    return true;
}

mcc::ValuePtr mcc::StringNode::Generate(Builder &builder) const
{
    return ConstantString::Create(Value);
}
