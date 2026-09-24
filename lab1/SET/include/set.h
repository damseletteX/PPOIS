#pragma once
#include <string>
#include <vector>
#include <memory>

class Set;
class Element
{
    bool isSet;
    std::shared_ptr<Set> subset;
    std::string atom;

public:
    Element(const Set &subset_) : isSet(true),
                                           subset(std::make_shared<Set>(subset_)),
                                           atom("") {}
    Element(const std::string &atom_) : isSet(false), subset(nullptr), atom(atom_) {}
    ~Element() {};
    bool operator==(const Element &other) const;
};

class Set
{
    std::vector<Element> els;
    Set parse(const std::string &);
    static std::string trim(const std::string &);
    std::vector<std::string> splitTop(const std::string &) const;

public:
    Set() {};
    Set(const std::string &set_) {}
    ~Set() {};
    bool isEmpty() const;
    size_t getCardinality() const;
    bool operator==(const Set &) const;
    void add(const Element &);
    void rmv(const Element &);
    void clear();

    // union
    Set operator+(const Set &) const;
    // difference
    Set operator-(const Set &) const;
    // intersection
    Set operator*(const Set &) const;
    // is element in set?
    bool operator[](const Element &) const;
    // boolean
    Set buildBoolean() const;
};