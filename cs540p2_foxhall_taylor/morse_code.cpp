#include "Map.hpp"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cctype>


cs540::Map<char, std::string> morse {
    {',', "--..--"},
    {'.', ".-.-.-"},
    {'?', "..--.."},
    {'0', "----- "},
    {'1', ".---- "},
    {'2', "..--- "},
    {'3', "...-- "},
    {'4', "....- "},
    {'5', "..... "},
    {'6', "-.... "},
    {'7', "--... "},
    {'8', "---.. "},
    {'9', "----. "},
    {'A', ".-"},
    {'B', "-..."},
    {'C', "-.-."},
    {'D', "-.. "},
    {'E', "."},
    {'F', "..-."},
    {'G', "--."},
    {'H', "...."},
    {'I', ".."},
    {'J', ".---"},
    {'K', "-.-"},
    {'L', ".-.."},
    {'M', "--"},
    {'N', "-."},
    {'O', "---"},
    {'P', ".--."},
    {'Q', "--.-"},
    {'R', ".-."},
    {'S', "..."},
    {'T', "-"},
    {'U', "..-"},
    {'V', "...-"},
    {'W', ".--"},
    {'X', "-..-"},
    {'Y', "-.--"},
    {'Z', "--.."},
};


int main() {
    std::cout << "Enter messages to be translated (CTRL+D to exit)\n";

    std::string message;
    while(std::cin >> message) {
        for (auto c : message) {
            try {
                std::cout << morse.at(toupper(c)) << '\n';
            } catch (std::out_of_range) {
                std::cout << "invalid character: " << c << '\n';
            }
        }
    }

    return 0;
}
