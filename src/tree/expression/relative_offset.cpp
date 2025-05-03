#include <mcc/tree.hpp>

mcc::RelativeOffsetExpression::RelativeOffsetExpression(const double offset)
    : Offset(offset)
{
}

std::ostream &mcc::RelativeOffsetExpression::Print(std::ostream &stream) const
{
    if (Offset == 0.0)
        return stream << '~';
    return stream << '~' << Offset;
}

mcc::ValuePtr mcc::RelativeOffsetExpression::Gen(Builder &builder, bool inline_) const
{
    throw std::runtime_error("TODO: RelativeOffsetExpression::Gen");
}
