#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::StringNode::StringNode(std::string value)
    : Value(std::move(value))
{
}

std::ostream &mcc::StringNode::Print(std::ostream &stream) const
{
    return stream << Value;
}

mcc::ValuePtr mcc::StringNode::Gen(Builder &builder, bool inline_) const
{
    return ConstantString::Create(Value);
}
