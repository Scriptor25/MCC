#pragma once

#include <memory>
#include <vector>

namespace mcc
{
    using statement_ptr_t = std::unique_ptr<struct statement_t>;
    using expression_ptr_t = std::unique_ptr<struct expression_t>;

    struct statement_t
    {
        virtual ~statement_t() = default;

        virtual std::ostream &print(std::ostream &stream) const = 0;
    };

    struct namespace_statement_t final : statement_t
    {
        explicit namespace_statement_t(std::string id)
            : id(std::move(id))
        {
        }

        std::ostream &print(std::ostream &stream) const override
        {
            return stream << "namespace " << id;
        }

        std::string id;
    };

    struct define_statement_t final : statement_t
    {
        define_statement_t(
            std::string id,
            std::vector<std::string> parameters,
            std::vector<statement_ptr_t> statements)
            : id(std::move(id)),
              parameters(std::move(parameters)),
              statements(std::move(statements))
        {
        }

        std::ostream &print(std::ostream &stream) const override
        {
            stream << "define " << id << "(";
            for (unsigned i = 0; i < parameters.size(); ++i)
            {
                if (i > 0)
                    stream << ", ";
                stream << parameters[i];
            }
            stream << ") {" << std::endl;
            for (auto &statement: statements)
                statement->print(stream) << std::endl;
            return stream << "}";
        }

        std::string id;
        std::vector<std::string> parameters;
        std::vector<statement_ptr_t> statements;
    };

    struct expression_t : statement_t
    {
    };
}
