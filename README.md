# MCC - A Minecraft Command Compiler

## About

The MCC (Minecraft command compiler) allows you to write logic in a simple programming language with features like
control flow, variables etc. without having to think much about how that would look in bare-bones Minecraft commands.
Those work fine in a small, few command datapack, but for larger applications it gets really messy real fast. So that's
why I made ... this.

## Building

### Prerequisites

Before you start, make sure you have the following tools installed or available in your development environment:

- cmake (version >= 3.20)
- a project file generator like ninja or gnu make
- a toolchain like gcc (version >= 13) or clang

### Getting the Source and Building

To build the project, first get the repository and its dependencies:

```shell
git clone https://github.com/Scriptor25/MCC.git
cd MCC
git submodule update --recursive --init --remote
```

Next, still inside the repository directory, configure cmake:

```shell
cmake -S . -B build
```

If you want to use a different generator or toolchain, please refer to the official cmake documentation for help.

Now you can build the project:

```shell
cmake --build build
```

To use multiple threads at once for building in parallel, use the `-j` option:

```shell
cmake --build build -j 10
```

This will build and output the executable into the build directory. To test if that worked, run:

```shell
./build/mcc
```

You should get a warning that no valid action was specified.

## Included Example Project

The [example project](example) contains, like the name suggest, some example source files. This includes
some [math operations](example/src/math.mcc), some simple snippets
like [hello world](example/src/hello.mcc), [ping-pong](example/src/ping.mcc) or [gold](example/src/gold.mcc), that
were used to test several different functionalities, but most of those merged over time into a unified system. The most
complex but still simple example is [sphere](example/src/sphere.mcc), which generates a sphere for a given radius and
position in the world.

![three differently sized spheres floating over a flat world](docs/spheres.png)

The example shown in the screenshot are three spheres, one with radius 40 (the biggest one), 20 and 5. The one with 5
executed instantly, while the one with 20 took about 4 seconds and the 40 block radius took whole 40 seconds to appear.

## Usage

The tool has a builtin way of telling you how to use it, but anyway, here's how it works: The tool always takes an
action, multiple flags or options. There are currently four valid actions:

- `help`
- `init`
- `compile`
- `package`

```shell
mcc help
```

This prints out a list of all valid actions plus their valid arguments.

```shell
mcc init
```

Initializes a new empty project with an empty source directory and prepopulated package info file.

```shell
mcc compile
```

Compiles the source files in a project into the target directory.

```shell
mcc package
```

Compiles the source files in a project into the target directory and packages this directory as a zip datapack. **WIP**

## Language Syntax

### Basics

To get started with the MCC language syntax, you need to have some basic knowledge about how Minecraft commands work.
Even though it provides an extensive abstraction layer for control flow like if and for, or wrappers for commands like
print (uses tellraw), you still have to write a big chunk of Minecraft commands yourself. A very good resource for
learning how the Minecraft command system works is
the [Minecraft fandom wiki](https://minecraft.fandom.com/wiki/Commands); that's also the place where I gathered a lot of
information about commands for this project.

Now back to the language. First things first: Comments.

```mcc
/* this is a block comment */
/*
 *  this
 *  is
 *  a
 *  large
 *  block
 *  comment
 */
```

MCC's only type of comment is the block comment, known and loved by a lot of other languages and their users. You can
place them anywhere, as long as they don't disrupt a token, e.g. `he/* some comment */llo` is not valid, but
`hello /* some comment */ world` is.

### Top Level Statements

The top level statements of a source file allow for two actions: setting the current namespace and defining functions.

To set the current namespace just use the `namespace` in conjunction with the namespace identifier you want to use.
Example:

```mcc
namespace foo
```

The namespace identifier can consist of alphanumerical letters, dots and underscores. It must not start with a digit
though.

Function definitions are the even more important statement, as they represent the core feature of a datapack. To define
one, use the `define`
keyword, a resource location (can be absolute or relative to the current namespace) and a list of argument names
inbetween parenthesis. After those you can optionally specify `tags`, which will also be generated for the datapack.
Last but not least you can write the function body. It consists of a whitespace-separated list of statements and
expression. Here an example:

```mcc
define foo:bar() #some:tag, #some:other/tag {
    ...
    /* do something */
    ...
}
```

Here an example of a namespace-relative function definition:

```
define :bar() #some:tag, #:other/tag {
    ...
}
```

The function resource location get mixed by following rule: if the namespace (part before the colon) is empty, it gets
the currently set namespace. If it contains no colon at all, the namespace is the default namespace `minecraft`.

### Statements

Onto the next topic, statements. There are currently 4/5 statements available:

- multi
- if/unless
- for
- return

#### The Multi-Statement

... provides you with a way to write multiple statements in sequence, between curly braces. Example:

```mcc
{
    ...
    /* do something */
    ...
}
```

These are only useful in combination with if/unless, or for statements, as they only expect one statement in parsing.

#### The If/Unless-Statement

... is the main statement responsible for control flow. Like with any other programming language, it takes a condition,
and based on that condition does something, or if the condition is not met it does something else. The only specialty
here is the `unless` keyword. This is currently the only way to invert a condition, but that will be fixed in a future
version, so `unless` may be deprecated.

```mcc
if (condition)
    ... /* if condition is true, do something */
else
    ... /* else do something */
...

unless (condition)
    ... /* unless condition is true, do something */
else
    ... /* else do something */
```

#### The Advanced For-Loop-Statement

... is very much copied from <any language here>, with one exception, instead of using semicolon to separate the
operands, it uses commas:

```mcc
for (i = 0, i < 10, i = i + 1)
    ... /* do something 10 times */
```

#### The Return-Statement

... represents your only way out of the function apart from running it until it ends.

```mcc
return some_value
```

Note: you can only return results from other functions or number values. Strings, arrays, objects and anything else is
not supported by Minecraft command results! This may be fixed some day, when I decide to make my own proprietary return
value system, but that will break compatibility with the default system.

### Expressions

There are a few kind of expressions, but mainly there are binary, call, and primary expressions. Binary expression are
composed of a left operand, an operator and a right operand:

```mcc
foo = left + right * bar
```

Chained operations like here are parsed by precedence, so `right * bar` gets evaluated before `left + ...`, and the last
thing that happens is the assignment to `foo`.

Call expressions allow you to either call builtin functions by name or user defined ones by resource location:

```mcc
/* call predefined function */
print(@s, "Hello World!")

...

/* simple tellraw-wrapper function */
define :print(target, message) {
    ?`$tellraw $(target) $(message)`
}

...
/* call user defined function */
$:print({ target: @s, message: "Hello World!" })
```

The function callee for predefined functions is just the function identifier, and the arguments are passed as a
comma-separated list inbetween parenthesis. User-defined functions are identified using a valid resource location,
followed by an argument object.

The last group are primary expressions. Those are numbers, strings, format strings, etc.:

- boolean literal: `true`, `false`
- integer literal: `0`, `-123`, `51612452`
- float literal: `0.123`, `-5.62233`
- string literal: `"foo"`
- format string: `` ` `` `Hello ${name}!` `` ` `` (just pretend like the backticks are positioned correctly here)
- target literal: `@e[type=item,distance=..10]`, `@s`
- symbol literal: `foo`, `bar`
- resource literal: `$foo:bar`, `$foo`, `$:bar`
- inline command: `?` `` ` `` `tellraw @s Foo` `` ` ``
- array literal: `[ some, "items", 123 ]`
- object literal: `{ foo: "bar", x, y: $y:z, "name": "foobar" }`
- relative offset: `~`, `~10`, `~-3`
- local offset: `^`, `^10`, `^-3`
