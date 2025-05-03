#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantLocalOffset::Create(FloatT offset)
{
    return std::make_shared<ConstantLocalOffset>(offset);
}

mcc::ConstantLocalOffset::ConstantLocalOffset(const FloatT offset)
    : Offset(offset)
{
}

mcc::CommandResult mcc::ConstantLocalOffset::GenResult(const bool stringify) const
{
    return {
        .Type = CommandResultType_Value,
        .Value = '^' + std::to_string(Offset),
    };
}
