#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantLocalOffset::Create(FloatT offset)
{
    return std::make_shared<ConstantLocalOffset>(offset);
}

mcc::ConstantLocalOffset::ConstantLocalOffset(const FloatT offset)
    : Offset(offset)
{
}

mcc::Command mcc::ConstantLocalOffset::GenInline() const
{
    return "^" + std::to_string(Offset);
}
