#include <mcc/error.hpp>
#include <mcc/statement.hpp>

mcc::Statement::Statement(const SourceLocation &where)
    : TreeNode(where)
{
}

void mcc::Statement::Generate(Builder &builder) const
{
    Frame target_frame;
    Generate(builder, target_frame);
}

void mcc::Statement::GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const
{
    Error(Where, "mcc::Statement::GenerateInclude");
}
