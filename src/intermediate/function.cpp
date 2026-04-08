#include <mcc/block.hpp>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

#include <algorithm>

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
        parameter_types.push_back(type_);

    auto type  = context.GetFunction(parameter_types, result_type, throws);
    auto self  = std::make_shared<Function>(where, type, location, parameters, result_type, throws);
    self->Self = self;
    return self;
}

mcc::Function::Function(
        const SourceLocation &where,
        const TypePtr &type,
        ResourceLocation location,
        ParameterList parameters,
        TypePtr result_type,
        const bool throws)
    : Value(where,
            type,
            FieldType_Value),
      Location(std::move(location)),
      Parameters(std::move(parameters)),
      ResultType(std::move(result_type)),
      Throws(throws)
{
}

void mcc::Function::Generate(
        CommandVector &commands,
        const bool stack) const
{
    Error(Where, "mcc::Function::Generate");
}

bool mcc::Function::RequireStack() const
{
    if (StackIndex)
        return true;

    return std::ranges::any_of(Blocks, [](const auto &block) { return block->RequireStack(); });
}

mcc::Result mcc::Function::GenerateResult() const
{
    return {
        .Type    = ResultType_Value,
        .Value   = '"' + Location.String() + '"',
        .NotNull = true,
    };
}

mcc::Result mcc::Function::GenerateResultUnwrap() const
{
    return {
        .Type    = ResultType_Value,
        .Value   = Location.String(),
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
    for (auto &block : Blocks)
    {
        if (blocks.contains(block))
            continue;

        if (block->Successors.size() == 1)
            if (auto &successor = *block->Successors.begin(); successor->Predecessors.size() == 1)
            {
                Assert(!block->Instructions.empty(), "instructions must not be empty");

                block->Instructions.pop_back();

                for (auto &instruction : successor->Instructions)
                    block->Instructions.push_back(std::move(instruction));
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
            path_.push_back(std::to_string(i));

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

void mcc::Function::ForwardArguments(
        std::string &prefix,
        std::string &arguments) const
{
    if (Parameters.empty())
        return;

    prefix = "$";
    arguments += " {";
    for (auto i = Parameters.begin(); i != Parameters.end(); ++i)
    {
        if (i != Parameters.begin())
            arguments += ',';
        if (i->Type->IsString())
            arguments += std::format("\"{0}\":\"$({0})\"", i->Name);
        else if (i->FieldType != FieldType_Value)
            arguments += std::format("\"{0}_target\":\"$({0}_target)\",\"{0}_path\":\"$({0}_path)\"", i->Name);
        else
            arguments += std::format("\"{0}\":$({0})", i->Name);
    }
    arguments += '}';
}

mcc::ResourceLocation mcc::Function::GetLocation(const BlockPtr &target_block) const
{
    Assert(!!target_block, "target block must not be null");

    for (unsigned i = 0; i < Blocks.size(); ++i)
        if (Blocks[i] == target_block)
            return Location.Child(std::to_string(i));

    Error(target_block->Where, "target block is not owned by the function");
}

mcc::BlockPtr mcc::Function::Erase(const BlockPtr &target_block)
{
    Assert(!!target_block, "target block must not be null");
    Assert(target_block->Predecessors.empty(), target_block->Where, "target block must not have any predecessors");

    for (auto i = Blocks.begin(); i != Blocks.end(); ++i)
        if (*i == target_block)
        {
            Blocks.erase(i);

            for (auto &successor : target_block->Successors)
                successor->Predecessors.erase(target_block);
            target_block->Successors.clear();

            target_block->Parent = nullptr;
            return target_block;
        }

    Error(target_block->Where, "target block is not owned by the function");
}
