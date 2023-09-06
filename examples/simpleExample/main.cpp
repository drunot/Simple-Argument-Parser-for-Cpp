#include"argparser.hpp"
#include<string>
#include<iostream>
#include<iomanip>
#include<cstdint>

struct parser : public argparser::Parser {
    const char* WelcomeMessage() const override {
        return "This program will print a message a number of times.\nHere are the possible settings:";
    }
    char** message = arg<char*>("msg", "m", "", "The message to print.", true);
    uint32_t* times = arg<uint32_t>("times", "t", 1, "The number of times the message is printed.");
    bool* printNum = arg<bool>("num", "n", false, "Print line numbers for the message.");
};

int main(int argc, char* argv[]) {
    parser p;
    std::vector<char*> arg_out;
    
    // Check for argument parse errors.
    if(!p.parse(argc, argv, &arg_out)) {
        // Handle errors.
        std::cout << p.GetErrorMessage() << "\n\n" << p.GetHelpMessage();
        return -1;
    };
    
    for(uint32_t i = 0; i < *p.times; i++) {
        if(*p.printNum) {
            std::cout << std::setw(3) << i + 1 << ": ";
        }
        
        std::cout << *p.message << "\n";
    }
    
    return 0;
}