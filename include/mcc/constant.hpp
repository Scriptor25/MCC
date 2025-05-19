#pragma once

#include <map>
#include <optional>
#include <mcc/common.hpp>
#include <mcc/value.hpp>

namespace mcc
{
    struct Constant : Value
    {
        Constant(const SourceLocation &where, const TypePtr &type);

        [[nodiscard]] bool RequireStack() const override;
    };

    struct ConstantBoolean final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, bool value);

        ConstantBoolean(const SourceLocation &where, bool value);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        bool Value;
    };

    struct ConstantInteger final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, IntegerT value);

        ConstantInteger(const SourceLocation &where, IntegerT value);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        IntegerT Value;
    };

    struct ConstantFloat final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, FloatT value);

        ConstantFloat(const SourceLocation &where, FloatT value);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        FloatT Value;
    };

    struct ConstantFloatRange final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, FloatT min, FloatT max);
        static ConstantPtr CreateMin(const SourceLocation &where, FloatT min);
        static ConstantPtr CreateMax(const SourceLocation &where, FloatT max);

        ConstantFloatRange(const SourceLocation &where, std::optional<FloatT> min, std::optional<FloatT> max);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        std::optional<FloatT> Min;
        std::optional<FloatT> Max;
    };

    struct ConstantString final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, const std::string &value);

        ConstantString(const SourceLocation &where, const std::string &value);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        std::string Value;
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
            const std::vector<ConstantPtr> &values,
            bool stringify);

        ConstantArray(
            const SourceLocation &where,
            const TypePtr &type,
            const std::vector<ConstantPtr> &values,
            bool stringify);
        ~ConstantArray() override;

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        std::vector<ConstantPtr> Values;
        bool Stringify;
    };

    struct ConstantObject final : Constant
    {
        static ConstantPtr Create(
            const SourceLocation &where,
            const TypePtr &type,
            const std::map<std::string, ConstantPtr> &values);
        static ConstantPtr Create(
            const SourceLocation &where,
            const std::map<std::string, ConstantPtr> &values);

        ConstantObject(
            const SourceLocation &where,
            const TypePtr &type,
            const std::map<std::string, ConstantPtr> &values);
        ~ConstantObject() override;

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        std::map<std::string, ConstantPtr> Values;
    };

    struct ConstantTarget final : Constant
    {
        static ConstantPtr Create(
            const SourceLocation &where,
            TargetSelectorE selector,
            std::map<std::string, std::vector<TargetAttributePtr>> attributes);

        ConstantTarget(
            const SourceLocation &where,
            TargetSelectorE selector,
            std::map<std::string, std::vector<TargetAttributePtr>> attributes);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        TargetSelectorE Selector;
        std::map<std::string, std::vector<TargetAttributePtr>> Attributes;
    };

    struct ConstantResource final : Constant
    {
        static ConstantPtr Create(const SourceLocation &where, const ResourceLocation &location);

        ConstantResource(const SourceLocation &where, ResourceLocation location);

        [[nodiscard]] Result GenerateResult(bool stringify) const override;

        ResourceLocation Location;
    };
}
