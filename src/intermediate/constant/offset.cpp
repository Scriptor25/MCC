#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantOffset::Create(OffsetTypeE type, FloatT offset)
{
    return std::make_shared<ConstantOffset>(type, offset);
}

mcc::ConstantOffset::ConstantOffset(const OffsetTypeE type, const FloatT offset)
    : Type(type),
      Offset(offset)
{
}

mcc::CommandResult mcc::ConstantOffset::GenResult(const bool stringify) const
{
    char prefix = 0;
    switch (Type)
    {
        case OffsetType_Relative:
            prefix = '~';
            break;

        case OffsetType_Local:
            prefix = '^';
            break;
    }

    return {
        .Type = CommandResultType_Value,
        .Value = prefix + std::to_string(Offset),
    };
}
