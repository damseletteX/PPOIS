/** @brief Set console interface */

#include <iostream>
#include <exception>
#include <map>
#include <string>
#include "set.h"

namespace
{

std::map<std::string, Set> sets;

std::string trim(const std::string &s)
{
    const char *ws = " \t\n\r";
    size_t b = s.find_first_not_of(ws);
    if (b == std::string::npos)
        return "";
    size_t e = s.find_last_not_of(ws);
    return s.substr(b, e - b + 1);
}

void checkName(const std::string &name)
{
    if (name.empty() || name.find_first_of(" \t") != std::string::npos)
        throw std::invalid_argument("Имя не должно быть пустым и не должно содержать пробелов.");
}

Element parseElement(const std::string &raw)
{
    std::string t = trim(raw);
    if (t.empty())
        throw std::invalid_argument("Элемент не может быть пустым.");
    if (t.front() == '{')
        return Element(Set(t));
    if (t.find_first_of("{}, \t") != std::string::npos)
        throw std::invalid_argument("Атом не должен содержать символы { } , и пробелы.");
    return Element(t);
}

std::string ask(const std::string &question)
{
    std::cout << question;
    std::string line;
    std::getline(std::cin, line);
    return trim(line);
}

Set &getSet(const std::string &name)
{
    auto it = sets.find(name);
    if (it == sets.end())
        throw std::invalid_argument("Множество '" + name + "' не найдено.");
    return it->second;
}

void printSet(const std::string &name, const Set &s)
{
    std::cout << name << " = " << s.toString() << "  (мощность " << s.getCardinality() << ")\n";
}

void createNewSet()
{
    std::string name = ask("Имя нового множества: ");
    checkName(name);
    std::string text = ask("Множество (например {a, b, {c}}): ");
    if (text.empty())
        throw std::invalid_argument("Строка множества пуста.");
    Set s(text);
    bool existed = sets.count(name) > 0;
    sets.insert_or_assign(name, s);
    std::cout << (existed ? "Перезаписано: " : "Создано: ") << name << " = " << s.toString() << "\n";
}

void addElement()
{
    Set &s = getSet(ask("Имя множества: "));
    Element e = parseElement(ask("Элемент (атом или {множество}): "));
    s.add(e);
    std::cout << "Элемент добавлен. Теперь: " << s.toString() << "\n";
}

void removeElement()
{
    Set &s = getSet(ask("Имя множества: "));
    Element e = parseElement(ask("Элемент (атом или {множество}): "));
    s.rmv(e);
    std::cout << "Элемент удалён. Теперь: " << s.toString() << "\n";
}

void printSetCardinality()
{
    std::string name = ask("Имя множества: ");
    std::cout << "Мощность " << name << ": " << getSet(name).getCardinality() << "\n";
}

void checkElementMembership()
{
    std::string name = ask("Имя множества: ");
    const Set &s = getSet(name);
    Element e = parseElement(ask("Элемент (атом или {множество}): "));
    std::cout << "Элемент " << (s[e] ? "принадлежит" : "не принадлежит") << " множеству " << name << ".\n";
}

void performUnion()
{
    Set &a = getSet(ask("Первое множество: "));
    const Set &b = getSet(ask("Второе множество: "));
    std::string resultName = ask("Имя для результата (пусто — изменить первое множество на месте): ");
    if (resultName.empty())
    {
        a += b;
        printSet("Результат", a);
        return;
    }
    checkName(resultName);
    Set r = a + b;
    sets.insert_or_assign(resultName, r);
    printSet(resultName, r);
}

void performIntersection()
{
    Set &a = getSet(ask("Первое множество: "));
    const Set &b = getSet(ask("Второе множество: "));
    std::string resultName = ask("Имя для результата (пусто — изменить первое множество на месте): ");
    if (resultName.empty())
    {
        a *= b;
        printSet("Результат", a);
        return;
    }
    checkName(resultName);
    Set r = a * b;
    sets.insert_or_assign(resultName, r);
    printSet(resultName, r);
}

void performDifference()
{
    Set &a = getSet(ask("Первое множество: "));
    const Set &b = getSet(ask("Второе множество: "));
    std::string resultName = ask("Имя для результата (пусто — изменить первое множество на месте): ");
    if (resultName.empty())
    {
        a -= b;
        printSet("Результат", a);
        return;
    }
    checkName(resultName);
    Set r = a - b;
    sets.insert_or_assign(resultName, r);
    printSet(resultName, r);
}

const size_t kMaxBooleanSize = 20; // 2^20 подмножеств — предел разумного для консоли

void printPowerSet()
{
    Set &s = getSet(ask("Имя множества: "));
    if (s.getCardinality() > kMaxBooleanSize)
        throw std::invalid_argument("Множество слишком велико для булеана (максимум " +
                                    std::to_string(kMaxBooleanSize) + " элементов).");
    std::string resultName = ask("Имя для булеана: ");
    checkName(resultName);
    Set b = s.buildBoolean();
    sets.insert_or_assign(resultName, b);
    printSet(resultName, b);
}

void printAllSets()
{
    if (sets.empty())
    {
        std::cout << "Множеств пока нет.\n";
        return;
    }
    for (const auto &kv : sets)
        printSet(kv.first, kv.second);
}

}

int main()
{
    int choice;
    do
    {
        std::cout << "\n{     Меню     }\n";
        std::cout << "{ 1. Создать новое множество      }\n";
        std::cout << "{ 2. Добавить элемент в множество     }\n";
        std::cout << "{ 3. Удалить элемент из множества     }\n";
        std::cout << "{ 4. Определить мощность множества    }\n";
        std::cout << "{    5. Проверить принадлежность элемента множеству    }\n";
        std::cout << "{    6. Объединенить два множества    }" << std::endl;
        std::cout << "{    7. Пересечение двух множеств    }" << std::endl;
        std::cout << "{   8. Разность двух множеств   }" << std::endl;
        std::cout << "{   9. Построить булеан множества   }" << std::endl;
        std::cout << "{   10. Показать все множества   }" << std::endl;
        std::cout << "{   0. Выход    }" << std::endl;
        std::cout << "+--> Введите номер операции: ";

        std::cin >> choice;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Введите число от 0 до 11." << std::endl;
            continue;
        }
        std::cin.ignore(10000, '\n');

        try
        {
            switch (choice)
            {
            case 1:
                createNewSet();
                break;
            case 2:
                addElement();
                break;
            case 3:
                removeElement();
                break;
            case 4:
                printSetCardinality();
                break;
            case 5:
                checkElementMembership();
                break;
            case 6:
                performUnion();
                break;
            case 7:
                performIntersection();
                break;
            case 8:
                performDifference();
                break;
            case 9:
                printPowerSet();
                break;
            case 10:
                printAllSets();
                break;
            case 0:
                std::cout << "Выход из программы." << std::endl;
                break;
            default:
                std::cout << "Введите корректный номер операции." << std::endl;
                break;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "Ошибка: " << e.what() << std::endl;
        }
    } while (choice != 0);

    return 0;
}