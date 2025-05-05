#include <mcc/tree.hpp>

mcc::FormatNode::FormatNode(SourceLocation where)
    : Where(std::move(where))
{
}
