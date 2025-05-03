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
    ResourceLocation location,
    std::vector<std::string> parameters,
    std::vector<ResourceLocation> tags,
    std::vector<ExpressionPtr> expressions)
    : Location(std::move(location)),
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

void mcc::DefineStatement::Gen(Context &context) const
{
    auto m_location = Location;
    if (m_location.Namespace.empty())
        m_location.Namespace = context.Namespace;

    for (auto &tag: Tags)
    {
        auto m_tag = tag;
        if (m_tag.Namespace.empty())
            m_tag.Namespace = context.Namespace;

        auto &[replace_, values_] = context.Package.Tags[m_tag];
        values_.emplace_back(m_location);
    }

    auto &[location_, commands_] = context.Package.Functions[m_location];
    location_ = m_location;
    commands_.clear();

    Builder builder(context, location_);

    for (auto &expression: Expressions)
        (void) expression->Gen(builder, false);

    commands_.emplace_back(std::format("data modify storage {} stack prepend value {{}}", location_));

    for (auto &parameter: Parameters)
        commands_.emplace_back(
            std::format(
                "$data modify storage {0} stack[0].var.{1} set value $({1})",
                location_,
                parameter));

    builder.Gen(commands_);

    commands_.emplace_back(std::format("data remove storage {} stack[0]", location_));
}
