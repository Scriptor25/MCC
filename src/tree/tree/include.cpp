#include <filesystem>
#include <fstream>
#include <utility>
#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>
#include <mcc/tree.hpp>

mcc::IncludeNode::IncludeNode(const SourceLocation &where, std::filesystem::path filepath)
    : TreeNode(where),
      Filepath(std::move(filepath))
{
}

std::ostream &mcc::IncludeNode::Print(std::ostream &stream) const
{
    return stream << "include " << '"' << Filepath.string() << '"';
}

void mcc::IncludeNode::Generate(Builder &builder) const
{
    std::ifstream stream(Filepath);
    Assert(stream.is_open(), Where, "failed to open file {}", Filepath.string());

    std::set<std::filesystem::path> include_chain;
    include_chain.insert(std::filesystem::canonical(Where.Filename));
    include_chain.insert(canonical(Filepath));

    Parser parser(builder.GetContext(), stream, Filepath.string());
    while (parser)
        if (const auto statement = parser())
            statement->GenerateInclude(builder, include_chain);
}

void mcc::IncludeNode::GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const
{
    if (include_chain.contains(canonical(Filepath)))
    {
        Warning(Where, "recursive include chain detected!");
        return;
    }

    std::ifstream stream(Filepath);
    Assert(stream.is_open(), Where, "failed to open file {}", Filepath.string());

    include_chain.insert(canonical(Filepath));

    Parser parser(builder.GetContext(), stream, Filepath.string());
    while (parser)
        if (const auto statement = parser())
            statement->GenerateInclude(builder, include_chain);
}
