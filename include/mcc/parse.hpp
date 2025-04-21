#pragma once

#include <memory>
#include <mcc/lex.hpp>
#include <mcc/tree.hpp>

namespace mcc
{
    struct parser_t
    {
        [[nodiscard]] bool at(token_type type, const std::string &value = {}) const;

        bool skip_if(token_type type, const std::string &value = {});

        token_t skip();
        token_t expect(token_type type, std::string value = {});

        statement_ptr_t operator()();

        statement_ptr_t parse_statement();
        statement_ptr_t parse_namespace_statement();
        statement_ptr_t parse_define_statement();

        expression_ptr_t parse_expression();

        std::istream &stream;
        token_t token;
    };
}
