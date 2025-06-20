#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>

mcc::ValuePtr mcc::Builder::Allocate(
    const SourceLocation &where,
    const TypePtr &type,
    const bool is_mutable) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");

    const auto location = m_InsertBlock->Parent->Location;
    const auto index = m_InsertBlock->Parent->StackIndex++;

    return GenericStorageReference::Create(
        where,
        type,
        location,
        std::format("stack[0].v[{}]", index),
        is_mutable);
}
