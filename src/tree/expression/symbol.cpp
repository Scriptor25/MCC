#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::SymbolExpression::SymbolExpression(SourceLocation where, std::string id)
    : Expression(std::move(where)),
      ID(std::move(id))
{
}

std::ostream &mcc::SymbolExpression::Print(std::ostream &stream) const
{
    return stream << ID;
}

mcc::ValuePtr mcc::SymbolExpression::Generate(Builder &builder, bool inline_) const
{
    return NamedValue::Create(builder.GetLocation(), ID);
}
