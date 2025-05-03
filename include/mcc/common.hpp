#pragma once

#include <format>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace mcc
{
    using IntegerT = long long;
    using FloatT = long double;
    using IndexT = unsigned long long;

    using Command = std::string;

    using StatementPtr = std::unique_ptr<struct Statement>;
    using ExpressionPtr = std::unique_ptr<struct Expression>;
    using FormatNodePtr = std::unique_ptr<struct FormatNode>;

    using TargetAttributePtr = std::shared_ptr<struct TargetAttribute>;

    using ValuePtr = std::shared_ptr<struct Value>;
    using ConstantPtr = std::shared_ptr<struct Constant>;
    using InstructionPtr = std::shared_ptr<struct Instruction>;

    enum CommandResultTypeE
    {
        CommandResultType_Value,
        CommandResultType_Storage,
        CommandResultType_Score,
    };

    struct CommandResult
    {
        CommandResultTypeE Type;

        /* Constant */
        std::string Value;

        /* Storage */
        std::string Path;

        /* Score */
        std::string Player;
        std::string Objective;
    };

    class CommandVector
    {
    public:
        explicit CommandVector(std::vector<Command> &commands)
            : m_Commands(commands)
        {
        }

        CommandVector &Append(Command command)
        {
            m_Commands.emplace_back(std::move(command));
            return *this;
        }

        template<typename... Args>
        CommandVector &Append(std::string_view format, Args &&... args)
        {
            return Append(std::vformat(std::move(format), std::make_format_args(args...)));
        }

    private:
        std::vector<Command> &m_Commands;
    };

    struct Context;
    class Builder;

    template<bool TAG>
    struct Resource
    {
        std::ostream &Print(std::ostream &stream) const
        {
            return stream << (TAG ? "#" : "") << Namespace << ':' << Path;
        }

        std::string String() const
        {
            return (TAG ? "#" : "") + Namespace + ':' + Path;
        }

        std::string Namespace;
        std::string Path;
    };

    using ResourceLocation = Resource<false>;
    using ResourceTag = Resource<true>;

    enum TargetSelectorE
    {
        TargetSelector_P,
        TargetSelector_R,
        TargetSelector_A,
        TargetSelector_E,
        TargetSelector_S,
    };

    enum GameModeE
    {
        GameMode_None,
        GameMode_Survival,
        GameMode_Creative,
        GameMode_Adventure,
        GameMode_Hardcore,
        GameMode_Spectator,
    };

    enum SortOrderE
    {
        SortOrder_None,
        SortOrder_Nearest,
        SortOrder_Furthest,
        SortOrder_Random,
        SortOrder_Arbitrary,
    };

    enum CalleeE
    {
        Callee_Advancement,
        Callee_Attribute,
        Callee_Ban,
        Callee_BanIP,
        Callee_BanList,
        Callee_BossBar,
        Callee_Clear,
        Callee_Clone,
        Callee_Damage,
        Callee_Data,
        Callee_Datapack,
        Callee_Debug,
        Callee_DefaultGameMode,
        Callee_DeOP,
        Callee_Difficulty,
        Callee_Effect,
        Callee_Enchant,
        Callee_Execute,
        Callee_Experience,
        Callee_Fill,
        Callee_FillBiome,
        Callee_ForceLoad,
        Callee_Function,
        Callee_GameMode,
        Callee_GameRule,
        Callee_Give,
        Callee_Help,
        Callee_Item,
        Callee_JFR,
        Callee_Kick,
        Callee_Kill,
        Callee_List,
        Callee_Locate,
        Callee_Loot,
        Callee_Me,
        Callee_Msg,
        Callee_OP,
        Callee_Pardon,
        Callee_PardonIP,
        Callee_Particle,
        Callee_Perf,
        Callee_Place,
        Callee_PlaySound,
        Callee_Publish,
        Callee_Random,
        Callee_Recipe,
        Callee_Reload,
        Callee_Return,
        Callee_Ride,
        Callee_SaveAll,
        Callee_SaveOff,
        Callee_SaveOn,
        Callee_Say,
        Callee_Schedule,
        Callee_Scoreboard,
        Callee_Seed,
        Callee_SetBlock,
        Callee_SetIdleTimeout,
        Callee_SetWorldSpawn,
        Callee_SpawnPoint,
        Callee_Spectate,
        Callee_SpreadPlayers,
        Callee_Stop,
        Callee_StopSound,
        Callee_Summon,
        Callee_Tag,
        Callee_Team,
        Callee_TeamMsg,
        Callee_Teleport,
        Callee_TellRaw,
        Callee_Time,
        Callee_Tick,
        Callee_Title,
        Callee_Trigger,
        Callee_Weather,
        Callee_Whitelist,
        Callee_WorldBorder,
    };

    enum ComparatorE
    {
        Comparator_None,
        Comparator_LT,
        Comparator_GT,
        Comparator_LE,
        Comparator_GE,
        Comparator_EQ,
        Comparator_NE,
    };

    enum OperatorE
    {
        Operator_None,
        Operator_Add,
        Operator_Sub,
        Operator_Mul,
        Operator_Div,
        Operator_Rem,
    };

    enum AllocationTypeE
    {
        AllocationType_Value,
        AllocationType_Array,
        AllocationType_Object,
    };

    enum ArrayOperationE
    {
        ArrayOperation_Append,
        ArrayOperation_Prepend,
        ArrayOperation_Insert,
    };

    inline TargetSelectorE ToTargetSelector(const std::string &selector_string)
    {
        static const std::map<std::string, TargetSelectorE> map
        {
            {"p", TargetSelector_P},
            {"r", TargetSelector_R},
            {"a", TargetSelector_A},
            {"e", TargetSelector_E},
            {"s", TargetSelector_S},
        };

        return map.at(selector_string);
    }

    inline CalleeE ToCallee(const std::string &callee_string)
    {
        static const std::map<std::string, CalleeE> map
        {
            {"advancement", Callee_Advancement},
            {"attribute", Callee_Attribute},
            {"ban", Callee_Ban},
            {"ban_ip", Callee_BanIP},
            {"banlist", Callee_BanList},
            {"bossbar", Callee_BossBar},
            {"clear", Callee_Clear},
            {"clone", Callee_Clone},
            {"damage", Callee_Damage},
            {"data", Callee_Data},
            {"datapack", Callee_Datapack},
            {"debug", Callee_Debug},
            {"defaultgamemode", Callee_DefaultGameMode},
            {"deop", Callee_DeOP},
            {"difficulty", Callee_Difficulty},
            {"effect", Callee_Effect},
            {"enchant", Callee_Enchant},
            {"execute", Callee_Execute},
            {"experience", Callee_Experience},
            {"fill", Callee_Fill},
            {"fillbiome", Callee_FillBiome},
            {"forceload", Callee_ForceLoad},
            {"function", Callee_Function},
            {"gamemode", Callee_GameMode},
            {"gamerule", Callee_GameRule},
            {"give", Callee_Give},
            {"help", Callee_Help},
            {"item", Callee_Item},
            {"jfr", Callee_JFR},
            {"kick", Callee_Kick},
            {"kill", Callee_Kill},
            {"list", Callee_List},
            {"locate", Callee_Locate},
            {"loot", Callee_Loot},
            {"me", Callee_Me},
            {"msg", Callee_Msg},
            {"op", Callee_OP},
            {"pardon", Callee_Pardon},
            {"pardon_ip", Callee_PardonIP},
            {"particle", Callee_Particle},
            {"perf", Callee_Perf},
            {"place", Callee_Place},
            {"playsound", Callee_PlaySound},
            {"publish", Callee_Publish},
            {"random", Callee_Random},
            {"recipe", Callee_Recipe},
            {"reload", Callee_Reload},
            {"return", Callee_Return},
            {"ride", Callee_Ride},
            {"save-all", Callee_SaveAll},
            {"save-off", Callee_SaveOff},
            {"save-on", Callee_SaveOn},
            {"say", Callee_Say},
            {"schedule", Callee_Schedule},
            {"scoreboard", Callee_Scoreboard},
            {"seed", Callee_Seed},
            {"setblock", Callee_SetBlock},
            {"setidletimeout", Callee_SetIdleTimeout},
            {"setworldspawn", Callee_SetWorldSpawn},
            {"spawnpoint", Callee_SpawnPoint},
            {"spectate", Callee_Spectate},
            {"spreadplayers", Callee_SpreadPlayers},
            {"stop", Callee_Stop},
            {"stopsound", Callee_StopSound},
            {"summon", Callee_Summon},
            {"tag", Callee_Tag},
            {"team", Callee_Team},
            {"teammsg", Callee_TeamMsg},
            {"teleport", Callee_Teleport},
            {"tell", Callee_Msg},
            {"tellraw", Callee_TellRaw},
            {"time", Callee_Time},
            {"tick", Callee_Tick},
            {"title", Callee_Title},
            {"tm", Callee_TeamMsg},
            {"tp", Callee_Teleport},
            {"trigger", Callee_Trigger},
            {"w", Callee_Msg},
            {"weather", Callee_Weather},
            {"whitelist", Callee_Whitelist},
            {"worldborder", Callee_WorldBorder},
            {"xp", Callee_Experience},
        };

        return map.at(callee_string);
    }

    inline const char *ToString(const TargetSelectorE &selector)
    {
        static const std::map<TargetSelectorE, const char *> map
        {
            {TargetSelector_P, "p"},
            {TargetSelector_R, "r"},
            {TargetSelector_A, "a"},
            {TargetSelector_E, "e"},
            {TargetSelector_S, "s"},
        };

        if (map.contains(selector))
            return map.at(selector);

        return "<undefined>";
    }

    inline const char *ToString(const CalleeE &callee)
    {
        static const std::map<CalleeE, const char *> map
        {
            {Callee_Advancement, "advancement"},
            {Callee_Attribute, "attribute"},
            {Callee_Ban, "ban"},
            {Callee_BanIP, "ban_ip"},
            {Callee_BanList, "banlist"},
            {Callee_BossBar, "bossbar"},
            {Callee_Clear, "clear"},
            {Callee_Clone, "clone"},
            {Callee_Damage, "damage"},
            {Callee_Data, "data"},
            {Callee_Datapack, "datapack"},
            {Callee_Debug, "debug"},
            {Callee_DefaultGameMode, "defaultgamemode"},
            {Callee_DeOP, "deop"},
            {Callee_Difficulty, "difficulty"},
            {Callee_Effect, "effect"},
            {Callee_Enchant, "enchant"},
            {Callee_Execute, "execute"},
            {Callee_Experience, "experience"},
            {Callee_Fill, "fill"},
            {Callee_FillBiome, "fillbiome"},
            {Callee_ForceLoad, "forceload"},
            {Callee_Function, "function"},
            {Callee_GameMode, "gamemode"},
            {Callee_GameRule, "gamerule"},
            {Callee_Give, "give"},
            {Callee_Help, "help"},
            {Callee_Item, "item"},
            {Callee_JFR, "jfr"},
            {Callee_Kick, "kick"},
            {Callee_Kill, "kill"},
            {Callee_List, "list"},
            {Callee_Locate, "locate"},
            {Callee_Loot, "loot"},
            {Callee_Me, "me"},
            {Callee_Msg, "msg"},
            {Callee_OP, "op"},
            {Callee_Pardon, "pardon"},
            {Callee_PardonIP, "pardon_ip"},
            {Callee_Particle, "particle"},
            {Callee_Perf, "perf"},
            {Callee_Place, "place"},
            {Callee_PlaySound, "playsound"},
            {Callee_Publish, "publish"},
            {Callee_Random, "random"},
            {Callee_Recipe, "recipe"},
            {Callee_Reload, "reload"},
            {Callee_Return, "return"},
            {Callee_Ride, "ride"},
            {Callee_SaveAll, "save-all"},
            {Callee_SaveOff, "save-off"},
            {Callee_SaveOn, "save-on"},
            {Callee_Say, "say"},
            {Callee_Schedule, "schedule"},
            {Callee_Scoreboard, "scoreboard"},
            {Callee_Seed, "seed"},
            {Callee_SetBlock, "setblock"},
            {Callee_SetIdleTimeout, "setidletimeout"},
            {Callee_SetWorldSpawn, "setworldspawn"},
            {Callee_SpawnPoint, "spawnpoint"},
            {Callee_Spectate, "spectate"},
            {Callee_SpreadPlayers, "spreadplayers"},
            {Callee_Stop, "stop"},
            {Callee_StopSound, "stopsound"},
            {Callee_Summon, "summon"},
            {Callee_Tag, "tag"},
            {Callee_Team, "team"},
            {Callee_TeamMsg, "teammsg"},
            {Callee_Teleport, "teleport"},
            {Callee_Msg, "tell"},
            {Callee_TellRaw, "tellraw"},
            {Callee_Time, "time"},
            {Callee_Tick, "tick"},
            {Callee_Title, "title"},
            {Callee_TeamMsg, "tm"},
            {Callee_Teleport, "tp"},
            {Callee_Trigger, "trigger"},
            {Callee_Msg, "w"},
            {Callee_Weather, "weather"},
            {Callee_Whitelist, "whitelist"},
            {Callee_WorldBorder, "worldborder"},
            {Callee_Experience, "xp"},
        };

        if (map.contains(callee))
            return map.at(callee);

        return "<undefined>";
    }

    inline std::ostream &operator<<(std::ostream &stream, const TargetSelectorE &e)
    {
        return stream << ToString(e);
    }

    inline std::ostream &operator<<(std::ostream &stream, const CalleeE &e)
    {
        return stream << ToString(e);
    }
}

namespace std
{
    template<>
    struct formatter<mcc::ResourceLocation> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::ResourceLocation &location, FormatContext &ctx) const
        {
            return formatter<string>::format(location.String(), ctx);
        }
    };

    template<>
    struct formatter<mcc::CalleeE> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const mcc::CalleeE &callee, FormatContext &ctx) const
        {
            return formatter<string>::format(mcc::ToString(callee), ctx);
        }
    };

    template<typename T>
    struct formatter<vector<T>> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const vector<T> &elements, FormatContext &ctx) const
        {
            string s = "[ ";

            for (unsigned i = 0; i < elements.size(); ++i)
            {
                if (i > 0)
                    s += ", ";
                s += std::format("{}", elements[i]);
            }

            s += " ]";

            return formatter<string>::format(s, ctx);
        }
    };
}
