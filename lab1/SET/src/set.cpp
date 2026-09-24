#include <iostream>
#include "../include/set.h"
#include <algorithm>

// ELEMENT METHODS
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

// SET METHODS
Set::Set()
{
    els.clear();
}

Set::Set(const std::string &set_)
{
    parse(set_);
}

Set Set::parse(const std::string &s)
{
    if(s[0]=='{' && s[s.length()-1=='}']) s.substr(1, s.size() - 2);
    else std::cout << "Set must start with a '{' and end with a '}'.\n";
    
    int depth = 0;
    for (int i = 0; i < s.length(); i++)
    {
        switch(s[i])
        {
        case '{':
            depth++;
            break;
        case '}':
            depth--;
            break;
        case ',' || ' ':
            break;
        default:
            
            break;
        }
    }
}

bool Set::isEmpty() const
{
    return els.empty();
}

size_t Set::getCardinality() const
{
    return els.size();
}

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

bool Set::operator[](const Element &el_) const
{
    for (const Element &elem : els)
    {
        if (el_ == elem)
            return true;
    }
    return false;
}

void Set::add(const Element &New)
{
    for (const Element &el : this->els)
    {
        if (el == New)
        {
            std::cout << "Element already in the set." << std::endl;
            return;
        }
    }
    this->els.push_back(New);
}

void Set::rmv(const Element &byebye)
{
    std::vector<Element>::iterator position = find(els.begin(), els.end(), byebye);
    if (position == els.end())
    {
        std::cout << "! Element not in set !\n";
        return;
    }
    else
    {
        els.erase(position);
    }
}

void Set::clear()
{
    els.clear();
    std::cout << "Set cleared of elements.\n";
}

Set Set::operator*(const Set &other) const
{
    Set intersection;
    for (const Element &myEl : els)
    {
        for (const Element &otherEl : other.els)
        {
            if (myEl == otherEl)
            {
                intersection.add(myEl);
            }
        }
    }
    return intersection;
}

Set Set::operator-(const Set &other) const
{
    Set difference = *this;
    for (const Element &otherEl : other.els)
    {
        difference.rmv(otherEl);
    }
    return difference;
}

Set Set::operator+(const Set &other) const
{
    Set united = *this;
    for (const Element &el : other.els)
    {
        united.add(el);
    }
    return united;
}

Set Set::buildBoolean() const
{
    Set boolean;

    return boolean;
}