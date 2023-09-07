# Simple Argument Parser for C++

This is a simple argument parser for C++, that aims to give a easy way to generate a command line argument parser, that does error checking, type conversion and easy value retrieval. The argument parser automatically generates a help message the by default will be printed when the first argument is `-h` or `--help`. The library is designed to be easy to include in a project being a single header.

## Basic usage

An example of how to create a parser is shown below. Here the parser is just inheriting form the parser class form the library. The `WelcomeMessage()` is overridden to change the welcome message used in the help message. All arguments are set with the `arg<T>(...)` function.

```cpp
struct parser : public argparser::Parser {
    const char* WelcomeMessage() const override {
        return "This program will print a message a number of times.\nHere are the possible settings:";
    }
    char** message = arg<char*>("msg", "m", "", "The message to print.", true);
    uint32_t* times = arg<uint32_t>("times", "t", 1, "The number of times the message is printed.");
    bool* printNum = arg<bool>("num", "n", false, "Print line numbers for the message.");
};
```

To populate the arguments from the class call the function parse on an instance of the class:

```cpp
int main(int argc, char* argv[]){
    parser p;
    p.parse(argc, argv);
}
```

To access the values simply use them dereferenced:

```cpp
int main(int argc, char* argv[]){
    ...
    for(uint32_t i = 0; i < *p.times; i++) {
        if(*p.printNum) {
            std::cout << std::setw(3) << i + 1 << ": ";
        }

        std::cout << *p.message << "\n";
    }
    ...
}
```

## The `Parser` class

The class to inherit from to create an argument parser.

| Members                                                                                                                                                                   | Descriptions                                                                                                                               |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| `public inline bool parse(int argc, char* argv, std::vector<char*>* out_arg)`                                                                                             | Parse the arguments received when main is called.                                                                                          |
| `public inline virtual const char * WelcomeMessage() const`                                                                                                               | Returns the welcome message printed with the help message. (Can be overridden.)                                                            |
| `public inline virtual const bool HelpEnabled() const`                                                                                                                    | Should the parser print help when '-h' and 'help' is called as the first argument. By default it always returns true. (Can be overridden.) |
| `public inline std::string GetHelpMessage()`                                                                                                                              | Get the help message for the parser.                                                                                                       |
| `public inline std::string GetErrorMessage() const`                                                                                                                       | Get the error messages if errors happened doing parsing.                                                                                   |
| `protected template<typename T>` <br/>`inline constexpr T* arg(const char* LongName, const char* ShortName, const T defaultValue, const char* helpMessage,bool required)` | Add an argument to the parser.                                                                                                             |

### Members

#### parse

```cpp
public inline bool parse(int argc,char * argv,std::vector< char * > * out_arg)
```

Parse the arguments received when main is called.

##### Parameters

- `argc` The argument count.

- `argv` The argument values.

- `out_arg` The arguments not consumed by the passer. (Ignored if nullptr)

##### Returns

- `true` Parsing happened without errors.

- `false` Errors occurred when parsing.

#### WelcomeMessage

```cpp
public inline virtual const char * WelcomeMessage() const
```

Returns the welcome message printed with the help message. (Can be overridden.)

##### Returns

const char\* The welcome message printed with the help message.

#### HelpEnabled

```cpp
public inline virtual const bool HelpEnabled() const
```

Should the parser print help when '-h' and 'help' is called as the first argument. By default it always returns true. (Can be overridden.)

##### Returns

- `true` Help will be printed.

- `false` '-h' and 'help' does nothing and can be used as normal arguments.

#### AllowUnknownArguments

```cpp
public inline virtual const bool AllowUnknownArguments() const
```

Should the parser report errors if an unknown arguments are given? By default it always returns false. (Can be overridden.)

##### Returns

true Unknown arguments will curse errors.

##### Returns

false Unknown arguments will not be reported as errors.

#### GetHelpMessage

```cpp
public inline std::string GetHelpMessage() const
```

Get the help message for the parser.

##### Returns

std::string The help message.

#### GetErrorMessage

```cpp
public inline std::string GetErrorMessage() const
```

Get the error messages if errors happened doing parsing.

##### Returns

std::string The error messages.

#### arg

```cpp
protected template<typename T>`
inline constexpr T * arg(const char * LongName,
                         const char * ShortName,
                         const T defaultValue,
                         const char * helpMessage,
                         bool required)
```

Add an argument to the parser.

##### Parameters

- `T` The type of the argument.

##### Parameters

- `LongName` The long name that specifies the argument. Ignored if empty string is given. (Called with two dashes before.)

- `ShortName` The short name that specifies the argument. Ignored if empty string is given. (Called with one dash before.)

- `defaultValue` The default value of the argument if the argument is not set. Default value is the type default constructor.

- `helpMessage` The help message that should be printed for this argument.

- `required` Should an error be reported if the argument is not given?

##### Returns

constexpr T\* A pointer to where the value will be stored after the parser has run.

## License

This project is under the MIT license.
