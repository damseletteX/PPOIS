#pragma once
#include <string>
#include <vector>

class CSet{ // unordered Cantor set
    std::vector<std::string> elem;
    void Add_el();
    bool Does_belong() const;
    void Parse(const std::string) const;
    public:
        bool Is_empty() const;
        int Det_cardinality() const;
        void Rmv_el();
        void Clear();
        CSet() = default;
        ~CSet() = default;
        CSet Unite(CSet&) const;
        CSet Intersect(CSet&) const;
        CSet Substract(CSet&) const;
        CSet Construct_PwrSet() const;
};

class PwrSet{
    std::vector<CSet> setElem;
    bool is
};
