#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <mcc/common.hpp>
#include <nlohmann/json_fwd.hpp>

namespace mcc
{
    using CommandNodePtr = std::unique_ptr<struct CommandNode>;

    struct CommandDefinition
    {
        std::string Base;
        std::string As;
        std::map<std::string, CommandNodePtr> Rules;
        std::vector<CommandNodePtr> Nodes;
    };

    using DefinitionsT = std::map<std::string, CommandDefinition>;

    enum CommandNodeTypeE
    {
        CommandNodeType_Fixed,
        CommandNodeType_Enum,
        CommandNodeType_Switch,
        CommandNodeType_Value,
        CommandNodeType_Rule,
        CommandNodeType_Greedy,
    };

    void from_json(const nlohmann::json &json, CommandDefinition &definition);
    void from_json(const nlohmann::json &json, CommandNodePtr &node_ptr);
    void from_json(const nlohmann::json &json, CommandNodeTypeE &type);

    void ReadDefinition(CommandDefinition &definition, const std::filesystem::path &filepath);

    struct CommandNode
    {
        CommandNode(CommandNodeTypeE type, bool optional);
        virtual ~CommandNode() = default;

        virtual std::ostream &Print(std::ostream &stream, const std::string &space) const;
        virtual bool IsValidNext(Parser &parser, const CommandDefinition &definition) const = 0;
        virtual std::vector<ExpressionPtr> Parse(Parser &parser, const CommandDefinition &definition) const;
        virtual ExpressionPtr ParseOne(Parser &parser, const CommandDefinition &definition) const;

        CommandNodeTypeE Type;
        bool Optional;
    };

    struct FixedNode final : CommandNode
    {
        FixedNode(std::string value, std::string as, bool optional);

        std::ostream &Print(std::ostream &stream, const std::string &space) const override;
        bool IsValidNext(Parser &parser, const CommandDefinition &definition) const override;
        ExpressionPtr ParseOne(Parser &parser, const CommandDefinition &definition) const override;

        std::string Value;
        std::string As;
    };

    struct EnumNode final : CommandNode
    {
        EnumNode(std::vector<CommandNodePtr> allowed, bool optional);

        std::ostream &Print(std::ostream &stream, const std::string &space) const override;
        bool IsValidNext(Parser &parser, const CommandDefinition &definition) const override;
        std::vector<ExpressionPtr> Parse(Parser &parser, const CommandDefinition &definition) const override;

        std::vector<CommandNodePtr> Allowed;
    };

    struct SwitchNode final : CommandNode
    {
        SwitchNode(
            std::map<std::string, std::vector<CommandNodePtr>> cases,
            std::vector<CommandNodePtr> default_,
            bool optional);

        std::ostream &Print(std::ostream &stream, const std::string &space) const override;
        bool IsValidNext(Parser &parser, const CommandDefinition &definition) const override;
        std::vector<ExpressionPtr> Parse(Parser &parser, const CommandDefinition &definition) const override;

        std::map<std::string, std::vector<CommandNodePtr>> Cases;
        std::vector<CommandNodePtr> Default;
    };

    struct ValueNode final : CommandNode
    {
        ValueNode(std::string label, std::string is, bool optional);

        std::ostream &Print(std::ostream &stream, const std::string &space) const override;
        bool IsValidNext(Parser &parser, const CommandDefinition &definition) const override;
        ExpressionPtr ParseOne(Parser &parser, const CommandDefinition &definition) const override;

        std::string Label;
        std::string Is;
    };

    struct RuleNode final : CommandNode
    {
        RuleNode(std::string id, bool optional);

        std::ostream &Print(std::ostream &stream, const std::string &space) const override;
        bool IsValidNext(Parser &parser, const CommandDefinition &definition) const override;
        std::vector<ExpressionPtr> Parse(Parser &parser, const CommandDefinition &definition) const override;

        std::string Id;
    };

    struct GreedyNode final : CommandNode
    {
        GreedyNode(std::string label, bool optional);

        std::ostream &Print(std::ostream &stream, const std::string &space) const override;
        bool IsValidNext(Parser &parser, const CommandDefinition &definition) const override;
        std::vector<ExpressionPtr> Parse(Parser &parser, const CommandDefinition &definition) const override;

        std::string Label;
    };

    std::ostream &operator<<(std::ostream &stream, const CommandDefinition &definition);
    std::ostream &operator<<(std::ostream &stream, const CommandNodeTypeE &type);
}
