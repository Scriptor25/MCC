#pragma once

#include <mcc/value.hpp>

namespace mcc
{
    struct Function final : Value
    {
        static FunctionPtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const ParameterList &parameters,
            const TypePtr &result_type,
            bool throws);

        Function(
            const SourceLocation &where,
            const TypePtr &type,
            ResourceLocation location,
            ParameterList parameters,
            TypePtr result_type,
            bool throws);

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;
        [[nodiscard]] Result GenerateResultUnwrap() const override;

        bool RemoveUnreferencedBlocks();
        bool MergeConsecutiveBlocks();

        void OptimizeBlocks();
        void GenerateFunction(Package &package) const;

        void ForwardArguments(std::string &prefix, std::string &arguments) const;

        [[nodiscard]] ResourceLocation GetLocation(const BlockPtr &target_block) const;
        BlockPtr Erase(const BlockPtr &target_block);

        ResourceLocation Location;
        ParameterList Parameters;
        TypePtr ResultType;
        bool Throws;

        IndexT StackIndex = 0;
        std::vector<BlockPtr> Blocks;
    };
}
