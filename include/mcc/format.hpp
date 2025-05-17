#pragma once

#include <iosfwd>
#include <string>
#include <mcc/common.hpp>
#include <mcc/lex.hpp>

namespace mcc
{
    struct FormatNode
    {
        explicit FormatNode(SourceLocation where);

        virtual ~FormatNode() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        [[nodiscard]] virtual ValuePtr Generate(Builder &builder, BlockPtr landing_pad) const = 0;

        SourceLocation Where;
    };

    struct StringNode final : FormatNode
    {
        StringNode(SourceLocation where, std::string value);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Generate(Builder &builder, BlockPtr landing_pad) const override;

        std::string Value;
    };

    struct ExpressionNode final : FormatNode
    {
        ExpressionNode(SourceLocation where, ExpressionPtr expression);

        std::ostream &Print(std::ostream &stream) const override;
        [[nodiscard]] ValuePtr Generate(Builder &builder, BlockPtr landing_pad) const override;

        ExpressionPtr Expression;
    };
}
