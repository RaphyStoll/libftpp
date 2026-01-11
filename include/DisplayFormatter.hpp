#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace libftpp {

class DisplayFormatter {
private:
    std::string _className;
    std::vector<std::string> _properties;

public:
    DisplayFormatter(std::string name);
    ~DisplayFormatter();
    
    // Template method must be in header
    template <typename T>
    DisplayFormatter& add(std::string key, const T& value) {
        std::stringstream ss;
        ss << key << ": " << value;
        _properties.push_back(ss.str());
        return *this;
    }

    std::string toString() const;

    // Static helper for headers (formerly pad_line)
    static std::string header(const std::string& title, int width = 40);
};

std::ostream& operator<<(std::ostream& os, const DisplayFormatter& formatter);

}

// Wrapper for backward compatibility (outside namespace)
std::string pad_line(const std::string& title);
