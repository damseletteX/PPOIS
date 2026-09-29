#include "../include/set.h"
#include <algorithm>
#include <stack>
#include <stdexcept>
#include <utility>

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

std::string Element::toString() const
{
    return isSet ? subset->toString() : atom;
}

// SET METHODS
Set::Set(const std::string &set_)
{
    parse(set_);
}

// Разбор строки через стек: levels.top() — множество, которое строится на
// текущем уровне вложенности. '{' открывает новый уровень, '}' закрывает:
// последний уровень становится результатом, остальные — элементами родителя.
// Результат присваивается только в конце, поэтому при ошибке объект не создаётся.
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

bool Set::insertUnique(const Element &New)
{
    if ((*this)[New])
        return false;
    els.push_back(New);
    return true;
}

void Set::add(const Element &New)
{
    if (!insertUnique(New))
        throw std::logic_error("Element already in set.");
}

void Set::rmv(const Element &byebye)
{
    std::vector<Element>::iterator position = std::find(els.begin(), els.end(), byebye);
    if (position == els.end())
        throw std::logic_error("Such element not found.");
    els.erase(position);
}

void Set::clear()
{
    els.clear();
}

// Объединение. Повторы — обычная ситуация, поэтому insertUnique, а не add.
// При other == *this ничего не добавляется, обход безопасен.
Set &Set::operator+=(const Set &other)
{
    for (const Element &el : other.els)
        insertUnique(el);
    return *this;
}

// Пересечение и разность собирают результат в отдельный вектор и подменяют els
// в конце, поэтому корректны и при other == *this.
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

Set Set::operator+(const Set &other) const
{
    Set result = *this;
    result += other;
    return result;
}

Set Set::operator*(const Set &other) const
{
    Set result = *this;
    result *= other;
    return result;
}

Set Set::operator-(const Set &other) const
{
    Set result = *this;
    result -= other;
    return result;
}

// Булеан без рекурсии: число mask от 0 до 2^N - 1 задаёт подмножество,
// i-й бит == 1 значит "i-й элемент входит". Элементы уникальны, поэтому
// подмножества заведомо различны и кладутся напрямую, минуя проверку.
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