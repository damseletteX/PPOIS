#include <iostream>
#include "../include/set.h"

int main()
{
    Set s1("{a, b, c}");
    std::cout << "s1 = {a, b, c}\n";
    std::cout << "  мощность = " << s1.getCardinality() << "\n";
    std::cout << "  пусто? " << (s1.isEmpty() ? "да" : "нет") << "\n";
    std::cout << "  'a' принадлежит s1? " << (s1[Element(std::string("a"))] ? "да" : "нет") << "\n";
    std::cout << "  'z' принадлежит s1? " << (s1[Element(std::string("z"))] ? "да" : "нет") << "\n\n";

    Set s2("{b, c, d}");
    std::cout << "s2 = {b, c, d}\n";
    std::cout << "  мощность = " << s2.getCardinality() << "\n\n";

    Set unionSet = s1 + s2;
    std::cout << "s1 + s2 (объединение), мощность = " << unionSet.getCardinality()
              << " (ожидается 4: a, b, c, d)\n";

    Set interSet = s1 * s2;
    std::cout << "s1 * s2 (пересечение), мощность = " << interSet.getCardinality()
              << " (ожидается 2: b, c)\n";

    Set diffSet = s1 - s2;
    std::cout << "s1 - s2 (разность), мощность = " << diffSet.getCardinality()
              << " (ожидается 1: a)\n\n";

    Set nested("{a, {b, c}, {}}");
    std::cout << "nested = {a, {b, c}, {}}\n";
    std::cout << "  мощность = " << nested.getCardinality()
              << " (ожидается 3 — вложенное множество и {} считаются как ОДИН элемент каждое)\n";

    Set empty;
    std::cout << "\nempty = Set()\n";
    std::cout << "  пусто? " << (empty.isEmpty() ? "да" : "нет") << "\n";

    return 0;
}