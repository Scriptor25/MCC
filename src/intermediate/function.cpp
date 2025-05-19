#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::FunctionPtr mcc::Function::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ParameterList &parameters,
    const TypePtr &result,
    const bool throws)
{
    return std::make_shared<Function>(where, context, location, parameters, result, throws);
}

mcc::Function::Function(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ParameterList &parameters,
    const TypePtr &result,
    const bool throws)
    : Value(where, context, context.GetVoid()),
      Location(location),
      Result(result),
      Throws(throws)
{
    for (const auto &[name_, type_]: parameters)
        Parameters.emplace_back(name_, NamedValue::Create(Where, Context, type_, Location, name_));
}

void mcc::Function::Generate(CommandVector &commands, const bool stack) const
{
    Error(Where, "mcc::Function::Generate");
}

bool mcc::Function::RequireStack() const
{
    if (StackIndex)
        return true;

    for (auto &parameter: Parameters | std::views::values)
        if (parameter->UseCount)
            return true;

    for (auto &block: Blocks)
        if (block->RequireStack())
            return true;

    return false;
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

            for (auto &[name_, value_]: Parameters)
            {
                if (!value_->UseCount)
                    continue;

                if (value_->Type == Context.GetString())
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

        Blocks.at(i)->Generate(commands, require_stack);
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
        if (Parameters[i].second->Type == Context.GetString())
            arguments += std::format("{0}:\"$({0})\"", Parameters[i].first);
        else
            arguments += std::format("{0}:$({0})", Parameters[i].first);
    }
    arguments += '}';
}

mcc::ResourceLocation mcc::Function::GetLocation(const BlockPtr &target_block) const
{
    for (unsigned i = 0; i < Blocks.size(); ++i)
        if (Blocks.at(i) == target_block)
            return {Location.Namespace, Location.Path + '/' + std::to_string(i)};
    Error("mcc::Function::GetLocation");
}

mcc::BlockPtr mcc::Function::Erase(const BlockPtr &target_block)
{
    for (auto i = Blocks.begin(); i != Blocks.end(); ++i)
        if (*i == target_block)
        {
            Blocks.erase(i);
            return target_block;
        }
    return nullptr;
}
