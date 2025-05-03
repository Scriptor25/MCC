#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::LocalOffsetExpression::LocalOffsetExpression(const double offset)
    : Offset(offset)
{
}

std::ostream &mcc::LocalOffsetExpression::Print(std::ostream &stream) const
{
    if (Offset == 0.0)
        return stream << '^';
    return stream << '^' << Offset;
}

mcc::ValuePtr mcc::LocalOffsetExpression::Gen(Builder &builder, bool inline_) const
{
    return ConstantLocalOffset::Create(Offset);
}
