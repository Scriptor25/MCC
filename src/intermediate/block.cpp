#include <utility>
#include <mcc/block.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::BlockPtr mcc::Block::Create(const SourceLocation &where, TypeContext &context, const FunctionPtr &parent)
{
    auto block = parent->Blocks.emplace_back(std::make_shared<Block>(where, context, parent));
    block->Self = block;
    return block;
}

mcc::Block::Block(const SourceLocation &where, TypeContext &context, FunctionPtr parent)
    : Value(where, context.GetVoid(), FieldType_Value),
      Parent(std::move(parent))
{
}

void mcc::Block::Generate(CommandVector &commands, const bool stack) const
{
    for (auto &instruction : Instructions)
        instruction->Generate(commands, stack);
}

bool mcc::Block::RequireStack() const
{
    return std::ranges::any_of(
        Instructions,
        [](auto &instruction)
        {
            return instruction->RequireStack();
        });
}

mcc::InstructionPtr mcc::Block::GetTerminator() const
{
    if (Instructions.empty() || !Instructions.back()->IsTerminator())
        return nullptr;

    return Instructions.back();
}

mcc::ResourceLocation mcc::Block::GetLocation() const
{
    return Parent->GetLocation(Self.lock());
}

void mcc::Block::Erase() const
{
    Parent->Erase(Self.lock());
}
