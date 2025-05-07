#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::SymbolExpression::SymbolExpression(SourceLocation where, const bool placeholder, std::string id)
    : Expression(std::move(where)),
      Placeholder(placeholder),
      ID(std::move(id))
{
}

std::ostream &mcc::SymbolExpression::Print(std::ostream &stream) const
{
    return stream << (Placeholder ? "" : "%") << ID;
}

mcc::ValuePtr mcc::SymbolExpression::Generate(Builder &builder, bool inline_) const
{
    if (Placeholder)
        return ConstantSymbol::Create(ID);
    return NamedValue::Create(builder.GetLocation(), ID);
}
