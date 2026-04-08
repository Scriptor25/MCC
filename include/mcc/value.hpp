#pragma once

#include <mcc/common.hpp>
#include <mcc/package.hpp>
#include <mcc/result.hpp>

namespace mcc
{
    auto operator<=>(
            const WeakValuePtr &lhs,
            const WeakValuePtr &rhs);

    struct ValueBase
    {
        ValueBase(
                SourceLocation where,
                TypePtr type,
                E_FieldType field_type);
        virtual ~ValueBase() = default;

        virtual void Generate(
                CommandVector &commands,
                bool stack) const;

        [[nodiscard]] virtual bool RequireStack() const;
        [[nodiscard]] virtual Result GenerateResult() const;
        [[nodiscard]] virtual Result GenerateResultUnwrap() const;

        void Use(WeakValuePtr user);
        void Drop(WeakValuePtr user);

        bool IsMutable() const;

        SourceLocation Where;
        TypePtr Type;
        E_FieldType FieldType;

        std::set<WeakValuePtr> Uses;

        const uint32_t StackId;
    };

    template<typename T>
    struct Value : ValueBase
    {
        using ValueBase::ValueBase;

        using SPtr = std::shared_ptr<T>;
        using WPtr = std::weak_ptr<T>;

        WPtr Self;
    };

    struct ArgumentValue final : Value<ArgumentValue>
    {
        static ValuePtr Create(
                const SourceLocation &where,
                const TypePtr &type,
                const std::string &name);

        ArgumentValue(
                const SourceLocation &where,
                const TypePtr &type,
                std::string name);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;
        [[nodiscard]] Result GenerateResultUnwrap() const override;

        std::string Name;
    };

    struct BranchResult final : Value<BranchResult>
    {
        static ValuePtr Create(
                const SourceLocation &where,
                const TypePtr &type,
                const ResourceLocation &location);

        BranchResult(
                const SourceLocation &where,
                const TypePtr &type,
                ResourceLocation location);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ResourceLocation Location;
    };

    struct ElementReference final : Value<ElementReference>
    {
        static SPtr Create(
                const SourceLocation &where,
                const ValuePtr &base,
                const ValuePtr &index);

        ElementReference(
                const SourceLocation &where,
                const TypePtr &type,
                const ValuePtr &base,
                const ValuePtr &index);
        ~ElementReference() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ValuePtr Base;
        ValuePtr Index;
    };

    struct FunctionResult final : Value<FunctionResult>
    {
        static ValuePtr Create(
                const SourceLocation &where,
                const TypePtr &type,
                const ResourceLocation &location);

        FunctionResult(
                const SourceLocation &where,
                const TypePtr &type,
                ResourceLocation location);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ResourceLocation Location;
    };

    struct GenericBlockReference final : Value<GenericBlockReference>
    {
        static ValuePtr Create(
                const SourceLocation &where,
                const TypePtr &type,
                const ValuePtr &position_x,
                const ValuePtr &position_y,
                const ValuePtr &position_z,
                const ValuePtr &path);

        GenericBlockReference(
                const SourceLocation &where,
                const TypePtr &type,
                const ValuePtr &position_x,
                const ValuePtr &position_y,
                const ValuePtr &position_z,
                const ValuePtr &path);
        ~GenericBlockReference() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ValuePtr PositionX, PositionY, PositionZ, Path;
    };

    struct GenericEntityReference final : Value<GenericEntityReference>
    {
        static ValuePtr Create(
                const SourceLocation &where,
                const TypePtr &type,
                const ValuePtr &name,
                const ValuePtr &path);

        GenericEntityReference(
                const SourceLocation &where,
                const TypePtr &type,
                const ValuePtr &name,
                const ValuePtr &path);
        ~GenericEntityReference() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ValuePtr Name, Path;
    };

    struct GenericStorageReference final : Value<GenericStorageReference>
    {
        static ValuePtr Create(
                const SourceLocation &where,
                const TypePtr &type,
                const ValuePtr &location,
                const ValuePtr &path,
                bool is_mutable);

        static ValuePtr Create(
                const SourceLocation &where,
                const TypePtr &type,
                const ResourceLocation &location,
                const std::string &path,
                bool is_mutable);

        GenericStorageReference(
                const SourceLocation &where,
                const TypePtr &type,
                ValuePtr location,
                ValuePtr path,
                bool is_mutable);
        ~GenericStorageReference() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ValuePtr Location, Path;
    };

    struct MemberReference final : Value<MemberReference>
    {
        static ValuePtr Create(
                const SourceLocation &where,
                const ValuePtr &object,
                const std::string &member);

        MemberReference(
                const SourceLocation &where,
                const TypePtr &type,
                const ValuePtr &object,
                std::string member);
        ~MemberReference() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ValuePtr Object;
        std::string Member;
    };

    struct StringifyValue final : Value<StringifyValue>
    {
        static ValuePtr Create(
                const SourceLocation &where,
                const ValuePtr &target);

        StringifyValue(
                const SourceLocation &where,
                const ValuePtr &target);
        ~StringifyValue() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ValuePtr Target;
    };
}
