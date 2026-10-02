#include "set.h"
#include <algorithm>
#include <stack>
#include <stdexcept>
#include <utility>

// ============================================================================
// ELEMENT METHODS
// ============================================================================

/**
 * @brief Checks equality between two elements (atomic or nested set).
 * @param other The element to compare with.
 * @return true if both elements are of the same type and have equal values, false otherwise.
 */
bool Element::operator==(const Element &other) const
{
    if (other.isSet != this->isSet)
        return false;
    else if (other.isSet == false)
    {
        return other.atom == this->atom;
    }
    else if (other.subset->getCardinality() == subset->getCardinality())
    {
        return *this->subset == *other.subset;
    }
    return false;
}

/**
 * @brief Converts the element to its string representation.
 * @return std::string formatted representation of the atom or nested set.
 */
std::string Element::toString() const
{
    return isSet ? subset->toString() : atom;
}

// ============================================================================
// SET METHODS
// ============================================================================

/**
 * @brief Constructs a Set by parsing a string representation.
 * @param set_ String containing the set structure (e.g., "{a, {b, c}}").
 */
Set::Set(const std::string &set_)
{
    parse(set_);
}

/**
 * @brief Parses a string in bracket notation into set elements using a stack.
 * @details '{' starts a new nested level, and '}' closes the current level.
 *          If a parsing error occurs, an exception is thrown without modifying the object.
 * @param s The input string to parse.
 * @throws std::invalid_argument If brackets are unbalanced or syntax is invalid.
 */
void Set::parse(const std::string &s)
{
    const char *bad = "Invalid set string: check the brackets and stray characters.";
    std::stack<Set> lvls;
    std::string curElement;
    Set ready;
    Set result;
    for (char c : s)
    {
        switch (c)
        {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            break;
        case '{':
            lvls.push(Set());
            break;
        case '}':
            if (lvls.empty())
                throw std::invalid_argument(bad);
            if (!curElement.empty())
                lvls.top().insertUnique(Element(curElement));
            curElement.clear();
            ready = lvls.top();
            lvls.pop();
            if (lvls.empty())
            {
                result = ready;
            }
            else
            {
                lvls.top().insertUnique(Element(ready));
            }
            break;
        case ',':
            if (lvls.empty())
                throw std::invalid_argument(bad);
            if (!curElement.empty())
            {
                lvls.top().insertUnique(Element(curElement));
                curElement.clear();
            }
            break;
        default:
            if (lvls.empty())
                throw std::invalid_argument(bad);
            curElement += c;
            break;
        }
    }

    if (!lvls.empty())
        throw std::invalid_argument(bad);
    *this = result;
}

/**
 * @brief Converts the set into a formatted string.
 * @return std::string representation of the set in "{elem1, elem2}" format.
 */
std::string Set::toString() const
{
    std::string out = "{";
    for (size_t i = 0; i < els.size(); ++i)
    {
        if (i > 0)
            out += ", ";
        out += els[i].toString();
    }
    return out + "}";
}

/**
 * @brief Checks whether the set is empty.
 * @return true if the set contains no elements, false otherwise.
 */
bool Set::isEmpty() const
{
    return els.empty();
}

/**
 * @brief Returns the cardinality (number of elements) of the set.
 * @return size_t number of elements.
 */
size_t Set::getCardinality() const
{
    return els.size();
}

/**
 * @brief Compares two sets for mathematical equality.
 * @param other The set to compare with.
 * @return true if sets contain the exact same elements regardless of order, false otherwise.
 */
bool Set::operator==(const Set &other) const
{
    if (this->els.size() != other.els.size())
        return false;
    size_t flag = 0;
    for (const Element &myEl : this->els)
    {
        for (const Element &otherEl : other.els)
        {
            if (myEl == otherEl)
                flag++;
        }
    }
    return flag == getCardinality();
}

/**
 * @brief Checks if a specific element belongs to the set.
 * @param el_ The element to search for.
 * @return true if the element is present, false otherwise.
 */
bool Set::operator[](const Element &el_) const
{
    for (const Element &elem : els)
    {
        if (el_ == elem)
            return true;
    }
    return false;
}

/**
 * @brief Inserts an element into the set if it is not already present.
 * @param New The element to insert.
 * @return true if inserted successfully, false if the element already exists.
 */
bool Set::insertUnique(const Element &New)
{
    if ((*this)[New])
        return false;
    els.push_back(New);
    return true;
}

/**
 * @brief Adds an element to the set.
 * @param New The element to add.
 * @throws std::logic_error If the element is already present in the set.
 */
void Set::add(const Element &New)
{
    if (!insertUnique(New))
        throw std::logic_error("Element already in set.");
}

/**
 * @brief Removes an element from the set.
 * @param byebye The element to remove.
 * @throws std::logic_error If the element is not found in the set.
 */
void Set::rmv(const Element &byebye)
{
    std::vector<Element>::iterator position = std::find(els.begin(), els.end(), byebye);
    if (position == els.end())
        throw std::logic_error("Such element not found.");
    els.erase(position);
}

/**
 * @brief Clears all elements from the set.
 */
void Set::clear()
{
    els.clear();
}

/**
 * @brief Performs set union with another set in place.
 * @param other The set to unite with.
 * @return Reference to this modified set.
 */
Set &Set::operator+=(const Set &other)
{
    for (const Element &el : other.els)
        insertUnique(el);
    return *this;
}

/**
 * @brief Performs set intersection with another set in place.
 * @param other The set to intersect with.
 * @return Reference to this modified set.
 */
Set &Set::operator*=(const Set &other)
{
    std::vector<Element> kept;
    for (const Element &el : els)
    {
        if (other[el])
            kept.push_back(el);
    }
    els = std::move(kept);
    return *this;
}

/**
 * @brief Performs relative difference (this \\ other) in place.
 * @param other The set to subtract.
 * @return Reference to this modified set.
 */
Set &Set::operator-=(const Set &other)
{
    std::vector<Element> kept;
    for (const Element &el : els)
    {
        if (!other[el])
            kept.push_back(el);
    }
    els = std::move(kept);
    return *this;
}

/**
 * @brief Computes the union of two sets.
 * @param other The right-hand side set.
 * @return A new Set object representing the union.
 */
Set Set::operator+(const Set &other) const
{
    Set result = *this;
    result += other;
    return result;
}

/**
 * @brief Computes the intersection of two sets.
 * @param other The right-hand side set.
 * @return A new Set object representing the intersection.
 */
Set Set::operator*(const Set &other) const
{
    Set result = *this;
    result *= other;
    return result;
}

/**
 * @brief Computes the relative difference of two sets.
 * @param other The right-hand side set.
 * @return A new Set object representing the difference (this \\ other).
 */
Set Set::operator-(const Set &other) const
{
    Set result = *this;
    result -= other;
    return result;
}

/**
 * @brief Generates the power set (set of all subsets) using bitwise masks.
 * @details Each bit from 0 to 2^N - 1 indicates whether the corresponding element is included.
 * @return A new Set containing all subsets as Element objects.
 * @throws std::length_error If cardinality is 63 or greater to prevent integer overflow.
 */
Set Set::buildBoolean() const
{
    const size_t N = els.size();

    if (N >= 63)
        throw std::length_error("Set is too large to build a boolean.");

    const unsigned long long total = 1ULL << N;
    Set boolean;
    for (unsigned long long mask = 0; mask < total; ++mask)
    {
        Set subset;
        for (size_t i = 0; i < N; ++i)
        {
            if (mask & (1ULL << i))
                subset.els.push_back(els[i]);
        }
        boolean.els.push_back(Element(subset));
    }
    return boolean;
}