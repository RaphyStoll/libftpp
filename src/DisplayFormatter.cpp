#include "DisplayFormatter.hpp"

namespace libftpp {

DisplayFormatter::DisplayFormatter(std::string name) : _className(name) {}

DisplayFormatter::~DisplayFormatter() {}

std::string DisplayFormatter::toString() const {
    std::stringstream ss;
    ss << _className << " { ";
    for (size_t i = 0; i < _properties.size(); ++i) {
        ss << _properties[i];
        if (i < _properties.size() - 1) {
            ss << ", ";
        }
    }
    ss << " }";
    return ss.str();
}

std::string DisplayFormatter::header(const std::string& title, int width) {
    std::string line;
    line += "+";
    line += std::string(width - 2, '-');
    line += "+";
    
    std::string header = line + "\n";
    header += "| ";
    header += title;
    
    int padding = width - 4 - title.size(); // -4 for "| " and " |"
    if (padding > 0) {
        header += std::string(padding, ' ');
    }
    
    header += " |";
    header += "\n";
    header += line;
    return header;
}

std::ostream& operator<<(std::ostream& os, const DisplayFormatter& formatter) {
    os << formatter.toString();
    return os;
}

}

// Wrapper for backward compatibility
std::string pad_line(const std::string& title) {
    return libftpp::DisplayFormatter::header(title);
}
