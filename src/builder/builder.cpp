#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>

#include <ranges>

mcc::Builder::Builder(
        Context &context,
        Package &package)
    : m_Context(context),
      m_Package(package),
      m_Module(m_Context)
{
}

mcc::Context &mcc::Builder::GetContext() const
{
    return m_Context;
}

mcc::Package &mcc::Builder::GetPackage() const
{
    return m_Package;
}

std::string mcc::Builder::GetNamespace() const
{
    return m_Namespace;
}

void mcc::Builder::SetNamespace(const std::string &namespace_)
{
    m_Namespace = namespace_;
}

void mcc::Builder::SetInsertBlock(const BlockPtr &block)
{
    m_InsertBlock = block;
}

mcc::BlockPtr mcc::Builder::GetInsertBlock() const
{
    return m_InsertBlock;
}

void mcc::Builder::PushVariables()
{
    m_Variables.emplace_back();
}

void mcc::Builder::PopVariables()
{
    m_Variables.pop_back();
}

mcc::InstructionPtr mcc::Builder::Insert(
        const SourceLocation &where,
        const InstructionPtr &instruction) const
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!!instruction, where, "instruction must not be null");

    m_InsertBlock->Instructions.push_back(instruction);
    return instruction;
}

void mcc::Builder::Generate() const
{
    for (auto &function : m_Module)
    {
        if (function->Blocks.empty())
            continue;

        function->OptimizeBlocks();
        function->GenerateFunction(m_Package);
    }
}
