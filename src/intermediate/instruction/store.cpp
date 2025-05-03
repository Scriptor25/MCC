#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::StoreInstruction::Create(ValuePtr dst, ValuePtr src)
{
    return std::make_shared<StoreInstruction>(dst, src);
}

mcc::StoreInstruction::StoreInstruction(ValuePtr dst, ValuePtr src)
    : Dst(std::move(dst)),
      Src(std::move(src))
{
}

void mcc::StoreInstruction::Gen(std::vector<Command> &commands) const
{
    throw std::runtime_error("TODO");
}

mcc::Command mcc::StoreInstruction::GenInline() const
{
    throw std::runtime_error("TODO");
}
