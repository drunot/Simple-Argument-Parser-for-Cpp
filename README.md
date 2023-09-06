# Simple Argument Parser for C++

This is a simple argument parser for C++, that aims to give a easy way to generate a command line argument parser, that does error checking, type conversion and easy value retrieval. The argument parser automatically generates a help message the by default will be printed when the first argument is `-h` or `--help`. The library is designed to be easy to include in a project being a single header.

|                             |
| --------------------------- |
| [Basic usage](#basic-usage) |
| [License](#license)         |

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

## License

This project is under the MIT license.
