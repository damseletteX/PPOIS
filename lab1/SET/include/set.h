#pragma once
#include <string>
#include <vector>
#include <memory>

class Set;
// unordered Cantor set
//     {std::vector<std::string> elem;

//     void Add_el();
//     bool Does_belong() const;
//     void Parse(const std::string) const;

//     public:
//         bool Is_empty() const;
//         int Det_cardinality() const;
//         void Rmv_el();
//         void Clear();
//         Set() = default;
//         ~Set() = default;
//         Set Unite(Set&) const;
//         Set Intersect(Set&) const;
//         Set Substract(Set&) const;
//         Set Construct_PwrSet() const;
// };
class Element
{
    bool isSet;
    std::shared_ptr<Set> subset;
    std::string atom;

public:
    explicit Element(const Set &subset_) : isSet(true),
                                           subset(std::make_shared<Set>(subset_)),
                                           atom("") {}
    explicit Element(const std::string &atom_) : isSet(false), subset(nullptr), atom(atom_) {}
    ~Element();
    bool elcmp(const Element &other) const;
};

class Set
{
    std::vector<Element> els;

public:
    explicit Set();
    ~Set();
    void add(Element);
    void rmv(Element);
    int const getCardinality();
    void Clear();
};