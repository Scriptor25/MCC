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

mcc::ValuePtr mcc::StringNode::Generate(Builder &builder, BlockPtr landing_pad) const
{
    return ConstantString::Create(Value);
}
