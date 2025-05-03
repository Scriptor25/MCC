#include <sstream>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseFormatExpression()
{
    auto format = Expect(TokenType_FormatString).Value;

    std::vector<FormatNodePtr> nodes;

    for (size_t pos; (pos = format.find("${")) != std::string::npos;)
    {
        if (pos != 0)
            nodes.emplace_back(std::make_unique<StringNode>(format.substr(0, pos)));

        format = format.substr(pos + 2);
        std::stringstream stream(format);

        Parser parser(stream, "none");
        auto expression = parser.ParseExpression();
        nodes.emplace_back(std::make_unique<ExpressionNode>(std::move(expression)));

        format = format.substr(stream.gcount() + 2);
    }

    return std::make_unique<FormatExpression>(std::move(nodes));
}
