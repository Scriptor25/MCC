#include <utility>
#include <mcc/tree.hpp>

mcc::TreeNode::TreeNode(SourceLocation where)
    : Where(std::move(where))
{
}
