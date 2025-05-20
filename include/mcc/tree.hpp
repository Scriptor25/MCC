#pragma once

#include <filesystem>
#include <mcc/common.hpp>

namespace mcc
{
    struct TreeNode
    {
        explicit TreeNode(SourceLocation where);
        virtual ~TreeNode() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

        virtual void Generate(Builder &builder) const = 0;
        virtual void GenerateInclude(Builder &builder) const = 0;

        SourceLocation Where;
    };

    struct DefineNode final : TreeNode
    {
        DefineNode(
            const SourceLocation &where,
            ResourceLocation location,
            ParameterList parameters,
            TypePtr result,
            bool throws,
            const std::vector<ResourceLocation> &tags,
            StatementPtr body);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;
        void GenerateInclude(Builder &builder) const override;

        ResourceLocation Location;
        ParameterList Parameters;
        TypePtr Result;
        bool Throws;
        std::vector<ResourceLocation> Tags;
        StatementPtr Body;
    };

    struct IncludeNode final : TreeNode
    {
        IncludeNode(const SourceLocation &where, std::filesystem::path filepath);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;
        void GenerateInclude(Builder &builder) const override;

        std::filesystem::path Filepath;
    };

    struct NamespaceNode final : TreeNode
    {
        NamespaceNode(const SourceLocation &where, std::string namespace_);

        std::ostream &Print(std::ostream &stream) const override;
        void Generate(Builder &builder) const override;
        void GenerateInclude(Builder &builder) const override;

        std::string Namespace;
    };
}
