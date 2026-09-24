#include <iostream>
#include "../include/set.h"
#include <algorithm>
#include <stack>

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
Set::Set(){}

Set::Set(const std::string &set_)
{
    parse(set_);
}

void Set::parse(const std::string &s)
{
        std::stack<Set> levels;
    std::string currentToken;
 
    for (char c : s)
    {
        if (std::isspace(static_cast<unsigned char>(c)))
        {
            continue;
        }
        else if (c == '{')
        {
            levels.push(Set());
        }
        else if (c == ',')
        {
            if (!currentToken.empty())
            {
                levels.top().add(Element(currentToken));
                currentToken.clear();
            }
        }
        else if (c == '}')
        {
            if (!currentToken.empty())
            {
                levels.top().add(Element(currentToken));
                currentToken.clear();
            }
 
            if (levels.empty())
            {
                std::cout << "! Несбалансированные скобки в строке множества !\n";
                return;
            }
 
            Set finished = levels.top();
            levels.pop();               
 
            if (levels.empty())
            {
                *this = finished;
            }
            else
            {
                levels.top().add(Element(finished));
            }
        }
        else
        {
            currentToken += c; 
        }
    }
 
    if (!levels.empty())
    {
        std::cout << "! Несбалансированные скобки в строке множества !\n";
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