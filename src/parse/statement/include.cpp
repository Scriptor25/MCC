#include <filesystem>
#include <fstream>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseIncludeStatement()
{
    const auto where = Expect(TokenType_Symbol, "include").Where;
    const auto filename = Expect(TokenType_String).Value;

    std::filesystem::path filepath(filename);
    if (filepath.is_relative())
        filepath = std::filesystem::path(where.Filename).parent_path() / filename;

    return std::make_unique<IncludeStatement>(where, filepath);
}
