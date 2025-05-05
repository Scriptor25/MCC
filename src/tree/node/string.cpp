#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::StringNode::StringNode(SourceLocation where, std::string value)
    : FormatNode(std::move(where)),
      Value(std::move(value))
{
}

std::ostream &mcc::StringNode::Print(std::ostream &stream) const
{
    return stream << Value;
}

mcc::ValuePtr mcc::StringNode::Generate(Builder &builder, bool inline_) const
{
    return ConstantString::Create(Value);
}
