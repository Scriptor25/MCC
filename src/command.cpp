#include <fstream>
#include <utility>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>
#include <nlohmann/json.hpp>

using namespace std::string_view_literals;

void mcc::from_json(const nlohmann::json &json, CommandDefinition &definition)
{
    definition.Base = json["base"];
    definition.As = json.value("as", definition.Base);
    definition.Rules = json.value("rules", std::map<std::string, CommandNodePtr>());
    definition.Nodes = json["nodes"];
}

void mcc::from_json(const nlohmann::json &json, CommandNodePtr &node_ptr)
{
    if (json.is_string())
    {
        std::string value = json;
        node_ptr = std::make_unique<FixedNode>(value, value, false);
        return;
    }

    if (json.is_array())
    {
        std::vector<CommandNodePtr> allowed = json;
        node_ptr = std::make_unique<EnumNode>(std::move(allowed), false);
        return;
    }

    const CommandNodeTypeE type = json["type"];
    const bool optional = json.value("optional", false);

    switch (type)
    {
        case CommandNodeType_Fixed:
        {
            std::string value = json["value"];
            std::string as = json.value("as", value);
            node_ptr = std::make_unique<FixedNode>(value, as, optional);
            break;
        }

        case CommandNodeType_Enum:
        {
            std::vector<CommandNodePtr> allowed = json["allowed"];
            node_ptr = std::make_unique<EnumNode>(std::move(allowed), optional);
            break;
        }

        case CommandNodeType_Switch:
        {
            std::map<std::string, std::vector<CommandNodePtr>> cases = json["cases"];
            std::vector<CommandNodePtr> default_ = json.value("default", std::vector<CommandNodePtr>());
            node_ptr = std::make_unique<SwitchNode>(std::move(cases), std::move(default_), optional);
            break;
        }

        case CommandNodeType_Value:
        {
            std::string label = json.value("label", "");
            std::string is = json.value("is", "");
            node_ptr = std::make_unique<ValueNode>(label, is, optional);
            break;
        }

        case CommandNodeType_Rule:
        {
            std::string id = json["id"];
            node_ptr = std::make_unique<RuleNode>(id, optional);
            break;
        }

        case CommandNodeType_Greedy:
        {
            std::string label = json.value("label", "");
            node_ptr = std::make_unique<GreedyNode>(label, optional);
            break;
        }
    }
}

void mcc::from_json(const nlohmann::json &json, CommandNodeTypeE &type)
{
    static const std::map<std::string_view, CommandNodeTypeE> map
    {
        {"fixed"sv, CommandNodeType_Fixed},
        {"enum"sv, CommandNodeType_Enum},
        {"switch"sv, CommandNodeType_Switch},
        {"value"sv, CommandNodeType_Value},
        {"rule"sv, CommandNodeType_Rule},
        {"greedy"sv, CommandNodeType_Greedy},
    };

    const std::string type_str = json;
    type = map.at(type_str);
}

void mcc::ReadDefinition(CommandDefinition &definition, const std::filesystem::path &filepath)
{
    std::ifstream stream(filepath);
    if (!stream)
        return;

    nlohmann::json json;
    stream >> json;
    definition = json;
}

mcc::CommandNode::CommandNode(const CommandNodeTypeE type, const bool optional)
    : Type(type),
      Optional(optional)
{
}

std::ostream &mcc::CommandNode::Print(std::ostream &stream, const std::string &space) const
{
    stream << "type: " << Type << std::endl;
    stream << space << "optional: " << (Optional ? "true" : "false") << std::endl;
    return stream;
}

std::vector<mcc::ExpressionPtr> mcc::CommandNode::Parse(Parser &parser, const CommandDefinition &definition) const
{
    std::vector<ExpressionPtr> expressions;
    if (auto expression = ParseOne(parser, definition))
        expressions.emplace_back(std::move(expression));
    return expressions;
}

mcc::ExpressionPtr mcc::CommandNode::ParseOne(Parser &parser, const CommandDefinition &definition) const
{
    throw std::runtime_error("TODO");
}

mcc::FixedNode::FixedNode(std::string value, std::string as, const bool optional)
    : CommandNode(CommandNodeType_Fixed, optional),
      Value(std::move(value)),
      As(std::move(as))
{
}

std::ostream &mcc::FixedNode::Print(std::ostream &stream, const std::string &space) const
{
    CommandNode::Print(stream, space);
    stream << space << "value: " << Value << std::endl;
    stream << space << "as: " << As << std::endl;
    return stream;
}

bool mcc::FixedNode::IsValidNext(Parser &parser, const CommandDefinition &definition) const
{
    return parser.At(TokenType_Symbol, Value);
}

mcc::ExpressionPtr mcc::FixedNode::ParseOne(Parser &parser, const CommandDefinition &definition) const
{
    if (Optional && !parser.At(TokenType_Symbol, Value))
        return {};

    return parser.ParseSymbolExpression(true);
}

mcc::EnumNode::EnumNode(std::vector<CommandNodePtr> allowed, const bool optional)
    : CommandNode(CommandNodeType_Enum, optional),
      Allowed(std::move(allowed))
{
}

std::ostream &mcc::EnumNode::Print(std::ostream &stream, const std::string &space) const
{
    CommandNode::Print(stream, space);
    stream << space << "allowed:" << std::endl;
    for (auto &allowed: Allowed)
        stream << space << " - " << allowed << std::endl;
    return stream;
}

bool mcc::EnumNode::IsValidNext(Parser &parser, const CommandDefinition &definition) const
{
    return std::ranges::any_of(
        Allowed,
        [&parser, &definition](const CommandNodePtr &allowed)
        {
            return allowed->IsValidNext(parser, definition);
        });
}

std::vector<mcc::ExpressionPtr> mcc::EnumNode::Parse(Parser &parser, const CommandDefinition &definition) const
{
    for (auto &allowed: Allowed)
        if (allowed->IsValidNext(parser, definition))
            return allowed->Parse(parser, definition);

    Assert(Optional, "enum node is not optional, but none of its values match");
    return {};
}

mcc::SwitchNode::SwitchNode(
    std::map<std::string, std::vector<CommandNodePtr>> cases,
    std::vector<CommandNodePtr> default_,
    const bool optional)
    : CommandNode(CommandNodeType_Switch, optional),
      Cases(std::move(cases)),
      Default(std::move(default_))
{
}

std::ostream &mcc::SwitchNode::Print(std::ostream &stream, const std::string &space) const
{
    CommandNode::Print(stream, space);
    stream << space << "cases:" << std::endl;
    for (auto &[case_, nodes_]: Cases)
    {
        stream << space << "   " << case_ << ':' << std::endl;
        for (auto &node: nodes_)
            node->Print(stream << space << "    - ", space + "      ");
    }
    stream << space << "default:" << std::endl;
    for (auto &node: Default)
        node->Print(stream << space << " - ", space + "   ");
    return stream;
}

bool mcc::SwitchNode::IsValidNext(Parser &parser, const CommandDefinition &definition) const
{
    if (const auto key = parser.m_Token.Value;
        Cases.contains(key))
        return true;

    return !Default.empty();
}

std::vector<mcc::ExpressionPtr> mcc::SwitchNode::Parse(Parser &parser, const CommandDefinition &definition) const
{
    auto key = parser.m_Token.Value;
    Assert(!Default.empty() || Cases.contains(key), "no valid case for key {}", key);

    std::vector<ExpressionPtr> expressions;

    if (Cases.contains(key))
    {
        expressions.emplace_back(parser.ParseSymbolExpression(true));
        for (auto &case_ = Cases.at(key);
             auto &node: case_)
        {
            auto expression = node->Parse(parser, definition);
            if (expression.empty())
                break;

            for (auto &e: expression)
                expressions.emplace_back(std::move(e));
        }
    }
    else
        for (auto &node: Default)
        {
            auto expression = node->Parse(parser, definition);
            if (expression.empty())
                break;

            for (auto &e: expression)
                expressions.emplace_back(std::move(e));
        }

    return expressions;
}

mcc::ValueNode::ValueNode(std::string label, std::string is, const bool optional)
    : CommandNode(CommandNodeType_Value, optional),
      Label(std::move(label)),
      Is(std::move(is))
{
}

std::ostream &mcc::ValueNode::Print(std::ostream &stream, const std::string &space) const
{
    CommandNode::Print(stream, space);
    stream << space << "label: " << Label << std::endl;
    stream << space << "is: " << Is << std::endl;
    return stream;
}

bool mcc::ValueNode::IsValidNext(Parser &parser, const CommandDefinition &definition) const
{
    throw std::runtime_error("TODO");
}

mcc::ExpressionPtr mcc::ValueNode::ParseOne(Parser &parser, const CommandDefinition &definition) const
{
    if (Is.empty())
        return parser.ParseExpression();

    if (Is == "integer")
        return parser.ParseIntegerExpression();

    if (Is == "float")
        return parser.ParseFloatExpression();

    if (Is == "range")
        return parser.ParseRangeExpression();

    if (Is == "entity")
    {
        if (parser.At(TokenType_Target))
            return parser.ParseTargetExpression(false);
        if (parser.At(TokenType_TargetAttributes))
            return parser.ParseTargetExpression(true);
        return parser.ParseSymbolExpression(true);
    }

    if (Is == "component")
    {
        if (parser.At(TokenType_Other, "{"))
            return parser.ParseObjectExpression();
        if (parser.At(TokenType_Other, "["))
            return parser.ParseArrayExpression();
        if (parser.At(TokenType_Integer))
            return parser.ParseIntegerExpression();
        if (parser.At(TokenType_Float))
            return parser.ParseFloatExpression();
        return parser.ParseStringExpression();
    }

    if (Is == "resource")
        return parser.ParseResourceExpression();

    if (Is == "resource_nbt")
    {
        auto where = parser.m_Token.Where;
        auto location = parser.ParseResourceLocation();
        ExpressionPtr nbt;
        if (parser.At(TokenType_Other, "{"))
            nbt = parser.ParseObjectExpression();
        return std::make_unique<ResourceExpression>(std::move(where), std::move(location), std::move(nbt));
    }

    throw std::runtime_error("TODO");
}

mcc::RuleNode::RuleNode(std::string id, const bool optional)
    : CommandNode(CommandNodeType_Rule, optional),
      Id(std::move(id))
{
}

std::ostream &mcc::RuleNode::Print(std::ostream &stream, const std::string &space) const
{
    CommandNode::Print(stream, space);
    stream << space << "id: " << Id << std::endl;
    return stream;
}

bool mcc::RuleNode::IsValidNext(Parser &parser, const CommandDefinition &definition) const
{
    auto &rules = definition.Rules;
    Assert(rules.contains(Id), "no rule {} in definition for {}", Id, definition.Base);

    return rules.at(Id)->IsValidNext(parser, definition);
}

std::vector<mcc::ExpressionPtr> mcc::RuleNode::Parse(Parser &parser, const CommandDefinition &definition) const
{
    auto &rules = definition.Rules;
    Assert(rules.contains(Id), "no rule {} in definition for {}", Id, definition.Base);

    return rules.at(Id)->Parse(parser, definition);
}

mcc::GreedyNode::GreedyNode(std::string label, const bool optional)
    : CommandNode(CommandNodeType_Greedy, optional),
      Label(std::move(label))
{
}

std::ostream &mcc::GreedyNode::Print(std::ostream &stream, const std::string &space) const
{
    CommandNode::Print(stream, space);
    stream << space << "label: " << Label << std::endl;
    return stream;
}

bool mcc::GreedyNode::IsValidNext(Parser &parser, const CommandDefinition &definition) const
{
    return true;
}

std::vector<mcc::ExpressionPtr> mcc::GreedyNode::Parse(Parser &parser, const CommandDefinition &definition) const
{
    std::vector<ExpressionPtr> expressions;
    while (!parser.At(TokenType_Other, ";"))
        expressions.emplace_back(parser.ParseExpression());
    return std::move(expressions);
}

std::ostream &mcc::operator<<(std::ostream &stream, const CommandDefinition &definition)
{
    stream << "base: " << definition.Base << std::endl;
    stream << "as: " << definition.As << std::endl;
    stream << "rules:" << std::endl;
    for (auto &[id_, rule_]: definition.Rules)
    {
        stream << "   " << id_ << ':' << std::endl;
        rule_->Print(stream << "      ", "      ");
    }
    stream << "nodes:" << std::endl;
    for (auto &node: definition.Nodes)
        node->Print(stream << " - ", "   ");
    return stream;
}

std::ostream &mcc::operator<<(std::ostream &stream, const CommandNodeTypeE &type)
{
    switch (type)
    {
        case CommandNodeType_Fixed:
            return stream << "fixed";

        case CommandNodeType_Enum:
            return stream << "enum";

        case CommandNodeType_Switch:
            return stream << "switch";

        case CommandNodeType_Value:
            return stream << "value";

        case CommandNodeType_Rule:
            return stream << "rule";

        case CommandNodeType_Greedy:
            return stream << "greedy";
    }

    return stream << "<undefined>";
}
