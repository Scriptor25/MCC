#pragma once

#include <mcc/common.hpp>

namespace mcc
{
    struct TargetAttribute
    {
        explicit TargetAttribute(bool invert);

        virtual ~TargetAttribute() = default;
        virtual std::ostream &Print(std::ostream &stream) const = 0;
        [[nodiscard]] virtual std::string String() const = 0;

        bool Invert;
    };

    struct EnumAttribute final : TargetAttribute
    {
        EnumAttribute(bool invert, std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        std::string Value;
    };

    struct FloatAttribute final : TargetAttribute
    {
        FloatAttribute(bool invert, FloatT value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        FloatT Value;
    };

    struct IntegerAttribute final : TargetAttribute
    {
        IntegerAttribute(bool invert, IntegerT value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        IntegerT Value;
    };

    struct MapAttribute final : TargetAttribute
    {
        MapAttribute(bool invert, std::map<std::string, TargetAttributePtr> values);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        std::map<std::string, TargetAttributePtr> Values;
    };

    struct NameAttribute final : TargetAttribute
    {
        NameAttribute(bool invert, std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        std::string Value;
    };

    struct NBTAttribute final : TargetAttribute
    {
        NBTAttribute(bool invert, ExpressionPtr value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        ExpressionPtr Value;
    };

    struct RangeAttribute final : TargetAttribute
    {
        RangeAttribute(bool invert, std::optional<IntegerT> beg, std::optional<IntegerT> end);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        std::optional<IntegerT> Beg, End;
    };

    struct ResourceAttribute final : TargetAttribute
    {
        ResourceAttribute(bool invert, ResourceLocation value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        ResourceLocation Value;
    };

    struct ResourceMapAttribute final : TargetAttribute
    {
        ResourceMapAttribute(bool invert, std::vector<std::pair<ResourceLocation, TargetAttributePtr>> values);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        std::vector<std::pair<ResourceLocation, TargetAttributePtr>> Values;
    };

    struct StringAttribute final : TargetAttribute
    {
        StringAttribute(bool invert, std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        std::string Value;
    };

    struct TagAttribute final : TargetAttribute
    {
        TagAttribute(bool invert, ResourceTag value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] std::string String() const override;

        ResourceTag Value;
    };
}
