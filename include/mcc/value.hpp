#pragma once

#include <mcc/common.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    struct Value
    {
        Value(const SourceLocation &where, TypePtr type);
        virtual ~Value() = default;

        virtual void Generate(CommandVector &commands, bool stack) const;
        [[nodiscard]] virtual bool RequireStack() const;
        [[nodiscard]] virtual Result GenerateResult(bool stringify) const;

        void Use();
        void Drop();

        SourceLocation Where;
        TypePtr Type;
        IndexT UseCount = 0;
    };

    struct NamedValue final : Value
    {
        static ValuePtr Create(
            const SourceLocation &where,
            TypePtr type,
            const ResourceLocation &location,
            const std::string &name);

        NamedValue(
            const SourceLocation &where,
            TypePtr type,
            const ResourceLocation &location,
            const std::string &name);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        std::string Name;
    };

    struct BranchResult final : Value
    {
        static ValuePtr Create(const SourceLocation &where, TypePtr type, const ResourceLocation &location);

        BranchResult(const SourceLocation &where, TypePtr type, const ResourceLocation &location);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
    };

    struct Block final : Value
    {
        static BlockPtr CreateTopLevel(
            const SourceLocation &where,
            TypePtr type,
            const ResourceLocation &location);
        static BlockPtr Create(
            const SourceLocation &where,
            const BlockPtr &parent,
            const ResourceLocation &location);

        Block(const SourceLocation &where, TypePtr type, const ResourceLocation &location);

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] InstructionPtr GetTerminator() const;
        [[nodiscard]] bool MayThrow() const;

        void ForwardArguments(std::string &prefix, std::string &arguments) const;

        ResourceLocation Location;
        std::vector<std::pair<std::string, ValuePtr>> Parameters;

        IndexT StackIndex = 0;
        std::vector<InstructionPtr> Instructions;

        BlockPtr Parent;
        std::vector<BlockPtr> Children;
    };

    struct FunctionResult final : Value
    {
        static ValuePtr Create(const SourceLocation &where, TypePtr type, const ResourceLocation &location);

        FunctionResult(const SourceLocation &where, TypePtr type, const ResourceLocation &location);

        [[nodiscard]] bool RequireStack() const override;
        Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
    };
}
