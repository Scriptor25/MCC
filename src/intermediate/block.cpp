#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::BlockPtr mcc::Block::CreateTopLevel(ResourceLocation location, std::vector<std::string> parameters)
{
    return std::make_shared<Block>(std::move(location), std::move(parameters));
}

mcc::BlockPtr mcc::Block::Create(const BlockPtr &parent, ResourceLocation location)
{
    auto block = std::make_shared<Block>(std::move(location), std::vector<std::string>());
    block->Parent = parent;
    parent->Children.push_back(block);
    return std::move(block);
}

mcc::Block::Block(ResourceLocation location, std::vector<std::string> parameters)
    : Location(std::move(location)),
      Parameters(std::move(parameters))
{
}

void mcc::Block::Generate(CommandVector &commands, const bool stack) const
{
    auto require_stack = stack || RequireStack();

    if (!require_stack)
        require_stack = std::ranges::any_of(
            Children,
            [](auto &child)
            {
                return child->RequireStack();
            });

    if (require_stack && !Parent)
    {
        commands.Append("data modify storage {} stack prepend value {{}}", Location);

        for (auto &parameter: Parameters)
        {
            if (!Variables.contains(parameter))
                continue;

            commands.Append(
                "$data modify storage {0} stack[0].var.{1} set value $({1})",
                Location,
                parameter);
        }
    }

    for (auto &instruction: Instructions)
        instruction->Generate(commands, require_stack);
}

bool mcc::Block::RequireStack() const
{
    if (Parent && Parent->RequireStack())
        return true;
    if (StackIndex)
        return true;
    if (!Variables.empty())
        return true;
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

bool mcc::Block::MayThrow() const
{
    Assert(!Parent, "top level block required");
    if (const auto terminator = GetTerminator())
        return !!std::dynamic_pointer_cast<ThrowInstruction>(terminator);
    for (auto &child: Children)
        if (const auto terminator = child->GetTerminator();
            terminator && !!std::dynamic_pointer_cast<ThrowInstruction>(terminator))
            return true;
    return false;
}
