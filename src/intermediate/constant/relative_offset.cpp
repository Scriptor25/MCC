#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantRelativeOffset::Create(FloatT offset)
{
    return std::make_shared<ConstantRelativeOffset>(offset);
}

mcc::ConstantRelativeOffset::ConstantRelativeOffset(const FloatT offset)
    : Offset(offset)
{
}

mcc::Command mcc::ConstantRelativeOffset::GenInline() const
{
    return "~" + std::to_string(Offset);
}
