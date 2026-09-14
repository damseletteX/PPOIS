#include <iostream>
#include "set.h"

//     элементом множества может быть другое множество;
//     проверка на пустое множество;
//     добавление элемента;
//     удаление элемента;
//     определение мощности множества;
//     проверка принадлежности элемента множеству ([]);
//     объединение двух множеств (+, +=);
//     пересечение двух множеств (*, *=);
//     разность двух множеств (-, -=);
//     построение булеана (множества всех подмножеств) данного множества.
// Описать класс «Неориентированное канторовское множество»
//  (элементы не повторяются и не упорядочены).
//  Класс должен дополнительно
// реализовывать формирование множества из строки
// (например, {a, b, c, {a, b}, {}, {a, {c}}}).

// ELEMENT METHODS
bool Element::elcmp(const Element &other) const
{
    if (other.isSet != this->isSet)
        return false;
    else if (other.isSet == this->isSet == false)
    {
        return other.atom == this->atom;
    }
    else
    { 
        
    }
}

// SET METHODS
void Set::add(Element New)
{
    if (elcmp(New))
    {
        std::cout << "Element already in the set." << std::endl;
        return;
    }
    this->els.push_back(New);
}

int const Set::getCardinality()
{
    return size(this->els);
}


int main()
{

    return 0;
}