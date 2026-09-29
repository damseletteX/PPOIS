#include "../include/cli.h"
#include "../include/set.h"
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

namespace
{

const size_t kMaxBooleanSize = 20; // 2^20 подмножеств — предел разумного для консоли

// Бросается, когда входной поток закончился посреди диалога.
struct EndOfInput
{
};

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
        throw std::invalid_argument("Name must be non-empty and contain no spaces.");
}

// Превращает введённый текст в элемент: "{...}" — вложенное множество, иначе атом.
Element parseElement(const std::string &raw)
{
    std::string t = trim(raw);
    if (t.empty())
        throw std::invalid_argument("Element must not be empty.");
    if (t.front() == '{')
        return Element(Set(t));
    if (t.find_first_of("{}, \t") != std::string::npos)
        throw std::invalid_argument("An atom must not contain the characters { } , or spaces.");
    return Element(t);
}

class Cli
{
public:
    Cli(std::istream &in, std::ostream &out) : in_(in), out_(out) {}

    int run()
    {
        try
        {
            while (true)
            {
                printMenu();
                std::string choice = ask("Your choice: ");
                if (choice == "0")
                {
                    out_ << "Goodbye!\n";
                    return 0;
                }
                try
                {
                    dispatch(choice);
                }
                catch (const std::exception &e)
                {
                    // Любая ошибка домена или ввода отменяет только текущую операцию.
                    out_ << "Error: " << e.what() << "\n";
                }
            }
        }
        catch (const EndOfInput &)
        {
            out_ << "\nInput ended.\n";
            return 0;
        }
    }

private:
    std::istream &in_;
    std::ostream &out_;
    std::map<std::string, Set> sets_; // имена множеств живут здесь, а не в классе Set

    std::string ask(const std::string &question)
    {
        out_ << question;
        std::string line;
        if (!std::getline(in_, line))
            throw EndOfInput();
        return trim(line);
    }

    Set &getSet(const std::string &name)
    {
        auto it = sets_.find(name);
        if (it == sets_.end())
            throw std::invalid_argument("Set '" + name + "' not found.");
        return it->second;
    }

    void printMenu()
    {
        out_ << "\n%%%%%%%%%%%\n"
                "Choose an operation:\n"
                "1) Create new set\n"
                "2) Add an element to a set\n"
                "3) Delete element from a set\n"
                "4) Determine set cardinality\n"
                "5) Check if an element is in set\n"
                "6) Unite 2 sets\n"
                "7) Intersect 2 sets\n"
                "8) Find difference between 2 sets\n"
                "9) Build boolean\n"
                "10) Show all sets\n"
                "11) Check if a set is empty\n"
                "12) Compare 2 sets\n"
                "13) Clear a set\n"
                "0) Exit\n"
                "%%%%%%%%%%%\n";
    }

    void dispatch(const std::string &choice)
    {
        if (choice == "1")
            createSet();
        else if (choice == "2")
            addElement();
        else if (choice == "3")
            removeElement();
        else if (choice == "4")
            showCardinality();
        else if (choice == "5")
            checkMembership();
        else if (choice == "6")
            binaryOperation('+');
        else if (choice == "7")
            binaryOperation('*');
        else if (choice == "8")
            binaryOperation('-');
        else if (choice == "9")
            booleanOperation();
        else if (choice == "10")
            showAll();
        else if (choice == "11")
            showEmptiness();
        else if (choice == "12")
            compareSets();
        else if (choice == "13")
            clearSet();
        else
            throw std::invalid_argument("Unknown menu item: " + choice);
    }

    void printSet(const std::string &name, const Set &s)
    {
        out_ << name << " = " << s.toString() << "  (cardinality " << s.getCardinality() << ")\n";
    }

    void createSet()
    {
        std::string name = ask("New set name: ");
        checkName(name);
        std::string text = ask("Set (for example {a, b, {c}}): ");
        if (text.empty())
            throw std::invalid_argument("Set string is empty.");
        Set s(text);
        bool existed = sets_.count(name) > 0;
        sets_.insert_or_assign(name, s);
        out_ << (existed ? "Overwritten: " : "Created: ") << name << " = " << s.toString() << "\n";
    }

    void showAll()
    {
        if (sets_.empty())
        {
            out_ << "No sets yet.\n";
            return;
        }
        for (const auto &kv : sets_)
            printSet(kv.first, kv.second);
    }

    void addElement()
    {
        Set &s = getSet(ask("Set name: "));
        Element e = parseElement(ask("Element (atom or {set}): "));
        s.add(e);
        out_ << "Element added. Now: " << s.toString() << "\n";
    }

    void removeElement()
    {
        Set &s = getSet(ask("Set name: "));
        Element e = parseElement(ask("Element (atom or {set}): "));
        s.rmv(e);
        out_ << "Element removed. Now: " << s.toString() << "\n";
    }

    void showCardinality()
    {
        std::string name = ask("Set name: ");
        const Set &s = getSet(name);
        out_ << "Cardinality of " << name << ": " << s.getCardinality() << "\n";
    }

    void showEmptiness()
    {
        std::string name = ask("Set name: ");
        const Set &s = getSet(name);
        out_ << "Set " << name << (s.isEmpty() ? " is empty.\n" : " is not empty.\n");
    }

    void checkMembership()
    {
        std::string name = ask("Set name: ");
        const Set &s = getSet(name);
        Element e = parseElement(ask("Element (atom or {set}): "));
        out_ << "Element " << (s[e] ? "belongs" : "does not belong") << " to set " << name << ".\n";
    }

    // op: '+' объединение, '*' пересечение, '-' разность.
    // Пустое имя результата означает "изменить первое множество на месте" (+=, *=, -=).
    void binaryOperation(char op)
    {
        std::string firstName = ask("First set: ");
        Set &a = getSet(firstName);
        Set &b = getSet(ask("Second set: "));
        std::string resultName = ask("Result name (leave empty to update the first set in place): ");

        if (resultName.empty())
        {
            if (op == '+')
                a += b;
            else if (op == '*')
                a *= b;
            else
                a -= b;
            printSet(firstName, a);
            return;
        }

        checkName(resultName);
        Set r;
        if (op == '+')
            r = a + b;
        else if (op == '*')
            r = a * b;
        else
            r = a - b;
        sets_.insert_or_assign(resultName, r);
        printSet(resultName, r);
    }

    void booleanOperation()
    {
        Set &s = getSet(ask("Set name: "));
        if (s.getCardinality() > kMaxBooleanSize)
            throw std::invalid_argument("Set is too large for a boolean (at most " +
                                        std::to_string(kMaxBooleanSize) + " elements).");
        std::string resultName = ask("Name for the boolean: ");
        checkName(resultName);
        Set b = s.buildBoolean();
        sets_.insert_or_assign(resultName, b);
        printSet(resultName, b);
    }

    void compareSets()
    {
        Set &a = getSet(ask("First set: "));
        Set &b = getSet(ask("Second set: "));
        out_ << (a == b ? "Sets are equal.\n" : "Sets are not equal.\n");
    }

    void clearSet()
    {
        std::string name = ask("Set name: ");
        getSet(name).clear();
        out_ << "Set " << name << " cleared.\n";
    }
};

} // namespace

int runCli(std::istream &in, std::ostream &out)
{
    Cli cli(in, out);
    return cli.run();
}