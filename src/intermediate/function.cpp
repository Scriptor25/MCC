#include <utility>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::FunctionPtr mcc::Function::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ParameterList &parameters,
    const TypePtr &result_type,
    const bool throws)
{
    std::vector<TypePtr> parameter_types;
    for (const auto &[name_, type_] : parameters)
        parameter_types.emplace_back(type_);

    auto type = context.GetFunction(parameter_types, result_type, throws);
    return std::make_shared<Function>(where, type, location, parameters, result_type, throws);
}

mcc::Function::Function(
    const SourceLocation &where,
    const TypePtr &type,
    ResourceLocation location,
    const ParameterList &parameters,
    TypePtr result_type,
    const bool throws)
    : Value(where, type, false),
      Location(std::move(location)),
      Parameters(parameters),
      ResultType(std::move(result_type)),
      Throws(throws)
{
}

void mcc::Function::Generate(CommandVector &commands, const bool stack) const
{
    Error(Where, "mcc::Function::Generate");
}

bool mcc::Function::RequireStack() const
{
    if (StackIndex)
        return true;

    for (auto &block : Blocks)
        if (block->RequireStack())
            return true;

    return false;
}

mcc::Result mcc::Function::GenerateResult() const
{
    return {
        .Type = ResultType_Value,
        .Value = '"' + Location.String() + '"',
        .NotNull = true,
    };
}

bool mcc::Function::RemoveUnreferencedBlocks()
{
    std::vector<BlockPtr> blocks;
    for (unsigned i = 1; i < Blocks.size(); ++i)
        if (Blocks[i]->Predecessors.empty())
            blocks.emplace_back(Blocks[i]);

    for (auto block : blocks)
        Erase(block);

    return !blocks.empty();
}

bool mcc::Function::MergeConsecutiveBlocks()
{
    std::vector<BlockPtr> blocks;
    for (auto &block : Blocks)
        if (block->Successors.size() == 1)
            if (auto successor = *block->Successors.begin(); successor->Predecessors.size() == 1)
            {
                block->Instructions.pop_back();
                successor->Predecessors.erase(block);

                for (auto &instruction : successor->Instructions)
                    block->Instructions.emplace_back(instruction);

                blocks.emplace_back(successor);
            }

    for (auto block : blocks)
        Erase(block);

    return !blocks.empty();
}

void mcc::Function::OptimizeBlocks()
{
    bool recycle;
    do
    {
        recycle = false;
        recycle |= RemoveUnreferencedBlocks();
        recycle |= MergeConsecutiveBlocks();
    }
    while (recycle);
}

void mcc::Function::GenerateFunction(Package &package) const
{
    const auto require_stack = RequireStack();

    for (unsigned i = 0; i < Blocks.size(); ++i)
    {
        auto [namespace_, path_] = Location;
        if (i)
            path_ += '/' + std::to_string(i);

        CommandVector commands(package.Functions[namespace_][path_]);

        if (i == 0 && require_stack)
        {
            commands.Append("data modify storage {} stack prepend value {{}}", Location);

            for (unsigned s = 0; s < StackIndex; ++s)
                commands.Append("data modify storage {} stack[0].val append value 0", Location);
        }

        Blocks[i]->Generate(commands, require_stack);
    }
}

void mcc::Function::ForwardArguments(std::string &prefix, std::string &arguments) const
{
    if (Parameters.empty())
        return;

    prefix = "$";
    arguments += " {";
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        if (i)
            arguments += ',';
        if (Parameters[i].Type->IsString())
            arguments += std::format("\"{0}\":\"$({0})\"", Parameters[i].Name);
        else
            arguments += std::format("\"{0}\":$({0})", Parameters[i].Name);
    }
    arguments += '}';
}

mcc::ResourceLocation mcc::Function::GetLocation(const BlockPtr &target_block) const
{
    for (unsigned i = 0; i < Blocks.size(); ++i)
        if (Blocks[i] == target_block)
            return { Location.Namespace, Location.Path + '/' + std::to_string(i) };

    Error(Where, "cannot get location for non-child block");
}

mcc::BlockPtr mcc::Function::Erase(const BlockPtr &target_block)
{
    for (auto i = Blocks.begin(); i != Blocks.end(); ++i)
        if (*i == target_block)
        {
            Assert(target_block->Predecessors.empty(), target_block->Where, "cannot erase referenced block");
            Blocks.erase(i);
            for (auto &successor : target_block->Successors)
                successor->Predecessors.erase(target_block);
            return target_block;
        }

    return nullptr;
}
