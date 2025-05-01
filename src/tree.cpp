#include <format>
#include <iostream>
#include <set>
#include <utility>
#include <mcc/context.hpp>
#include <mcc/format.hpp>
#include <mcc/tree.hpp>

static constexpr unsigned spaces = 2;
static unsigned indentation = 0;

static unsigned global_objective_id = 0;

static std::string get_indentation()
{
    std::string s;
    for (unsigned i = 0; i < indentation; ++i)
        s += ' ';
    return s;
}

std::ostream &mcc::ResourceLocation::Print(std::ostream &stream) const
{
    return stream << Namespace << ':' << Path;
}

std::string mcc::ResourceLocation::Gen(const ResourceLocation &location) const
{
    if (Namespace.empty())
        return location.Namespace + ':' + Path;
    return Namespace + ':' + Path;
}

mcc::StringNode::StringNode(std::string value)
    : Value(std::move(value))
{
}

std::ostream &mcc::StringNode::Print(std::ostream &stream) const
{
    return stream << Value;
}

bool mcc::StringNode::IsText() const
{
    return true;
}

bool mcc::StringNode::IsConstant() const
{
    return true;
}

std::string mcc::StringNode::Gen(const ResourceLocation &location, std::vector<std::string> &commands) const
{
    return '"' + Value + '"';
}

mcc::ExpressionNode::ExpressionNode(ExpressionPtr expression)
    : Expression(std::move(expression))
{
}

std::ostream &mcc::ExpressionNode::Print(std::ostream &stream) const
{
    return Expression->Print(stream << "${") << '}';
}

bool mcc::ExpressionNode::IsText() const
{
    return false;
}

bool mcc::ExpressionNode::IsConstant() const
{
    return Expression->IsConstant();
}

std::string mcc::ExpressionNode::Gen(const ResourceLocation &location, std::vector<std::string> &commands) const
{
    return Expression->Gen(location, commands, false);
}

mcc::NamespaceStatement::NamespaceStatement(std::string namespace_)
    : Namespace(std::move(namespace_))
{
}

std::ostream &mcc::NamespaceStatement::Print(std::ostream &stream) const
{
    return stream << "namespace " << Namespace;
}

void mcc::NamespaceStatement::Gen(Context &context) const
{
    context.Namespace = Namespace;
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

    // setup function stack frame
    commands_.emplace_back(std::format("data modify storage {0} stack prepend value {{}}", m_location));

    for (auto &parameter: Parameters)
    {
        // set variable with parameter value in stack frame
        commands_.emplace_back(
            std::format(
                "$data modify storage {0} stack[0].var.{1} set value $({1})",
                m_location,
                parameter));
    }

    for (auto &expression: Expressions)
    {
        // generate expression code
        (void) expression->Gen(location_, commands_, false);
    }

    // clean up stack frame
    commands_.emplace_back(std::format("data remove storage {0} stack[0]", m_location));

    std::cout << std::format("location: {}, commands: {}", location_, commands_) << std::endl;
}

mcc::BoolExpression::BoolExpression(const bool value)
    : Value(value)
{
}

std::ostream &mcc::BoolExpression::Print(std::ostream &stream) const
{
    return stream << (Value ? "true" : "false");
}

bool mcc::BoolExpression::IsConstant() const
{
    return true;
}

std::string mcc::BoolExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    return Value ? "true" : "false";
}

mcc::IntegerExpression::IntegerExpression(const int64_t value)
    : Value(value)
{
}

std::ostream &mcc::IntegerExpression::Print(std::ostream &stream) const
{
    return stream << Value;
}

bool mcc::IntegerExpression::IsConstant() const
{
    return true;
}

std::string mcc::IntegerExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    return std::to_string(Value);
}

mcc::FloatExpression::FloatExpression(const double value)
    : Value(value)
{
}

std::ostream &mcc::FloatExpression::Print(std::ostream &stream) const
{
    return stream << Value;
}

bool mcc::FloatExpression::IsConstant() const
{
    return true;
}

std::string mcc::FloatExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    return std::to_string(Value);
}

mcc::StringExpression::StringExpression(std::string value)
    : Value(std::move(value))
{
}

std::ostream &mcc::StringExpression::Print(std::ostream &stream) const
{
    return stream << '"' << Value << '"';
}

bool mcc::StringExpression::IsConstant() const
{
    return true;
}

std::string mcc::StringExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    return '"' + Value + '"';
}

mcc::ResourceExpression::ResourceExpression(std::string ns, std::string id)
    : Location(std::move(ns), std::move(id))
{
}

std::ostream &mcc::ResourceExpression::Print(std::ostream &stream) const
{
    return Location.Print(stream);
}

bool mcc::ResourceExpression::IsConstant() const
{
    return true;
}

std::string mcc::ResourceExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    return Location.Gen(location);
}

mcc::TargetExpression::TargetExpression(std::string specifier)
    : Specifier(std::move(specifier))
{
}

std::ostream &mcc::TargetExpression::Print(std::ostream &stream) const
{
    return stream << '@' << Specifier;
}

bool mcc::TargetExpression::IsConstant() const
{
    return true;
}

std::string mcc::TargetExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    return '@' + Specifier;
}

mcc::ArrayExpression::ArrayExpression(std::vector<ExpressionPtr> elements)
    : Elements(std::move(elements))
{
}

std::ostream &mcc::ArrayExpression::Print(std::ostream &stream) const
{
    stream << "[ ";
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Elements[i]->Print(stream);
    }
    return stream << " ]";
}

bool mcc::ArrayExpression::IsConstant() const
{
    return true;
}

std::string mcc::ArrayExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    std::string value;
    value += "[ ";
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        if (i > 0)
            value += ", ";
        value += Elements[i]->Gen(location, commands, false);
    }
    return value += " ]";
}

mcc::ObjectExpression::ObjectExpression(std::map<std::string, ExpressionPtr> elements)
    : Elements(std::move(elements))
{
}

std::ostream &mcc::ObjectExpression::Print(std::ostream &stream) const
{
    stream << "{ ";
    auto first = true;
    for (auto &[key_, value_]: Elements)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        value_->Print(stream << key_ << ": ");
    }
    return stream << " }";
}

bool mcc::ObjectExpression::IsConstant() const
{
    return true;
}

std::string mcc::ObjectExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    std::string value;
    value += "{ ";
    auto first = true;
    for (auto &[key_, value_]: Elements)
    {
        if (first)
            first = false;
        else
            value += ", ";
        value += key_;
        value += ": ";
        value += value_->Gen(location, commands, false);
    }
    return value += " }";
}

mcc::RelativeOffsetExpression::RelativeOffsetExpression(const double offset)
    : Offset(offset)
{
}

std::ostream &mcc::RelativeOffsetExpression::Print(std::ostream &stream) const
{
    if (Offset == 0.0)
        return stream << '~';
    return stream << '~' << Offset;
}

bool mcc::RelativeOffsetExpression::IsConstant() const
{
    return true;
}

std::string mcc::RelativeOffsetExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    return '~' + std::to_string(Offset);
}

mcc::LocalOffsetExpression::LocalOffsetExpression(const double offset)
    : Offset(offset)
{
}

std::ostream &mcc::LocalOffsetExpression::Print(std::ostream &stream) const
{
    if (Offset == 0.0)
        return stream << '^';
    return stream << '^' << Offset;
}

bool mcc::LocalOffsetExpression::IsConstant() const
{
    return true;
}

std::string mcc::LocalOffsetExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    return '^' + std::to_string(Offset);
}

mcc::BinaryExpression::BinaryExpression(std::string operator_, ExpressionPtr left, ExpressionPtr right)
    : Operator(std::move(operator_)),
      Left(std::move(left)),
      Right(std::move(right))
{
}

std::ostream &mcc::BinaryExpression::Print(std::ostream &stream) const
{
    return Right->Print(Left->Print(stream) << ' ' << Operator << ' ');
}

bool mcc::BinaryExpression::IsConstant() const
{
    return false;
}

std::string mcc::BinaryExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    const bool inlined) const
{
    static const std::set<std::string> comparator_set
    {
        "==",
        "!=",
        "<=",
        ">=",
        "<",
        ">",
    };

    static const std::map<std::string, std::string> operator_map
    {
        {"+", "+="},
        {"-", "-="},
        {"*", "*="},
        {"/", "/="},
        {"%", "%="},
    };

    auto left = Left->Gen(location, commands, false);
    auto right = Right->Gen(location, commands, false);

    if (Operator == "=")
    {
        if (Left->IsConstant())
        {
            throw std::runtime_error("cannot store a value to a constant");
        }

        std::string command;
        if (Right->IsConstant())
        {
            command = std::format(
                "data modify storage {0} {1} set value {2}",
                location,
                left,
                right);
        }
        else
        {
            command = std::format(
                "data modify storage {0} {1} set from storage {0} {2}",
                location,
                left,
                right);
        }

        commands.emplace_back(command);
        return inlined ? std::format("data get storage {0} {1}", location, left) : left;
    }

    auto objective_id = "tmp" + std::to_string(global_objective_id++);

    // create temporary scoreboard
    commands.emplace_back(std::format("scoreboard objectives add {0} dummy", objective_id));

    // copy left into scoreboard
    if (Left->IsConstant())
    {
        commands.emplace_back(std::format("scoreboard players set %a {0} {1}", objective_id, left));
    }
    else
    {
        commands.emplace_back(
            std::format(
                "execute store result score %a {0} run data get storage {1} {2}",
                objective_id,
                location,
                left));
    }

    // copy right into scoreboard
    if (Right->IsConstant())
    {
        commands.emplace_back(std::format("scoreboard players set %b {0} {1}", objective_id, right));
    }
    else
    {
        commands.emplace_back(
            std::format(
                "execute store result score %b {0} run data get storage {1} {2}",
                objective_id,
                location,
                right));
    }

    if (comparator_set.contains(Operator))
    {
        commands.emplace_back(std::format("scoreboard players set %c {0} 0", objective_id));

        if (Operator == "==" || Operator == "!=")
        {
            commands.emplace_back(
                std::format(
                    "execute {0} score %a {1} = %b {1} run scoreboard players set %c {1} 1",
                    Operator == "!=" ? "unless" : "if",
                    objective_id));
        }
        else
        {
            commands.emplace_back(
                std::format(
                    "execute if score %a {0} {1} %b {0} run scoreboard players set %c {0} 1",
                    objective_id,
                    Operator));
        }

        // copy result into storage
        commands.emplace_back(
            std::format(
                "execute store result storage {0} stack[0].tmp byte 1 run scoreboard players get %c {1}",
                location,
                objective_id));
    }
    else
    {
        auto operator_ = operator_map.at(Operator);
        commands.emplace_back(
            std::format(
                "scoreboard players operation %a {0} {1} %b {0}",
                objective_id,
                operator_));

        // copy result into storage
        commands.emplace_back(
            std::format(
                "execute store result storage {0} stack[0].tmp double 1 run scoreboard players get %a {1}",
                location,
                objective_id));
    }

    // delete temporary scoreboard
    commands.emplace_back(std::format("scoreboard objectives remove {0}", objective_id));

    return inlined ? std::format("data get storage {0} stack[0].tmp", location) : "stack[0].tmp";
}

mcc::CallExpression::CallExpression(std::string callee, std::vector<ExpressionPtr> arguments)
    : Callee(std::move(callee)),
      Arguments(std::move(arguments))
{
}

std::ostream &mcc::CallExpression::Print(std::ostream &stream) const
{
    stream << Callee << '(';
    for (unsigned i = 0; i < Arguments.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Arguments[i]->Print(stream);
    }
    return stream << ')';
}

bool mcc::CallExpression::IsConstant() const
{
    return false;
}

std::string mcc::CallExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    const bool inlined) const
{
    std::string args;
    for (auto &argument: Arguments)
    {
        args += ' ' + argument->Gen(location, commands, true);
    }

    auto command = std::format("{0}{1}", Callee, args);
    if (!inlined)
    {
        commands.emplace_back(command);
    }
    return command;
}

mcc::FormatExpression::FormatExpression(std::vector<FormatNodePtr> nodes)
    : Nodes(std::move(nodes))
{
}

std::ostream &mcc::FormatExpression::Print(std::ostream &stream) const
{
    stream << '`';
    for (auto &node: Nodes)
        node->Print(stream);
    return stream << '`';
}

bool mcc::FormatExpression::IsConstant() const
{
    return true;
}

std::string mcc::FormatExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    bool inlined) const
{
    auto objective_id = "tmp" + std::to_string(global_objective_id++);

    // create temporary scoreboard
    commands.emplace_back(std::format("scoreboard objectives add {0} dummy", objective_id));

    std::vector<std::string> args;
    for (auto &node: Nodes)
    {
        auto value = node->Gen(location, commands);

        if (node->IsText())
        {
            args.emplace_back(value);
            continue;
        }

        auto player_id = args.size();

        if (node->IsConstant())
        {
            commands.emplace_back(
                std::format(
                    "scoreboard players set %{0} {1} {2}",
                    player_id,
                    objective_id,
                    value));
        }
        else
        {
            commands.emplace_back(
                std::format(
                    "execute store result score %{0} {1} run data get storage {2} {3}",
                    player_id,
                    objective_id,
                    location,
                    value));
        }

        args.emplace_back(
            std::format(
                "{{ score: {{ name: \"%{0}\", objective: \"{1}\" }} }}",
                player_id,
                objective_id));
    }

    std::string value = "[ ";
    for (unsigned i = 0; i < args.size(); ++i)
    {
        if (i > 0)
            value += ", ";
        value += args[i];
    }
    value += " ]";

    return value;
}

mcc::IfExpression::IfExpression(ExpressionPtr condition, ExpressionPtr then, ExpressionPtr else_)
    : Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
}

std::ostream &mcc::IfExpression::Print(std::ostream &stream) const
{
    return Else->Print(Then->Print(Condition->Print(stream << "if (") << ") ") << " else ");
}

bool mcc::IfExpression::IsConstant() const
{
    return false;
}

std::string mcc::IfExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    const bool inlined) const
{
    auto condition = Condition->Gen(location, commands, false);

    commands.emplace_back(
        std::format(
            "execute if data storage {0} {1} run execute store result storage {0} stack[0].tmp double 1 run {2}",
            location,
            condition,
            Then->Gen(location, commands, true)));
    commands.emplace_back(
        std::format(
            "execute unless data storage {0} {1} run execute store result storage {0} stack[0].tmp double 1 run {2}",
            location,
            condition,
            Else->Gen(location, commands, true)));

    return inlined ? std::format("data get storage {0} stack[0].tmp", location) : "stack[0].tmp";
}

mcc::ReturnExpression::ReturnExpression(ExpressionPtr value)
    : Value(std::move(value))
{
}

std::ostream &mcc::ReturnExpression::Print(std::ostream &stream) const
{
    return Value->Print(stream << "return ");
}

bool mcc::ReturnExpression::IsConstant() const
{
    return false;
}

std::string mcc::ReturnExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    const bool inlined) const
{
    auto value = std::format("return run {}", Value->Gen(location, commands, true));
    if (!inlined)
    {
        commands.emplace_back(value);
    }
    return value;
}

mcc::SymbolExpression::SymbolExpression(std::string id)
    : ID(std::move(id))
{
}

std::ostream &mcc::SymbolExpression::Print(std::ostream &stream) const
{
    return stream << '%' << ID;
}

bool mcc::SymbolExpression::IsConstant() const
{
    return false;
}

std::string mcc::SymbolExpression::Gen(
    const ResourceLocation &location,
    std::vector<std::string> &commands,
    const bool inlined) const
{
    if (inlined)
        return std::format("data get storage {} stack[0].var.{}", location, ID);
    return std::format("stack[0].var.{}", ID);
}
