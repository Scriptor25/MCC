#pragma once

#include <mcc/common.hpp>
#include <mcc/package.hpp>
#include <mcc/result.hpp>

namespace mcc
{
    struct Value
    {
        Value(SourceLocation where, TypePtr type, E_FieldType field_type);
        virtual ~Value() = default;

        virtual void Generate(CommandVector &commands, bool stack) const;
        [[nodiscard]] virtual bool RequireStack() const;
        [[nodiscard]] virtual Result GenerateResult() const;
        [[nodiscard]] virtual Result GenerateResultUnwrap() const;

        void Use();
        void Drop();

        bool IsMutable() const;

        SourceLocation Where;
        TypePtr Type;
        E_FieldType FieldType;

        IndexT UseCount = 0;
    };

    struct ArgumentValue final : Value
    {
        static ValuePtr Create(const SourceLocation &where, const TypePtr &type, const std::string &name);

        ArgumentValue(const SourceLocation &where, const TypePtr &type, std::string name);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;
        [[nodiscard]] Result GenerateResultUnwrap() const override;

        std::string Name;
    };

    struct BranchResult final : Value
    {
        static ValuePtr Create(const SourceLocation &where, const TypePtr &type, const ResourceLocation &location);

        BranchResult(const SourceLocation &where, const TypePtr &type, ResourceLocation location);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ResourceLocation Location;
    };

    struct ElementReference final : Value
    {
        static ValuePtr Create(const SourceLocation &where, const ValuePtr &base, const ValuePtr &index);

        ElementReference(const SourceLocation &where, const TypePtr &type, const ValuePtr &base, const ValuePtr &index);
        ~ElementReference() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ValuePtr Base;
        ValuePtr Index;
    };

    struct FunctionResult final : Value
    {
        static ValuePtr Create(const SourceLocation &where, const TypePtr &type, const ResourceLocation &location);

        FunctionResult(const SourceLocation &where, const TypePtr &type, ResourceLocation location);

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ResourceLocation Location;
    };

    struct GenericBlockReference final : Value
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

    struct GenericEntityReference final : Value
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

    struct GenericStorageReference final : Value
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

    struct MemberReference final : Value
    {
        static ValuePtr Create(const SourceLocation &where, const ValuePtr &object, const std::string &member);

        MemberReference(const SourceLocation &where, const TypePtr &type, const ValuePtr &object, std::string member);
        ~MemberReference() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ValuePtr Object;
        std::string Member;
    };

    struct StringifyValue final : Value
    {
        static ValuePtr Create(const SourceLocation &where, const ValuePtr &target);

        StringifyValue(const SourceLocation &where, const ValuePtr &target);
        ~StringifyValue() override;

        [[nodiscard]] bool RequireStack() const override;
        [[nodiscard]] Result GenerateResult() const override;

        ValuePtr Target;
    };
}
