#pragma once

#include <mcc/value.hpp>

#include <set>

namespace mcc
{
    struct Block final : Value<Block>
    {
        static BlockPtr Create(
                const SourceLocation &where,
                const std::string &name,
                Context &context,
                const FunctionPtr &parent);

        Block(const SourceLocation &where,
              const std::string &name,
              Context &context,
              FunctionPtr parent);

        void Generate(
                CommandVector &commands,
                bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] InstructionPtr GetTerminator() const;

        [[nodiscard]] ResourceLocation GetLocation() const;
        void Erase() const;

        FunctionPtr Parent;

        std::set<BlockPtr> Predecessors;
        std::set<BlockPtr> Successors;

        std::vector<InstructionPtr> Instructions;
    };
}
