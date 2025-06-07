#pragma once

#include <map>
#include <mcc/common.hpp>
#include <mcc/result.hpp>
#include <mcc/value.hpp>

namespace mcc
{
    struct Constant : Value
    {
        Constant(const SourceLocation &where, const TypePtr &type);

        [[nodiscard]] bool RequireStack() const override;
    };

    struct ConstantArray final : Constant
    {
        static ConstantPtr Create(
            const SourceLocation &where,
            const TypePtr &type,
            const std::vector<ConstantPtr> &values,
            bool stringify);
        static ConstantPtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const std::vector<ConstantPtr> &values,
            bool stringify);

        ConstantArray(
            const SourceLocation &where,
            const TypePtr &type,
            const std::vector<ConstantPtr> &values,
            bool stringify);
        ~ConstantArray() override;

        [[nodiscard]] Result GenerateResult() const override;

        std::vector<ConstantPtr> Values;
        bool Stringify;
    };

    struct ConstantBoolean final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, TypeContext &context, bool value);

        ConstantBoolean(const SourceLocation &where, TypeContext &context, bool value);

        [[nodiscard]] Result GenerateResult() const override;

        bool Value;
    };

    struct ConstantNull final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, TypeContext &context);

        ConstantNull(const SourceLocation &where, TypeContext &context);

        [[nodiscard]] Result GenerateResult() const override;
    };

    struct ConstantNumber final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, TypeContext &context, IntegerT value);

        ConstantNumber(const SourceLocation &where, TypeContext &context, IntegerT value);

        [[nodiscard]] Result GenerateResult() const override;

        IntegerT Value;
    };

    struct ConstantObject final : Constant
    {
        static ConstantPtr Create(
            const SourceLocation &where,
            const TypePtr &type,
            const std::map<std::string, ConstantPtr> &values);
        static ConstantPtr Create(
            const SourceLocation &where,
            TypeContext &context,
            const std::map<std::string, ConstantPtr> &values);

        ConstantObject(
            const SourceLocation &where,
            const TypePtr &type,
            const std::map<std::string, ConstantPtr> &values);
        ~ConstantObject() override;

        [[nodiscard]] Result GenerateResult() const override;

        std::map<std::string, ConstantPtr> Values;
    };

    struct ConstantResource final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, TypeContext &context, const ResourceLocation &location);

        ConstantResource(const SourceLocation &where, TypeContext &context, ResourceLocation location);

        [[nodiscard]] Result GenerateResult() const override;

        ResourceLocation Location;
    };

    struct ConstantString final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, TypeContext &context, const std::string &value);

        ConstantString(const SourceLocation &where, TypeContext &context, std::string value);

        [[nodiscard]] Result GenerateResult() const override;
        [[nodiscard]] Result GenerateResultUnwrap() const override;

        std::string Value;
    };
}
