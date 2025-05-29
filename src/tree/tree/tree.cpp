#include <mcc/tree.hpp>
#include <utility>

mcc::TreeNode::TreeNode(SourceLocation where)
    : Where(std::move(where))
{
}
