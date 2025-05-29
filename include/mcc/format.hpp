#pragma once

#include <iosfwd>
#include <mcc/common.hpp>
#include <mcc/lex.hpp>
#include <string>

namespace mcc
{
    struct FormatNode
    {
        explicit FormatNode(const SourceLocation &where);

        virtual ~FormatNode() = default;

        virtual std::ostream &Print(std::ostream &stream) const                             = 0;
        [[nodiscard]] virtual ValuePtr Generate(Builder &builder, const Frame &frame) const = 0;

        SourceLocation Where;
    };

    struct StringNode final : FormatNode
    {
        StringNode(const SourceLocation &where, const std::string &value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Generate(Builder &builder, const Frame &frame) const override;

        std::string Value;
    };

    struct ExpressionNode final : FormatNode
    {
        ExpressionNode(const SourceLocation &where, ExpressionPtr expression);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Generate(Builder &builder, const Frame &frame) const override;

        ExpressionPtr Expression;
    };
}
