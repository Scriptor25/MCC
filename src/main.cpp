#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <mcc/lex.hpp>
#include <mcc/actions.hpp>
#include <mcc/parse.hpp>

enum operand_type
{
    operand_type_enum,

    /**
     * either 'true' or 'false'
     */
    operand_type_bool,
    /**
     * double float value (64 bit)
     */
    operand_type_double,
    /**
     * single float value (32 bit)
     */
    operand_type_float,
    /**
     * integer value (32 bit)
     */
    operand_type_integer,
    /**
     * long integer value (64 bit)
     */
    operand_type_long,
    /**
     * single word string
     */
    operand_type_string_single,
    /**
     * single word string, or quoted string
     */
    operand_type_string_quotes,
    /**
     * takes everything else of the command string that's left for this operand
     */
    operand_type_string_greedy,
    /**
     * absolute yaw angle in [-180.0;180.0) degree, or relative offset from '~'
     */
    operand_type_angle,
    /**
     * absolute integer block position, or relative offset from '~ ~ ~', or local offset from '^ ^ ^'
     */
    operand_type_block_pos,
    /**
     * block_id[block_states]{data_tags}, where 'block_id' is either a resource location or tag
     */
    operand_type_block_predicate,
    /**
     * block_id[block_states]{data_tags}, where 'block_id' is a resource location
     */
    operand_type_block_state,
    /**
     * one of the 16 chat color names, or 'reset'
     */
    operand_type_color,
    /**
     * absolute integer column position, or relative offset from '~ ~'
     */
    operand_type_column_pos,
    /**
     * raw json value
     */
    operand_type_component,
    /**
     * resource location
     */
    operand_type_dimension,
    /**
     * either a player name, target selector or uuid
     */
    operand_type_entity,
    /**
     * enum:
     * - eyes
     * - feet
     */
    operand_type_entity_anchor,
    /**
     * float value range, either '<exact value>', '<min value>...', '...<max value>' or '<min value>...<max value>', each inclusive
     */
    operand_type_float_range,
    /**
     * resource location or tag
     */
    operand_type_function,
    /**
     * TODO
     */
    operand_type_game_profile,
    /**
     * enum:
     * - survival
     * - creative
     * - adventure
     * - spectator
     */
    operand_type_gamemode,
    /**
     * enum:
     * - world_surface
     * - motion_blocking
     * - motion_blocking_no_leaves
     * - ocean_floor
     */
    operand_type_heightmap,
    /**
     * integer value range, either '<exact value>', '<min value>...', '...<max value>' or '<min value>...<max value>', each inclusive
     */
    operand_type_int_range,
    /**
     * item_id{data_tags}, where 'item_id' is either a resource location or tag
     */
    operand_type_item_predicate,
    /**
     * TODO
     */
    operand_type_item_slot,
    /**
     * item_id{data_tags}, where 'item_id' is a resource location
     */
    operand_type_item_stack,
    /**
     * plain text with spaces and target selectors
     */
    operand_type_message,
    /**
     * TODO
     */
    operand_type_nbt_compound_tag,
    /**
     * TODO
     */
    operand_type_nbt_path,
    /**
     * TODO
     */
    operand_type_nbt_tag,
    /**
     * objective name
     */
    operand_type_objective,
    /**
     * TODO
     */
    operand_type_objective_criteria,
    /**
     * TODO
     */
    operand_type_operation,
    /**
     * TODO
     */
    operand_type_particle,
    operand_type_resource,
    operand_type_resource_key,
    operand_type_resource_location,
    /**
     * either a resource location or tag
     */
    operand_type_resource_or_tag,
    /**
     * either a resource location or a tag
     */
    operand_type_resource_or_tag_key,
    /**
     * absolute two-element rotation, [yaw, pitch], in range [-180.0;180.0) and [-90.0;90.0] degree, or relative offset from '~ ~'
     */
    operand_type_rotation,
    /**
     * either a target selector, player name, uuid or '*' to select all score holders
     */
    operand_type_score_holder,
    /**
     * enum:
     * - list
     * - sidebar
     * - sidebar.team.<color>
     * - belowName
     */
    operand_type_scoreboard_slot,
    /**
     * non-repeating combination of 'x', 'y' and 'z' in any order, no duplicates
     */
    operand_type_swizzle,
    /**
     * team name, containing '-', '+', '.', '_', [A-Z], [a-z], [0-9]
     */
    operand_type_team,
    /**
     * enum:
     * - none
     * - front_back
     * - left_right
     */
    operand_type_template_mirror,
    /**
     * enum:
     * - none
     * - clockwise_90
     * - counterclockwise_90
     * - 180
     */
    operand_type_template_rotation,
    /**
     * floating point time value with unit suffix:
     * - d
     * - s
     * - t
     */
    operand_type_time,
    /**
     * hyphenated hexadecimal uuid (8-4-4-4-12)
     */
    operand_type_uuid,
    /**
     * absolute two-component floating point vector, or relative offset from '~ ~', or local offset from '^ ^'
     */
    operand_type_vec2,
    /**
     * absolute three-component floating point vector, or relative offset from '~ ~ ~', or local offset from '^ ^ ^'
     */
    operand_type_vec3,
};

std::map<std::string, std::vector<operand_type>> operand_map
{
    {
        "clear",
        {
            // targets
            operand_type_entity,
            // item
            operand_type_item_predicate,
            // maxCount
            operand_type_integer
        }
    },
    {
        // clone <begin> <end> <destination> [replace|masked] [force|move|normal]
        "clone",
        {
            // begin
            operand_type_block_pos,
            // end
            operand_type_block_pos,
            // destination
            operand_type_block_pos,
            // replace|masked
            operand_type_enum,
            // force|move|normal
            operand_type_enum
        }
    },
    {
        // clone from <sourceDimension> <begin> <end> <destination> [replace|masked] [force|move|normal]
        "clone.from",
        {
            // sourceDimension
            operand_type_dimension,
            // begin
            operand_type_block_pos,
            // end
            operand_type_block_pos,
            // destination
            operand_type_block_pos,
            // replace|masked
            operand_type_enum,
            // force|move|normal
            operand_type_enum
        }
    },
    {
        // clone <begin> <end> to <targetDimension> <destination> [replace|masked] [force|move|normal]
        "clone.to",
        {
            // begin
            operand_type_block_pos,
            // end
            operand_type_block_pos,
            // targetDimension
            operand_type_dimension,
            // destination
            operand_type_block_pos,
            // replace|masked
            operand_type_enum,
            // force|move|normal
            operand_type_enum
        }
    },
    {
        // clone from <sourceDimension> <begin> <end> to <targetDimension> <destination> [replace|masked] [force|move|normal]
        "clone.from.to",
        {
            // sourceDimension
            operand_type_dimension,
            // begin
            operand_type_block_pos,
            // end
            operand_type_block_pos,
            // targetDimension
            operand_type_dimension,
            // destination
            operand_type_block_pos,
            // replace|masked
            operand_type_enum,
            // force|move|normal
            operand_type_enum
        }
    },
    {
        // clone <begin> <end> <destination> filtered <filter> [force|move|normal]
        "clone.filtered",
        {
            // begin
            operand_type_block_pos,
            // end
            operand_type_block_pos,
            // destination
            operand_type_block_pos,
            // filter
            operand_type_block_predicate,
            // force|move|normal
            operand_type_enum
        }
    },
    {
        // clone from <sourceDimension> <begin> <end> <destination> filtered <filter> [force|move|normal]
        "clone.from.filtered",
        {
            // sourceDimension
            operand_type_dimension,
            // begin
            operand_type_block_pos,
            // end
            operand_type_block_pos,
            // destination
            operand_type_block_pos,
            // filter
            operand_type_block_predicate,
            // force|move|normal
            operand_type_enum
        }
    },
    {
        // clone <begin> <end> to <targetDimension> <destination> filtered <filter> [force|move|normal]
        "clone.to.filtered",
        {
            // begin
            operand_type_block_pos,
            // end
            operand_type_block_pos,
            // targetDimension
            operand_type_dimension,
            // destination
            operand_type_block_pos,
            // filter
            operand_type_block_predicate,
            // force|move|normal
            operand_type_enum
        }
    },
    {
        // clone from <sourceDimension> <begin> <end> to <targetDimension> <destination> filtered <filter> [force|move|normal]
        "clone.from.to.filtered",
        {
            // sourceDimension
            operand_type_dimension,
            // begin
            operand_type_block_pos,
            // end
            operand_type_block_pos,
            // targetDimension
            operand_type_dimension,
            // destination
            operand_type_block_pos,
            // filter
            operand_type_block_predicate,
            // force|move|normal
            operand_type_enum
        }
    },
    {
        // damage <target> <amount> [<damageType>] [at <location>]
        "damage.at",
        {
            // target
            operand_type_entity,
            // amount
            operand_type_float,
            // damageType
            operand_type_resource,
            // location
            operand_type_vec3
        }
    },
    {
        // damage <target> <amount> [<damageType>] [by <entity>] [from <cause>]
        "damage.by.from",
        {
            // target
            operand_type_entity,
            // amount
            operand_type_float,
            // damageType
            operand_type_resource,
            // entity
            operand_type_entity,
            // cause
            operand_type_entity
        }
    },
    {
        // deop <targets>
        "deop",
        {
            // targets
            operand_type_game_profile
        }
    },
    {
        // difficulty [peaceful|easy|normal|hard]
        "difficulty",
        {
            // peaceful|easy|normal|hard
            operand_type_enum
        }
    },
    {
        // effect clear [<targets>] [<effect>]
        "effect.clear",
        {
            // targets
            operand_type_entity,
            // effect
            operand_type_resource
        }
    },
    {
        // effect give <targets> <effect> [<seconds>] [<amplifier>] [<hideParticles>]
        "effect.give",
        {
            // targets
            operand_type_entity,
            // effect
            operand_type_resource,
            // seconds
            operand_type_integer,
            // amplifier
            operand_type_integer,
            // hideParticles
            operand_type_bool
        }
    },
    {
        // effect give <targets> <effect> infinite [<amplifier>] [<hideParticles>]
        "effect.give.infinite",
        {
            // targets
            operand_type_entity,
            // effect
            operand_type_resource,
            // amplifier
            operand_type_integer,
            // hideParticles
            operand_type_bool
        }
    },
    {
        // enchant <targets> <enchantment> [<level>]
        "enchant",
        {
            // targets
            operand_type_entity,
            // enchantment
            operand_type_resource,
            // level
            operand_type_integer
        }
    }
};

int main(int argc, const char **argv)
{
    // https://minecraft.fandom.com/wiki/Argument_types
    // https://minecraft.fandom.com/wiki/Commands
    // https://minecraft.fandom.com/wiki/Data_pack

    mcc::Actions actions(
        {
            {
                0,
                "help",
                "",
            },
            // mcc init [-name <package name>] [-version <package version>] [-description <package description>] -> initialize a new package with a given name, version and description
            {
                1,
                "init",
                "",
                {
                    {false, "-name", ""},
                    {false, "-version", ""},
                    {false, "-description", ""}
                }
            },
            // mcc compile [-pkg <package file>] [-target <target directory>] -> compile a package to a target directory
            {
                2,
                "compile",
                "",
                {
                    {false, "-pkg", ""},
                    {false, "-target", ""},
                }
            },
            // mcc package [-pkg <package file>] [-target <target directory>] [-destination <destination file>] -> package a package into a zip destination file
            {
                3,
                "package",
                "",
                {
                    {false, "-pkg", ""},
                    {false, "-target", ""},
                    {false, "-destination", ""},
                }
            },
        }
    );
    actions(argc, argv);

    std::ifstream stream("example/src/add.mcc");
    if (!stream)
        return 1;

    std::string namespace_ = "minecraft";

    mcc::Parser parser(stream, "example/src/add.mcc");
    while (parser)
        if (const auto statement = parser())
        {
            statement->Print(std::cout) << std::endl;

            if (auto ptr = dynamic_cast<mcc::NamespaceStatement *>(statement.get()))
            {
                namespace_ = ptr->GetID();
                continue;
            }

            if (auto ptr = dynamic_cast<mcc::DefineStatement *>(statement.get()))
            {
            }
        }

    stream.close();
    return 0;
}
