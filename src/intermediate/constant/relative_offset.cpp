#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantRelativeOffset::Create(FloatT offset)
{
    return std::make_shared<ConstantRelativeOffset>(offset);
}

mcc::ConstantRelativeOffset::ConstantRelativeOffset(const FloatT offset)
    : Offset(offset)
{
}

mcc::CommandResult mcc::ConstantRelativeOffset::GenResult(const bool stringify) const
{
    return {
        .Type = CommandResultType_Value,
        .Value = '~' + std::to_string(Offset),
    };
}
