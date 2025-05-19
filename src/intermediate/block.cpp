#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::BlockPtr mcc::Block::Create(const SourceLocation &where, const FunctionPtr &parent)
{
    return parent->Blocks.emplace_back(std::make_shared<Block>(where, parent));
}

mcc::Block::Block(const SourceLocation &where, const FunctionPtr &parent)
    : Value(where, TypeContext::GetVoid()),
      Parent(parent)
{
}

void mcc::Block::Generate(CommandVector &commands, const bool stack) const
{
    for (auto &instruction: Instructions)
        instruction->Generate(commands, stack);
}

bool mcc::Block::RequireStack() const
{
    for (auto &instruction: Instructions)
        if (instruction->RequireStack())
            return true;

    return false;
}

mcc::InstructionPtr mcc::Block::GetTerminator() const
{
    if (Instructions.empty() || !Instructions.back()->IsTerminator())
        return nullptr;
    return Instructions.back();
}
