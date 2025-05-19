#include <fstream>
#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::IncludeStatement::IncludeStatement(const SourceLocation &where, const std::filesystem::path &filepath)
    : Statement(where),
      Filepath(filepath)
{
}

std::ostream &mcc::IncludeStatement::Print(std::ostream &stream) const
{
    return stream << "include " << '"' << Filepath.string() << '"';
}

void mcc::IncludeStatement::Generate(Builder &builder, const Frame &frame) const
{
    std::ifstream stream(Filepath);
    Assert(stream.is_open(), Where, "failed to open file {}", Filepath.string());

    Parser parser(builder.GetContext(), stream, Filepath.string());
    while (parser)
        if (const auto statement = parser())
            statement->GenerateInclude(builder);
}

void mcc::IncludeStatement::GenerateInclude(Builder &builder) const
{
    Error(Where, "mcc::IncludeStatement::GenerateInclude");
}
