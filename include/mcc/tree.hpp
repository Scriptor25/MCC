#pragma once

#include <filesystem>
#include <set>
#include <mcc/common.hpp>
#include <mcc/resource.hpp>

namespace mcc
{
    struct TreeNode
    {
        explicit TreeNode(SourceLocation where);
        virtual ~TreeNode() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

        virtual void Generate(Builder &builder) const = 0;
        virtual void GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const = 0;

        SourceLocation Where;
    };

    struct DefineNode final : TreeNode
    {
        DefineNode(
            const SourceLocation &where,
            ResourceLocation location,
            ParameterList parameters,
            TypePtr result_type,
            bool throws,
            const std::vector<ResourceTag> &tags,
            StatementPtr body);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;
        void GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const override;

        void Check(const FunctionPtr &function) const;

        ResourceLocation Location;
        ParameterList Parameters;
        TypePtr ResultType;
        bool Throws;
        std::vector<ResourceTag> Tags;
        StatementPtr Body;
    };

    struct GlobalNode final : TreeNode
    {
        GlobalNode(const SourceLocation &where, ResourceLocation location, TypePtr type);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;
        void GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const override;

        ResourceLocation Location;
        TypePtr Type;
    };

    struct IncludeNode final : TreeNode
    {
        IncludeNode(const SourceLocation &where, std::filesystem::path filepath);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;
        void GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const override;

        std::filesystem::path Filepath;
    };

    struct NamespaceNode final : TreeNode
    {
        NamespaceNode(const SourceLocation &where, std::string namespace_);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;
        void GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const override;

        std::string Namespace;
    };
}
