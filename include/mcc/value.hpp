#pragma once

#include <mcc/common.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    struct Value
    {
        Value(SourceLocation where, TypeContext &context, TypePtr type);
        virtual ~Value() = default;

        virtual void Generate(CommandVector &commands, bool stack) const;
        [[nodiscard]] virtual bool RequireStack() const;
        [[nodiscard]] virtual Result GenerateResult(bool stringify) const;

        void Use();
        void Drop();

        SourceLocation Where;
        TypeContext &Context;
        TypePtr Type;
        IndexT UseCount = 0;
    };

    struct BranchResult final : Value
    {
        static ValuePtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location);

        BranchResult(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
    };

    struct Block final : Value
    {
        static BlockPtr Create(const SourceLocation &where, TypeContext &context, const FunctionPtr &parent);

        Block(const SourceLocation &where, TypeContext &context, FunctionPtr parent);

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        [[nodiscard]] InstructionPtr GetTerminator() const;

        FunctionPtr Parent;

        std::vector<BlockPtr> Predecessors;
        BlockPtr Successor;

        std::vector<InstructionPtr> Instructions;
    };

    struct ElementReference final : Value
    {
        static ValuePtr Create(const SourceLocation &where, TypeContext &context, const ValuePtr &array, IndexT index);

        ElementReference(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            ValuePtr array,
            IndexT index);
        ~ElementReference() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ValuePtr Array;
        IndexT Index;
    };

    struct Function final : Value
    {
        static FunctionPtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const ParameterList &parameters,
            const TypePtr &result,
            bool throws);

        Function(
            const SourceLocation &where,
            TypeContext &context,
            const ResourceLocation &location,
            const ParameterList &parameters,
            const TypePtr &result,
            bool throws);

        void Generate(CommandVector &commands, bool stack) const override;
        [[nodiscard]] bool RequireStack() const override;

        void GenerateFunction(Package &package) const;

        void ForwardArguments(std::string &prefix, std::string &arguments) const;

        [[nodiscard]] ResourceLocation GetLocation(const BlockPtr &target_block) const;
        BlockPtr Erase(const BlockPtr &target_block);

        ResourceLocation Location;
        std::vector<std::pair<std::string, ValuePtr>> Parameters;
        TypePtr Result;
        bool Throws;

        IndexT StackIndex = 0;
        std::vector<BlockPtr> Blocks;
    };

    struct FunctionResult final : Value
    {
        static ValuePtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location);

        FunctionResult(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
    };

    struct MemberReference final : Value
    {
        static ValuePtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const ValuePtr &object,
            const std::string &member);

        MemberReference(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            ValuePtr object,
            const std::string &member);
        ~MemberReference() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ValuePtr Object;
        std::string Member;
    };

    struct NamedValue final : Value
    {
        static ValuePtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location,
            const std::string &name);

        NamedValue(
            const SourceLocation &where,
            TypeContext &context,
            const TypePtr &type,
            const ResourceLocation &location,
            const std::string &name);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        std::string Name;
    };
}
