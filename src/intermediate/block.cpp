#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::BlockPtr mcc::Block::CreateTopLevel(ResourceLocation location, std::vector<std::string> parameters)
{
    return std::make_shared<Block>(location, std::move(parameters));
}

mcc::BlockPtr mcc::Block::Create(const BlockPtr &parent, ResourceLocation location)
{
    auto block = std::make_shared<Block>(location, std::vector<std::string>());
    block->Parent = parent;
    parent->Children.push_back(block);
    return std::move(block);
}

mcc::Block::Block(ResourceLocation location, std::vector<std::string> parameters)
    : Location(std::move(location)),
      Parameters(std::move(parameters))
{
}

void mcc::Block::Generate(CommandVector &commands) const
{
    if (!Parent)
    {
        commands.Append("data modify storage {} stack prepend value {{}}", Location);

        for (auto &parameter: Parameters)
            commands.Append(
                "$data modify storage {0} stack[0].var.{1} set value $({1})",
                Location,
                parameter);
    }

    for (auto &instruction: Instructions)
        instruction->Generate(commands);
}

mcc::CommandT mcc::Block::GenerateInline() const
{
    return std::format("function {}", Location);
}

mcc::InstructionPtr mcc::Block::GetTerminator() const
{
    if (Instructions.empty() || !Instructions.back()->IsTerminator())
        return nullptr;
    return Instructions.back();
}
