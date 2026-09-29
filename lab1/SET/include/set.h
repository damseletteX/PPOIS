#pragma once
#include <string>
#include <vector>
#include <memory>

class Set;

// Элемент множества: либо атом (строка), либо вложенное множество.
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
    ~Element() = default;

    bool operator==(const Element &other) const;
    // Текстовое представление: атом как есть, множество как "{...}".
    std::string toString() const;
};

// Неориентированное канторовское множество: элементы не повторяются и не упорядочены.
// Класс ничего не печатает. Ошибки сообщаются исключениями (все наследуются от
// std::logic_error), а вывод — забота интерфейса (см. cli.h).
class Set
{
    std::vector<Element> els;

    void parse(const std::string &);
    // Добавляет элемент, если его ещё нет. Не бросает исключений; true — если добавил.
    bool insertUnique(const Element &);

public:
    Set() = default;
    // Формирование из строки вида "{a, b, {c}, {}}".
    // Повторы схлопываются, при неверном формате — std::invalid_argument.
    explicit Set(const std::string &);

    bool isEmpty() const;
    size_t getCardinality() const;
    bool operator==(const Set &) const;

    // Бросает std::logic_error, если элемент уже есть.
    void add(const Element &);
    // Бросает std::logic_error, если элемента нет.
    void rmv(const Element &);
    void clear();

    // is element in set?
    bool operator[](const Element &) const;

    // union
    Set operator+(const Set &) const;
    Set &operator+=(const Set &);
    // intersection
    Set operator*(const Set &) const;
    Set &operator*=(const Set &);
    // difference
    Set operator-(const Set &) const;
    Set &operator-=(const Set &);

    // boolean: a set of all possible subsets.
    // Для 63 и более элементов бросает std::length_error.
    Set buildBoolean() const;

    // Например "{a, {b, c}}". Строка разбирается обратно конструктором.
    std::string toString() const;
};