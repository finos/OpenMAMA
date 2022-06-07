#ifndef CommonStrUtilsCppH__
#define CommonStrUtilsCppH__

#include <vector>
#include <string>

namespace Wombat {

class COMMONExpDLL CommonStrUtils
{
public:
    /**
     * Removes leading and trailing whitespace from the given string. This is
     * a non-destructive operation
     * @param s The original string to examine
     * @param t The optional list of characters to trim
     * @return The trimmed string.
     */
    static std::string& trim(std::string& s, const char* t = " \t\n\r\f\v");

    /**
     * Removes leading whitespace from the given string. This is
     * a non-destructive operation
     * @param s The original string to examine
     * @param t The optional list of characters to trim
     * @return The trimmed string.
     */
    static std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");

    /**
     * Removes trailing whitespace from the given string. This is
     * a non-destructive operation
     * @param s The original string to examine
     * @param t The optional list of characters to trim
     * @return The trimmed string.
     */
    static std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v");

    /**
     * Splits a given string according to the given delimiter and returns a
     * vector of strings containing each component separated into separate
     * string objects.
     * @param input The string to split
     * @param delim The character or sequence of characters to split according
     *              to.
     * @return A vector containing each component of the split string
     */
    static std::vector<std::string> split(const char* input, const char* delim);
};
}
#endif
