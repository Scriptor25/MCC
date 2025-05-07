#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantOffset::Create(const OffsetTypeE type, ConstantPtr offset)
{
    return std::make_shared<ConstantOffset>(type, std::move(offset));
}

mcc::ConstantOffset::ConstantOffset(const OffsetTypeE type, ConstantPtr offset)
    : Type(type),
      Offset(std::move(offset))
{
}

mcc::Result mcc::ConstantOffset::GenerateResult(const bool stringify, const bool use_stack) const
{
    if (!Offset)
        return {
            .Type = ResultType_Value,
            .Value = ToString(Type),
        };

    auto offset = Offset->GenerateResult(stringify, use_stack);
    Assert(offset.Type == ResultType_Value, "offset must be {}, but is {}", ResultType_Value, offset.Type);

    return {
        .Type = ResultType_Value,
        .Value = ToString(Type) + offset.Value,
    };
}
