#include <mcc/format.hpp>

mcc::FormatNode::FormatNode(SourceLocation where)
    : Where(std::move(where))
{
}
