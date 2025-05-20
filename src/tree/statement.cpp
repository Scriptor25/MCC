#include <mcc/error.hpp>
#include <mcc/statement.hpp>

mcc::Statement::Statement(const SourceLocation &where)
    : TreeNode(where)
{
}

void mcc::Statement::Generate(Builder &builder) const
{
    Generate(builder, {});
}

void mcc::Statement::GenerateInclude(Builder &builder) const
{
    Error(Where, "mcc::Statement::GenerateInclude");
}
