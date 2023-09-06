/**
 * @file argparser.hpp
 * @author Anton Vigen Smolarz (anton@smolarz.dk)
 * @brief Argument Parser Library
 * @version 1.0
 * @date 2023-09-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include<typeinfo>
#include<stdexcept>
#include<string>
#include<map>
#include<cstdlib>
#include<cstring>
#include<type_traits>
#include<sstream>
#include<iostream>
#include<iomanip>
#include<array>
#include<vector>

#ifndef B0C93573_F291_4C30_963A_579DFC3CA4B1
#define B0C93573_F291_4C30_963A_579DFC3CA4B1


//! Namespace for the argument parser library.
namespace argparser {
    //! Internal class used by the argument parser.
    class AnyTypeArg {
      public:
        /**
         * @brief Get the Type Info of the internal type.
         *
         * @return const std::type_info& The type info of the internal type.
         */
        virtual const std::type_info& getTypeInfo() const = 0;
        
        /**
         * @brief Get the Value. (If type of T does not match getTypeInfo() an error is thrown.)
         *
         * @tparam T    The type to return.
         * @return T*   The value of the type.
         */
        template<typename T>
        T* getValue() const {
            if(getTypeInfo() != typeid(T)) {
                throw std::runtime_error("Types does not match.");
            }
            
            return reinterpret_cast<T*>(_value);
        }
        
        /**
         * @brief Set the value from a string literal.
         *
         * @param value     The value that should be converted into the internal type.
         * @retval 0        value was *not* used to set the value. (E.g. booleans are by default just toggled.)
         * @retval 1        value was used to set the value.
         * @retval Other    value could not be converted into the internal type.
         */
        virtual int setValue(const char* value) = 0;
        
        /**
         * @brief Set the help message for the argument.
         *
         * @param message The help message.
         */
        void setHelpMessage(const char* message) {
            _helpMsg = std::string(message);
        }
        
        /**
         * @brief Get the help message for the argument.
         *
         * @return std::string The help message.
         */
        std::string getHelpMessage() const {
            return _helpMsg;
        }
        
        /**
         * @brief Set if the argument is required.
         *
         * @param required The argument is required if true. It is not otherwise.
         */
        void setRequired(bool required) {
            _required = required;
        }
        
        /**
         * @brief Get if the argument is required.
         *
         * @retval true     The argument is required.
         * @retval false    The argument is not required.
         */
        bool getRequired() const {
            return _required;
        }
        
        /**
         * @brief Get if this value was set when parsing.
         *
         * @retval true     This value was set when parsing.
         * @retval false    This value wasn't set when parsing.
         */
        bool wasValueSet() {
            return _set;
        }
        
        /**
         * @brief Get the error that occurred when setting the value.
         *
         * @return std::string The error message.
         */
        std::string getErrorMsg() const {
            return _errorMsg;
        }
      protected:
        //! Constructor is protected since AnyTypeArg should never exist without inheritance.
        AnyTypeArg() = default;
        void* _value;           //!< Pointer to the value
        std::string _helpMsg;   //!< The help message for the argument.
        std::string _errorMsg;  //!< The error message if setValue fails.
        bool _required;         //!< Is the argument required.
        bool _set = false;      //!< Was the value set doing parsing.
    };
    
    /**
     * @brief Class for handling arguments of specific types.
     *
     * @tparam T The type of the argument.
     */
    template<typename T>
    class TypeHandler: public AnyTypeArg {
      public:
        /**
         * @brief Default constructor.
         *
         */
        TypeHandler() {
            _value = new T();
        }
        /**
         * @brief Copy constructor.
         *
         * @param toCopy Object to copy.
         */
        TypeHandler(const TypeHandler& toCopy) {
            _value = new T();
            std::memcpy(_value, toCopy._value, sizeof(T));
        }
        /**
         * @brief Move constructor.
         *
         * @param toMove Object to move.
         */
        TypeHandler(TypeHandler&& toMove) {
            _value = toMove._value;
            toMove._value = new T();
        }
        
        /**
         * @brief Copy assignment.
         *
         * @param toCopy        Object to copy.
         * @return TypeHandler& The object it self.
         */
        TypeHandler& operator=(const TypeHandler& toCopy) {
            std::memcpy(_value, toCopy._value, sizeof(T));
            return &this;
        }
        
        /**
         * @brief Move assignment.
         *
         * @param toMove        Object to move.
         * @return TypeHandler& The object it self.
         */
        TypeHandler& operator=(TypeHandler&& toMove) {
            _value = toMove._value;
            toMove._value = new T();
            return &this;
        }
        
        /**
         * @brief Destructor. (If T is a pointer; it will be deleted with 'delete []')
         *
         */
        ~TypeHandler() {
            if(std::is_pointer<T>::value) {
                delete [] *reinterpret_cast<T*>(_value);
            }
            
            delete reinterpret_cast<T*>(_value);
        }
        
        /**
         * @brief Get the Type Info of T.
         *
         * @return const std::type_info& The type info T.
         */
        virtual const std::type_info& getTypeInfo() const override {
            return typeid(T);
        }
        
        /**
         * @brief Set the value of the object.
         *
         * @param value The new value of the object.
         */
        virtual void setValue(const T& value) {
            *(reinterpret_cast<T*>(_value)) = value;
        }
        
        /**
         * @brief Set the Value object from a string.
         *
         * @param value     The value that should be converted into the T.
         * @retval 0        value was *not* used to set the value. (E.g. booleans are by default just toggled.)
         * @retval 1        value was used to set the value.
         * @retval Other    value could not be converted into T.
         */
        virtual int setValue(const char* value) override {
            _set = true;
            return setValueChar(value);
        }
      private:
        /**
         * @brief Used to implement template specialisations of setValue.
         *
         * @param value     The value that should be converted into the T.
         * @retval 0        value was *not* used to set the value. (E.g. booleans are by default just toggled.)
         * @retval 1        value was used to set the value.
         * @retval Other    value could not be converted into T.
         */
        int setValueChar(const char* value);
    };
    
    //! The class to inherit from to create an argument parser.
    class Parser {
      public:
        /**
         * @brief Parse the arguments received when main is called.
         *
         * @param argc      The argument count.
         * @param argv      The argument values.
         * @param out_arg   The arguments not consumed by the passer. (Ignored if nullptr)
         * @retval true     Parsing happened without errors.
         * @retval false    Errors occurred when parsing.
         */
        bool parse(int argc, char* argv[], std::vector<char*>* out_arg = nullptr) {
            _errorMsg = "";
            bool error = false;
            
            if(HelpEnabled() && argc == 2 && (!std::strcmp(argv[1], "--help") || !std::strcmp(argv[1], "-h"))) {
                std::cout << GetHelpMessage();
                exit(0);
            }
            
            for(int i = 1; i < argc;) {
                auto anyValue = _argData.find(std::string(argv[i]));
                
                if(anyValue == _argData.end()) {
                    // If the value was not found add it to the outgoing arguments.
                    if(out_arg) {
                        out_arg->push_back(argv[i]);
                    }
                    
                    if(!AllowUnknownArguments()) {
                        error = true;
                        _errorMsg += _errorMsg == "" ? "" : "\n";
                        _errorMsg += "Unknown argument: " + std::string(argv[i]);
                    }
                    
                    // Skip the unknown argument
                    ++i;
                } else {
                    auto retVal = anyValue->second->setValue(argv[++i]);
                    
                    // Assume error if return value is not zero or one.
                    if(retVal < 0 || retVal > 1) {
                        error = true;
                        // TODO: Allow for better error reporting.
                        _errorMsg += _errorMsg == "" ? "" : "\n";
                        _errorMsg += "Error in argument: " + anyValue->first + ", " + anyValue->second->getErrorMsg();
                        ++i; // skip the argument for now.
                        continue;
                    }
                    
                    i += retVal;
                }
            }
            
            std::map<AnyTypeArg*, std::string> ReqSorter;
            
            for(auto pair : _argData) {
                if(pair.second->getRequired() && !pair.second->wasValueSet()) {
                    error = true;
                    
                    if(ReqSorter.find(pair.second) == ReqSorter.end()) {
                        ReqSorter[pair.second] = pair.first;
                    } else {
                        ReqSorter[pair.second] += " or " + pair.first;
                    }
                }
            }
            
            if(error && !ReqSorter.empty()) {
                _errorMsg += _errorMsg == "" ? "" : "\n";
                _errorMsg += "The following required arguments was not set: ";
                
                for(auto pair : ReqSorter) {
                    _errorMsg += pair.second + ", ";
                }
                
                _errorMsg = _errorMsg.substr(0, _errorMsg.length() - 2);
            }
            
            return !error;
        }
        
        /**
         * @brief Returns the welcome message printed with the help message. (Can be overridden.)
         *
         * @return const char* The welcome message printed with the help message.
         */
        virtual const char* WelcomeMessage() const {
            return "This are the arguments available for this program:";
        }
        
        /**
         * @brief Should the parser print help when '-h' and '--help' is called as the first argument. By default it always returns true. (Can be overridden.)
         *
         * @retval true     Help will be printed.
         * @retval false    '-h' and '--help' does nothing and can be used as normal arguments.
         */
        virtual const bool HelpEnabled() const {
            return true;
        }
        
        /**
         * @brief Should the parser report errors if an unknown arguments are given? By default it always returns false. (Can be overridden.)
         *
         * @return true     Unknown arguments will curse errors.
         * @return false    Unknown arguments will not be reported as errors.
         */
        virtual const bool AllowUnknownArguments() const {
            return false;
        }
        
        /**
         * @brief Get the help message for the parser.
         *
         * @return std::string The help message.
         */
        std::string GetHelpMessage() {
            std::map<AnyTypeArg*, std::array<std::string, 2>> Sorter;
            
            for(auto pair : _argData) {
                if(Sorter.find(pair.second) != Sorter.end()) {
                    Sorter[pair.second][0] = pair.first;
                } else {
                    Sorter[pair.second][1] = pair.first;
                }
            }
            
            std::stringstream ss;
            ss << WelcomeMessage() << "\n";
            
            for(auto pair : Sorter) {
                ss << std::setw(10) << pair.second[0] << " " << std::setw(10) << pair.second[1] << " : " << pair.first->getHelpMessage() << "\n";
            }
            
            return ss.str();
        }
        
        /**
         * @brief Get the error messages if errors happened doing parsing.
         *
         * @return std::string The error messages.
         */
        std::string GetErrorMessage() const {
            return _errorMsg;
        }
      protected:
      
        /**
         * @brief Add an argument to the parser.
         *
         * @tparam T            The type of the argument.
         * @param LongName      The long name that specifies the argument. Ignored if empty string is given. (Called with two dashes before.)
         * @param ShortName     The short name that specifies the argument. Ignored if empty string is given. (Called with one dash before.)
         * @param defaultValue  The default value of the argument if the argument is not set. Default value is the type default constructor.
         * @param helpMessage   The help message that should be printed for this argument.
         * @param required      Should an error be reported if the argument is not given?
         * @return constexpr T* A pointer to where the value will be stored after the parser has run.
         */
        template<typename T>
        constexpr T* arg(const char* LongName, const char* ShortName = "", const T defaultValue = T(), const char* helpMessage = "", bool required = false) {
            TypeHandler<T>* value = new TypeHandler<T>();
            value->setValue(defaultValue);
            value->setHelpMessage(helpMessage);
            value->setRequired(required);
            
            if(LongName[0] != 0) {
                _argData.insert({{"--" + std::string(LongName), value}});
            }
            
            if(ShortName[0] != 0) {
                _argData.insert({{"-" + std::string(ShortName), value}});
            }
            
            return value->template getValue<T>();
        }
        //! Constructor is protected since Parser should never exist without inheritance.
        Parser() = default;
      private:
        std::map<const std::string, AnyTypeArg*> _argData; //!< A map containing all the arguments to be used by the parser.
        std::string _errorMsg = ""; //!< The error messages if the parser created errors.
    };
    
    template<>
    int TypeHandler<std::string>::setValueChar(const char* value) {
        *(reinterpret_cast<std::string*>(_value)) = std::string(value);
        return 1;
    }
    
    template<>
    int TypeHandler<char*>::setValueChar(const char* value) {
        std::size_t string_size = std::strlen(value);
        *reinterpret_cast<char**>(_value) = new char[string_size + 1];
        (*reinterpret_cast<char**>(_value))[string_size] = 0;
        // Use secure string copy if MSVC is used to make MSVC shut up.
#ifdef _MSC_VER
        strncpy_s(*reinterpret_cast<char**>(_value), string_size + 1, value, string_size);
#else
        std::strncpy(*reinterpret_cast<char**>(_value), value, string_size);
#endif
        return 1;
    }
    
    template<>
    int TypeHandler<int>::setValueChar(const char* value) {
        char* end;
        *(reinterpret_cast<int*>(_value)) = std::strtol(value, &end, 10);
        
        if(end == value || *end != 0) {
            _errorMsg = "\"" + std::string(value) + "\" is not an integer.";
            return -1;
        }
        
        return 1;
    }
    
    template<>
    int TypeHandler<long>::setValueChar(const char* value) {
        char* end;
        *(reinterpret_cast<long*>(_value)) = std::strtol(value, &end, 10);
        
        if(end == value || *end != 0) {
            _errorMsg = "\"" + std::string(value) + "\" is not an integer.";
            return -1;
        }
        
        return 1;
    }
    
    template<>
    int TypeHandler<long long>::setValueChar(const char* value) {
        char* end;
        *(reinterpret_cast<long long*>(_value)) = std::strtoll(value, &end, 10);
        
        if(end == value || *end != 0) {
            _errorMsg = "\"" + std::string(value) + "\" is not an integer.";
            return -1;
        }
        
        return 1;
    }
    
    template<>
    int TypeHandler<unsigned int>::setValueChar(const char* value) {
        char* end;
        *(reinterpret_cast<unsigned int*>(_value)) = std::strtoul(value, &end, 10);
        
        if(end == value || *end != 0) {
            _errorMsg = "\"" + std::string(value) + "\" is not a positive integer.";
            return -1;
        }
        
        return 1;
    }
    
    template<>
    int TypeHandler<unsigned long>::setValueChar(const char* value) {
        char* end;
        *(reinterpret_cast<unsigned long*>(_value)) = std::strtoul(value, &end, 10);
        
        if(end == value || *end != 0) {
            _errorMsg = "\"" + std::string(value) + "\" is not a positive integer.";
            return -1;
        }
        
        return 1;
    }
    
    template<>
    int TypeHandler<unsigned long long>::setValueChar(const char* value) {
        char* end;
        *(reinterpret_cast<unsigned long long*>(_value)) = std::strtoull(value, &end, 10);
        
        if(end == value || *end != 0) {
            _errorMsg = "\"" + std::string(value) + "\" is not a positive integer.";
            return -1;
        }
        
        return 1;
    }
    
    template<>
    int TypeHandler<float>::setValueChar(const char* value) {
        char* end;
        *(reinterpret_cast<float*>(_value)) = std::strtof(value, &end);
        
        if(end == value || *end != 0) {
            _errorMsg = "\"" + std::string(value) + "\" is not a number.";
            return -1;
        }
        
        return 1;
    }
    
    template<>
    int TypeHandler<double>::setValueChar(const char* value) {
        char* end;
        *(reinterpret_cast<double*>(_value)) = std::strtof(value, &end);
        
        if(end == value || *end != 0) {
            _errorMsg = "\"" + std::string(value) + "\" is not a number.";
            return -1;
        }
        
        return 1;
    }
    
    template<>
    int TypeHandler<bool>::setValueChar(const char* value) {
        *(reinterpret_cast<bool*>(_value)) = !*(reinterpret_cast<bool*>(_value));
        return 0;
    }
    
    template<>
    int TypeHandler<char>::setValueChar(const char* value) {
        *(reinterpret_cast<char*>(_value)) = value[0];
        return 0;
    }
}

#endif /* B0C93573_F291_4C30_963A_579DFC3CA4B1 */
