#include <mcc/constant.hpp>
#include <mcc/format.hpp>

mcc::StringNode::StringNode(const SourceLocation &where, const std::string &value)
    : FormatNode(where),
      Value(value)
{
}

std::ostream &mcc::StringNode::Print(std::ostream &stream) const
{
    return stream << Value;
}

mcc::ValuePtr mcc::StringNode::Generate(Builder &builder, const Frame &frame) const
{
    return ConstantString::Create(Where, Value);
}
