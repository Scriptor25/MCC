#include <mcc/statement.hpp>

mcc::MultiStatement::MultiStatement(const SourceLocation &where, std::vector<StatementPtr> statements)
    : Statement(where),
      Statements(std::move(statements))
{
}

std::ostream &mcc::MultiStatement::Print(std::ostream &stream) const
{
    static std::string indentation;

    stream << '{' << std::endl;
    indentation.append(2, ' ');
    for (auto &statement: Statements)
        statement->Print(stream << indentation) << std::endl;
    indentation.erase(0, 2);
    return stream << indentation << '}';
}

void mcc::MultiStatement::Generate(Builder &builder, const Frame &frame) const
{
    for (auto &statement: Statements)
        statement->Generate(builder, frame);
}
