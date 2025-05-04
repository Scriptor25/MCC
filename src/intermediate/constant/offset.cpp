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

mcc::Result mcc::ConstantOffset::GenResult(const bool stringify, bool use_stack) const
{
    return {
        .Type = ResultType_Value,
        .Value = ToString(Type) + std::to_string(Offset),
    };
}
