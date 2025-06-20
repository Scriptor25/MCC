#include <utility>
#include <mcc/block.hpp>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
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
    for (const auto &[_0, type_, _1] : parameters)
        parameter_types.emplace_back(type_);

    auto type = context.GetFunction(parameter_types, result_type, throws);
    return std::make_shared<Function>(where, type, location, parameters, result_type, throws);
}

mcc::Function::Function(
    const SourceLocation &where,
    const TypePtr &type,
    ResourceLocation location,
    ParameterList parameters,
    TypePtr result_type,
    const bool throws)
    : Value(where, type, FieldType_Value),
      Location(std::move(location)),
      Parameters(std::move(parameters)),
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

    return std::ranges::any_of(
        Blocks,
        [](const auto &block)
        {
            return block->RequireStack();
        });
}

mcc::Result mcc::Function::GenerateResult() const
{
    return {
        .Type = ResultType_Value,
        .Value = '"' + Location.String() + '"',
        .NotNull = true,
    };
}

mcc::Result mcc::Function::GenerateResultUnwrap() const
{
    return {
        .Type = ResultType_Value,
        .Value = Location.String(),
        .NotNull = true,
    };
}

bool mcc::Function::RemoveUnreferencedBlocks()
{
    std::set<BlockPtr> blocks;
    for (unsigned i = 1; i < Blocks.size(); ++i)
        if (Blocks[i]->Predecessors.empty())
            blocks.insert(Blocks[i]);

    for (const auto &block : blocks)
        Erase(block);

    return !blocks.empty();
}

bool mcc::Function::MergeConsecutiveBlocks()
{
    std::set<BlockPtr> blocks;
    for (const auto &block : Blocks)
    {
        if (blocks.contains(block))
            continue;

        if (block->Successors.size() == 1)
            if (auto successor = *block->Successors.begin(); successor->Predecessors.size() == 1)
            {
                Assert(!block->Instructions.empty(), "instructions must not be empty");

                block->Instructions.pop_back();

                for (auto &instruction : successor->Instructions)
                    block->Instructions.emplace_back(instruction);
                successor->Instructions.clear();

                block->Successors.erase(successor);
                for (auto &next : successor->Successors)
                {
                    next->Predecessors.erase(successor);
                    next->Predecessors.insert(block);
                    block->Successors.insert(next);
                }

                successor->Predecessors.clear();
                successor->Successors.clear();

                blocks.insert(successor);
            }
    }

    for (const auto &block : blocks)
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
            path_.emplace_back(std::to_string(i));

        CommandVector commands(package.Functions[namespace_][path_]);

        if (!i && require_stack)
        {
            commands.Append("data modify storage {} stack prepend value {{}}", Location);

            if (StackIndex)
            {
                std::string values;
                for (unsigned s = 0; s < StackIndex; ++s)
                {
                    if (s)
                        values += ',';
                    values += '0';
                }

                commands.Append("data modify storage {} stack[0].v set value [{}]", Location, values);
            }
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
        else if (Parameters[i].FieldType != FieldType_Value)
            arguments += std::format(
                "\"{0}_target\":\"$({0}_target)\",\"{0}_path\":\"$({0}_path)\"",
                Parameters[i].Name);
        else
            arguments += std::format("\"{0}\":$({0})", Parameters[i].Name);
    }
    arguments += '}';
}

mcc::ResourceLocation mcc::Function::GetLocation(const BlockPtr &target_block) const
{
    for (unsigned i = 0; i < Blocks.size(); ++i)
        if (Blocks[i] == target_block)
            return Location.Child(std::to_string(i));

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
            target_block->Successors.clear();
            target_block->Parent = nullptr;
            return target_block;
        }

    return nullptr;
}
