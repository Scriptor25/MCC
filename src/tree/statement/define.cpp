#include <iostream>
#include <mcc/builder.hpp>
#include <mcc/context.hpp>
#include <mcc/tree.hpp>

static constexpr unsigned spaces = 2;
static unsigned indentation = 0;

static std::string get_indentation()
{
    std::string s;
    for (unsigned i = 0; i < indentation; ++i)
        s += ' ';
    return s;
}

mcc::DefineStatement::DefineStatement(
    SourceLocation where,
    ResourceLocation location,
    std::vector<std::string> parameters,
    std::vector<ResourceLocation> tags,
    std::vector<ExpressionPtr> expressions)
    : Statement(std::move(where)),
      Location(std::move(location)),
      Parameters(std::move(parameters)),
      Tags(std::move(tags)),
      Expressions(std::move(expressions))
{
}

std::ostream &mcc::DefineStatement::Print(std::ostream &stream) const
{
    Location.Print(stream << "define ") << '(';
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        stream << Parameters[i];
    }
    stream << ") ";
    for (unsigned i = 0; i < Tags.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Tags[i].Print(stream << '#');
    }
    if (!Tags.empty())
        stream << ' ';
    stream << '{' << std::endl;
    indentation += spaces;
    auto is = get_indentation();
    for (auto &statement: Expressions)
        statement->Print(stream << is) << ';' << std::endl;
    indentation -= spaces;
    is = get_indentation();
    return stream << is << '}';
}

void mcc::DefineStatement::Generate(Context &context) const
{
    auto mut_location = Location;
    if (mut_location.Namespace.empty())
        mut_location.Namespace = context.Namespace;

    std::cerr << '[' << mut_location.Namespace << ':' << mut_location.Path << ']' << std::endl;

    for (auto &tag: Tags)
    {
        auto mut_tag = tag;
        if (mut_tag.Namespace.empty())
            mut_tag.Namespace = context.Namespace;

        auto &[replace_, values_] = context.Pkg.Tags[mut_tag];
        values_.emplace_back(mut_location);
    }

    auto &[location_, commands_] = context.Pkg.Functions[mut_location];
    location_ = mut_location;
    commands_.clear();

    Builder builder(context, location_);
    for (auto &expression: Expressions)
        (void) expression->Generate(builder, false);

    const auto require_stack = builder.RequireStack();
    const auto require_cleanup = builder.RequireCleanup();

    if (require_stack)
    {
        commands_.emplace_back(std::format("data modify storage {} stack prepend value {{}}", location_));

        for (auto &parameter: Parameters)
            commands_.emplace_back(
                std::format(
                    "$data modify storage {0} stack[0].var.{1} set value $({1})",
                    location_,
                    parameter));
    }

    builder.Generate(commands_, require_stack);

    if (require_stack && require_cleanup)
        commands_.emplace_back(std::format("data remove storage {} stack[0]", location_));
}
