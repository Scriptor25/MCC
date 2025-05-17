#include <sstream>
#include <mcc/format.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseFormatExpression()
{
    auto where = m_Token.Where;
    auto format = Expect(TokenType_FormatString).Value;

    std::vector<FormatNodePtr> nodes;

    unsigned offset = 0;

    auto get_where = [&]() -> SourceLocation
    {
        return {
            where.Filename,
            where.Col + offset,
            where.Row,
        };
    };

    for (size_t pos; (pos = format.find("${")) != std::string::npos;)
    {
        if (pos != 0)
            nodes.emplace_back(std::make_unique<StringNode>(get_where(), format.substr(0, pos)));

        format = format.substr(pos + 2);

        offset += pos;
        auto node_where = get_where();

        offset += 2;
        auto stream_where = get_where();

        std::stringstream stream(format);
        Parser parser(m_Context, stream, stream_where);

        auto expression = parser.ParseExpression();

        nodes.emplace_back(std::make_unique<ExpressionNode>(node_where, std::move(expression)));

        auto count = parser.Count();
        format = format.substr(count);
        offset += count;
    }

    if (!format.empty())
        nodes.emplace_back(std::make_unique<StringNode>(get_where(), format));

    return std::make_unique<FormatExpression>(where, std::move(nodes));
}
