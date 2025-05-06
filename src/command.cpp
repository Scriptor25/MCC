#include <fstream>
#include <utility>
#include <mcc/command.hpp>
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
            node_ptr = std::make_unique<ValueNode>(label, optional);
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
            node_ptr = std::make_unique<GreedyNode>(optional);
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
        allowed->Print(stream << space << " - ", space + "   ");
    return stream;
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

mcc::ValueNode::ValueNode(std::string label, const bool optional)
    : CommandNode(CommandNodeType_Value, optional),
      Label(std::move(label))
{
}

std::ostream &mcc::ValueNode::Print(std::ostream &stream, const std::string &space) const
{
    CommandNode::Print(stream, space);
    stream << space << "label: " << Label << std::endl;
    return stream;
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

mcc::GreedyNode::GreedyNode(const bool optional)
    : CommandNode(CommandNodeType_Greedy, optional)
{
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
