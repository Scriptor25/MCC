#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::BlockPtr mcc::Block::CreateTopLevel(
    const SourceLocation &where,
    TypePtr type,
    const ResourceLocation &location)
{
    return std::make_shared<Block>(where, type, location);
}

mcc::BlockPtr mcc::Block::Create(
    const SourceLocation &where,
    const BlockPtr &parent,
    const ResourceLocation &location)
{
    auto block = std::make_shared<Block>(where, parent->Type, location);
    block->Parent = parent;
    parent->Children.push_back(block);
    return block;
}

mcc::Block::Block(const SourceLocation &where, TypePtr type, const ResourceLocation &location)
    : Value(where, type),
      Location(location)
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

        for (auto &[name_, value_]: Parameters)
        {
            if (!value_->UseCount)
                continue;

            if (value_->Type == TypeContext::GetString())
                commands.Append(
                    "$data modify storage {0} stack[0].var.{1} set value \"$({1})\"",
                    Location,
                    name_);
            else
                commands.Append(
                    "$data modify storage {0} stack[0].var.{1} set value $({1})",
                    Location,
                    name_);
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
    return std::ranges::any_of(
               Parameters,
               [](auto &parameter)
               {
                   return parameter.second->UseCount;
               })
           || std::ranges::any_of(
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
    Assert(!Parent, Where, "top level block required");
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
            if (parameters[i].second->Type == TypeContext::GetString())
                arguments += std::format("{0}:\"$({0})\"", parameters[i].first);
            else
                arguments += std::format("{0}:$({0})", parameters[i].first);
        }
        arguments += '}';
    }
}
