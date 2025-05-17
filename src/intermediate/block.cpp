#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::BlockPtr mcc::Block::CreateTopLevel(
    ResourceLocation location,
    ParameterList parameters)
{
    return std::make_shared<Block>(std::move(location), std::move(parameters));
}

mcc::BlockPtr mcc::Block::Create(const BlockPtr &parent, ResourceLocation location)
{
    auto block = std::make_shared<Block>(std::move(location), ParameterList());
    block->Parent = parent;
    parent->Children.push_back(block);
    return std::move(block);
}

mcc::Block::Block(ResourceLocation location, ParameterList parameters)
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
            if (!Variables.contains(parameter.Name))
                continue;

            if (parameter.Type == TypeID_String)
                commands.Append(
                    "$data modify storage {0} stack[0].var.{1} set value \"$({1})\"",
                    Location,
                    parameter.Name);
            else
                commands.Append(
                    "$data modify storage {0} stack[0].var.{1} set value $({1})",
                    Location,
                    parameter.Name);
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
    if (const auto terminator = GetTerminator();
        terminator && !!std::dynamic_pointer_cast<ThrowInstruction>(terminator))
        return true;
    for (auto &child: Children)
        if (const auto terminator = child->GetTerminator();
            terminator && !!std::dynamic_pointer_cast<ThrowInstruction>(terminator))
            return true;
    return false;
}

void mcc::Block::ForwardArguments(std::string &prefix, std::string &arguments) const
{
    if (auto &parameters = Parent ? Parent->Parameters : Parameters; !parameters.empty())
    {
        prefix = "$";
        arguments += " {";
        for (unsigned i = 0; i < parameters.size(); ++i)
        {
            if (i)
                arguments += ',';
            if (parameters[i].Type == TypeID_String)
                arguments += std::format("{0}:\"$({0})\"", parameters[i].Name);
            else
                arguments += std::format("{0}:$({0})", parameters[i].Name);
        }
        arguments += '}';
    }
}
