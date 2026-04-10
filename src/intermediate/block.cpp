#include <mcc/block.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

#include <algorithm>

mcc::BlockPtr mcc::Block::Create(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        const FunctionPtr &parent)
{
    auto self  = std::make_shared<Block>(where, name, context, parent);
    self->Self = self;
    parent->Blocks.push_back(self);
    return self;
}

mcc::Block::Block(
        const SourceLocation &where,
        const std::string &name,
        Context &context,
        FunctionPtr parent)
    : Value(where,
            name,
            context.GetVoid(),
            FieldType_::Value),
      Parent(std::move(parent))
{
}

void mcc::Block::Generate(
        CommandVector &commands,
        const bool stack) const
{
    for (auto &instruction : Instructions)
        instruction->Generate(commands, stack);
}

bool mcc::Block::RequireStack() const
{
    return std::ranges::any_of(
            Instructions,
            [](const InstructionPtr &instruction) { return instruction->RequireStack(); });
}

mcc::InstructionPtr mcc::Block::GetTerminator() const
{
    if (Instructions.empty() || !Instructions.back()->IsTerminator())
        return nullptr;

    return Instructions.back();
}

mcc::ResourceLocation mcc::Block::GetLocation() const
{
    Assert(!!Parent, Where, "parent must not be null");
    return Parent->GetLocation(Self.lock());
}

void mcc::Block::Erase() const
{
    Assert(!!Parent, Where, "parent must not be null");
    Parent->Erase(Self.lock());
}
