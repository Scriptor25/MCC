#pragma once

#include <mcc/common.hpp>
#include <mcc/lex.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    struct Value
    {
        explicit Value(const SourceLocation &where);
        virtual ~Value() = default;

        virtual void Generate(CommandVector &commands, bool stack) const;
        [[nodiscard]] virtual bool RequireStack() const;
        [[nodiscard]] virtual Result GenerateResult(bool stringify) const;

        void Use();
        void Drop();

        SourceLocation Where;
        IndexT UseCount = 0;
    };

    struct NamedValue final : Value
    {
        static ValuePtr Create(const SourceLocation &where, const ResourceLocation &location, const std::string &name);

        NamedValue(const SourceLocation &where, const ResourceLocation &location, const std::string &name);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        std::string Name;
    };

    struct BranchResult final : Value
    {
        static ValuePtr Create(const SourceLocation &where, const ResourceLocation &location);

        BranchResult(const SourceLocation &where, const ResourceLocation &location);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
    };

    struct Block final : Value
    {
        static BlockPtr CreateTopLevel(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ParameterList &parameters);
        static BlockPtr Create(
            const SourceLocation &where,
            const BlockPtr &parent,
            const ResourceLocation &location);

        Block(
            const SourceLocation &where,
            const ResourceLocation &location,
            const ParameterList &parameters);

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] InstructionPtr GetTerminator() const;
        [[nodiscard]] bool MayThrow() const;

        void ForwardArguments(std::string &prefix, std::string &arguments) const;

        ResourceLocation Location;
        ParameterList Parameters;

        IndexT StackIndex = 0;
        std::map<std::string, ValuePtr> Variables;
        std::vector<InstructionPtr> Instructions;

        BlockPtr Parent;
        std::vector<BlockPtr> Children;
    };

    struct FunctionResult final : Value
    {
        static ValuePtr Create(const SourceLocation &where, const ResourceLocation &location);

        FunctionResult(const SourceLocation &where, const ResourceLocation &location);

        [[nodiscard]] bool RequireStack() const override;
        Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
    };
}
