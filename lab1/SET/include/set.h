#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>

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

/**
 * @brief Orchestrates a collection of named Set objects.
 * @details Owns the mapping from a user-chosen name to a Set and exposes the
 *          same operations as Set itself, resolved by name. Contains no
 *          input/output of its own: callers (e.g. a CLI) are responsible for
 *          reading input and printing results. Errors (missing name, an
 *          element already present, etc.) are reported via exceptions, the
 *          same way Set itself reports them.
 */
class SetContainer
{
    /** Name -> Set storage. */
    std::map<std::string, Set> sets;

public:
    SetContainer() = default;

    /**
     * @brief Checks whether a set with the given name exists.
     * @param name The name to look up.
     * @return true if a set with this name exists, false otherwise.
     */
    bool exists(const std::string &name) const;

    /**
     * @brief Looks up a set by name (mutable access).
     * @param name The name to look up.
     * @return Reference to the stored Set.
     * @throws std::invalid_argument if no set with this name exists.
     */
    Set &get(const std::string &name);

    /**
     * @brief Looks up a set by name (read-only access).
     * @param name The name to look up.
     * @return Const reference to the stored Set.
     * @throws std::invalid_argument if no set with this name exists.
     */
    const Set &get(const std::string &name) const;

    /**
     * @brief Stores a set under the given name, creating or overwriting it.
     * @param name The name to store the set under.
     * @param value The set to store.
     * @return true if an existing set with this name was overwritten, false if newly created.
     */
    bool store(const std::string &name, const Set &value);

    /**
     * @brief Creates a new named set by parsing a string description.
     * @param name The name to store the set under.
     * @param description Bracket-notation description, e.g. "{a, b, {c}}".
     * @return true if an existing set with this name was overwritten, false if newly created.
     * @throws std::invalid_argument if the description is malformed.
     */
    bool createFromString(const std::string &name, const std::string &description);

    /**
     * @brief Adds an element to a named set.
     * @param name The name of the set to modify.
     * @param element The element to add.
     * @throws std::invalid_argument if no set with this name exists.
     * @throws std::logic_error if the element is already present.
     */
    void addElement(const std::string &name, const Element &element);

    /**
     * @brief Removes an element from a named set.
     * @param name The name of the set to modify.
     * @param element The element to remove.
     * @throws std::invalid_argument if no set with this name exists.
     * @throws std::logic_error if the element is not found.
     */
    void removeElement(const std::string &name, const Element &element);

    /**
     * @brief Returns the cardinality of a named set.
     * @param name The name of the set.
     * @throws std::invalid_argument if no set with this name exists.
     */
    size_t cardinality(const std::string &name) const;

    /**
     * @brief Checks whether a named set is empty.
     * @param name The name of the set.
     * @throws std::invalid_argument if no set with this name exists.
     */
    bool isEmpty(const std::string &name) const;

    /**
     * @brief Checks whether an element belongs to a named set.
     * @param name The name of the set.
     * @param element The element to check.
     * @throws std::invalid_argument if no set with this name exists.
     */
    bool isMember(const std::string &name, const Element &element) const;

    /**
     * @brief Checks whether two named sets are equal.
     * @throws std::invalid_argument if either name does not exist.
     */
    bool areEqual(const std::string &a, const std::string &b) const;

    /**
     * @brief Computes the union of two named sets without storing the result.
     * @throws std::invalid_argument if either name does not exist.
     */
    Set unite(const std::string &a, const std::string &b) const;

    /**
     * @brief Unites a named set with another, in place (a += b).
     * @throws std::invalid_argument if either name does not exist.
     */
    void uniteInPlace(const std::string &a, const std::string &b);

    /**
     * @brief Computes the intersection of two named sets without storing the result.
     * @throws std::invalid_argument if either name does not exist.
     */
    Set intersect(const std::string &a, const std::string &b) const;

    /**
     * @brief Intersects a named set with another, in place (a *= b).
     * @throws std::invalid_argument if either name does not exist.
     */
    void intersectInPlace(const std::string &a, const std::string &b);

    /**
     * @brief Computes the difference of two named sets without storing the result.
     * @throws std::invalid_argument if either name does not exist.
     */
    Set difference(const std::string &a, const std::string &b) const;

    /**
     * @brief Subtracts a named set from another, in place (a -= b).
     * @throws std::invalid_argument if either name does not exist.
     */
    void differenceInPlace(const std::string &a, const std::string &b);

    /**
     * @brief Builds the power set of a named set without storing the result.
     * @throws std::invalid_argument if the name does not exist.
     * @throws std::length_error if the set's cardinality is 63 or greater.
     */
    Set powerSet(const std::string &name) const;

    /**
     * @brief Checks whether the container holds no named sets at all.
     */
    bool empty() const;

    /**
     * @brief Returns the underlying name -> Set storage for read-only iteration
     *        (e.g. to list every stored set).
     */
    const std::map<std::string, Set> &all() const;
};