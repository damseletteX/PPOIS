#pragma once
#include <string>
#include <vector>
#include <memory>

class Set;

/**
 * @brief Represents an element in a set, which can be either an atom (string) or a nested Set.
 */
class Element
{
    /** Flag indicating whether the element is a nested Set (true) or an atom (false). */
    bool isSet;

    /** Shared pointer to the nested Set (used if isSet is true). */
    std::shared_ptr<Set> subset;

    /** String representation of the atomic element (used if isSet is false). */
    std::string atom;

public:
    /**
     * @brief Constructs an Element containing a nested Set.
     * @param subset_ The Set to be stored inside this element.
     */
    explicit Element(const Set &subset_) : isSet(true),
                                           subset(std::make_shared<Set>(subset_)),
                                           atom("") {}

    /**
     * @brief Constructs an atomic Element.
     * @param atom_ The string value of the atomic element.
     */
    explicit Element(const std::string &atom_) : isSet(false), subset(nullptr), atom(atom_) {}

    /** Default destructor. */
    ~Element() = default;

    /**
     * @brief Checks equality between two elements (atomic values or nested sets).
     * @param other The element to compare with.
     * @return true if elements are equal, false otherwise.
     */
    bool operator==(const Element &other) const;

    /**
     * @brief Converts the element to its string representation.
     * @return std::string representation of the element.
     */
    std::string toString() const;
};

/**
 * @brief Represents a mathematical set of Elements.
 */
class Set
{
    /** Internal storage for set elements. */
    std::vector<Element> els;

    /**
     * @brief Parses a set string in bracket notation into set elements.
     * @param str String representation of the set (e.g., "{a, {b, c}}").
     */
    void parse(const std::string &str);

    /**
     * @brief Inserts an element into the set if it is not already present.
     * @param New The element to insert.
     * @return true if inserted, false if the element already exists.
     */
    bool insertUnique(const Element &New);

public:
    /** Default constructor. Creates an empty set. */
    Set() = default;

    /**
     * @brief Constructs a Set by parsing a string representation.
     * @param str String containing the set definition.
     */
    explicit Set(const std::string &str);

    /**
     * @brief Checks whether the set is empty.
     * @return true if empty, false otherwise.
     */
    bool isEmpty() const;

    /**
     * @brief Returns the cardinality (number of elements) of the set.
     * @return Number of elements in the set.
     */
    size_t getCardinality() const;

    /**
     * @brief Checks equality with another set.
     * @param other The set to compare with.
     * @return true if sets contain identical elements, false otherwise.
     */
    bool operator==(const Set &other) const;

    /**
     * @brief Adds an element to the set.
     * @param New The element to add.
     * @throws std::logic_error if the element is already present.
     */
    void add(const Element &New);

    /**
     * @brief Removes an element from the set.
     * @param byebye The element to remove.
     * @throws std::logic_error if the element is not found.
     */
    void rmv(const Element &byebye);

    /** Removes all elements from the set. */
    void clear();

    /**
     * @brief Checks whether an element exists in the set.
     * @param el_ The element to search for.
     * @return true if the element is present, false otherwise.
     */
    bool operator[](const Element &el_) const;

    /**
     * @brief Returns the union of two sets.
     * @param other The set to unite with.
     * @return A new Set containing all unique elements from both sets.
     */
    Set operator+(const Set &other) const;

    /**
     * @brief Computes the union with another set in place.
     * @param other The set to unite with.
     * @return Reference to this set.
     */
    Set &operator+=(const Set &other);

    /**
     * @brief Returns the intersection of two sets.
     * @param other The set to intersect with.
     * @return A new Set containing common elements.
     */
    Set operator*(const Set &other) const;

    /**
     * @brief Computes the intersection with another set in place.
     * @param other The set to intersect with.
     * @return Reference to this set.
     */
    Set &operator*=(const Set &other);

    /**
     * @brief Returns the relative difference (this \\ other).
     * @param other The set to subtract.
     * @return A new Set containing elements present in this set but not in other.
     */
    Set operator-(const Set &other) const;

    /**
     * @brief Computes the relative difference (this \\ other) in place.
     * @param other The set to subtract.
     * @return Reference to this set.
     */
    Set &operator-=(const Set &other);

    /**
     * @brief Generates the power set (boolean set containing all subsets).
     * @return A new Set containing all possible subsets as elements.
     * @throws std::length_error if the set cardinality is 63 or greater.
     */
    Set buildBoolean() const;

    /**
     * @brief Returns the string representation of the set.
     * @return Formatted std::string representation (e.g., "{a, b}").
     */
    std::string toString() const;
};