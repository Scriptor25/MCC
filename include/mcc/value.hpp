#pragma once

#include <mcc/common.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    struct Value
    {
        virtual ~Value() = default;

        virtual void Generate(CommandVector &commands, bool stack) const;
        [[nodiscard]] virtual bool RequireStack() const;
        [[nodiscard]] virtual Result GenerateResult(bool stringify) const;

        void Use();
        void Drop();

        IndexT UseCount = 0;
    };

    struct NamedValue final : Value
    {
        static ValuePtr Create(ResourceLocation location, std::string name);

        NamedValue(ResourceLocation location, std::string name);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
        std::string Name;
    };

    struct BranchResult final : Value
    {
        static ValuePtr Create(ResourceLocation location);

        explicit BranchResult(ResourceLocation location);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
    };

    struct Block final : Value
    {
        static BlockPtr CreateTopLevel(
            ResourceLocation location,
            ParameterList parameters);
        static BlockPtr Create(const BlockPtr &parent, ResourceLocation location);

        Block(ResourceLocation location, ParameterList parameters);

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
        static ValuePtr Create(ResourceLocation location);

        explicit FunctionResult(ResourceLocation location);

        [[nodiscard]] bool RequireStack() const override;
        Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
    };
}
