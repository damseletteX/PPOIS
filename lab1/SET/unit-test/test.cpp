// test.cpp
// Юнит-тесты для класса Set / Element.
// Без внешних библиотек — самодельный мини-фреймворк из нескольких строк.
//
// СБОРКА (важно!):
//   компилировать нужно ЭТОТ файл + src/set.cpp,
//   и НИ В КОЕМ СЛУЧАЕ не подключать src/main.cpp одновременно —
//   иначе будет "multiple definition of main" (мы это уже разбирали).
//
// Пример команды из корня проекта (SET/):
//   g++ -std=c++17 unit-test/test.cpp src/set.cpp -o run_tests
//   ./run_tests

#include <iostream>
#include <string>
#include "../include/set.h"

// ---------------------------------------------------------------------
// Мини-фреймворк: просто считает пройденные/непройденные проверки
// и печатает отчёт. Не останавливает выполнение при первом же провале
// (в отличие от assert()) — так видно сразу все проблемы за один запуск.
// ---------------------------------------------------------------------
static int testsRun = 0;
static int testsPassed = 0;

void check(bool condition, const std::string& testName)
{
    testsRun++;
    if (condition)
    {
        testsPassed++;
        std::cout << "[PASS] " << testName << "\n";
    }
    else
    {
        std::cout << "[FAIL] " << testName << "\n";
    }
}

// =======================================================================
// 1. isEmpty() и add()
// =======================================================================
void test_isEmpty_default()
{
    Set s;
    check(s.isEmpty(), "Новое множество пустое");
    check(s.getCardinality() == 0, "Мощность нового множества равна 0");
}

void test_add_single_atom()
{
    Set s;
    s.add(Element(std::string("a")));
    check(!s.isEmpty(), "После добавления множество не пустое");
    check(s.getCardinality() == 1, "Мощность после одного add() равна 1");
    check(s[Element(std::string("a"))], "Добавленный элемент найден через operator[]");
}

void test_add_duplicate_ignored()
{
    Set s;
    s.add(Element(std::string("a")));
    s.add(Element(std::string("a"))); // дубликат — не должен добавиться
    check(s.getCardinality() == 1, "Повторное add() того же элемента не увеличивает мощность");
}

void test_add_several_distinct()
{
    Set s;
    s.add(Element(std::string("a")));
    s.add(Element(std::string("b")));
    s.add(Element(std::string("c")));
    check(s.getCardinality() == 3, "Три разных элемента дают мощность 3");
}

// =======================================================================
// 2. rmv()
// =======================================================================
void test_rmv_existing()
{
    Set s;
    s.add(Element(std::string("a")));
    s.add(Element(std::string("b")));
    s.rmv(Element(std::string("a")));
    check(s.getCardinality() == 1, "После удаления существующего элемента мощность уменьшилась");
    check(!s[Element(std::string("a"))], "Удалённого элемента больше нет в множестве");
    check(s[Element(std::string("b"))], "Оставшийся элемент по-прежнему на месте");
}

void test_rmv_nonexistent_is_noop()
{
    Set s;
    s.add(Element(std::string("a")));
    s.rmv(Element(std::string("z"))); // такого элемента нет
    check(s.getCardinality() == 1, "Удаление несуществующего элемента не меняет мощность");
}

void test_rmv_from_empty()
{
    Set s;
    s.rmv(Element(std::string("a"))); // множество и так пустое
    check(s.isEmpty(), "Удаление из пустого множества не ломает его");
}

// =======================================================================
// 3. clear()
// =======================================================================
void test_clear()
{
    Set s;
    s.add(Element(std::string("a")));
    s.add(Element(std::string("b")));
    s.clear();
    check(s.isEmpty(), "После clear() множество пустое");
    check(s.getCardinality() == 0, "После clear() мощность равна 0");
}

// =======================================================================
// 4. operator== — включая независимость от порядка (неупорядоченность!)
// =======================================================================
void test_equality_same_order()
{
    Set a; a.add(Element(std::string("x"))); a.add(Element(std::string("y")));
    Set b; b.add(Element(std::string("x"))); b.add(Element(std::string("y")));
    check(a == b, "Множества с одинаковым составом и порядком равны");
}

void test_equality_different_order()
{
    Set a; a.add(Element(std::string("x"))); a.add(Element(std::string("y")));
    Set b; b.add(Element(std::string("y"))); b.add(Element(std::string("x"))); // обратный порядок
    check(a == b, "Множества с одинаковым составом, но разным порядком добавления — равны");
}

void test_equality_different_size()
{
    Set a; a.add(Element(std::string("x")));
    Set b; b.add(Element(std::string("x"))); b.add(Element(std::string("y")));
    check(!(a == b), "Множества разной мощности не равны");
}

void test_equality_different_content_same_size()
{
    Set a; a.add(Element(std::string("x"))); a.add(Element(std::string("y")));
    Set b; b.add(Element(std::string("x"))); b.add(Element(std::string("z")));
    check(!(a == b), "Множества одинакового размера, но разного состава — не равны");
}

// =======================================================================
// 5. operator+ (объединение)
// =======================================================================
void test_union_with_empty()
{
    Set empty;
    Set b; b.add(Element(std::string("a"))); b.add(Element(std::string("b")));
    Set result = empty + b;
    check(result == b, "Объединение пустого множества с непустым равно непустому");
}

void test_union_disjoint()
{
    Set a; a.add(Element(std::string("x")));
    Set b; b.add(Element(std::string("y")));
    Set result = a + b;
    check(result.getCardinality() == 2, "Объединение непересекающихся множеств даёт сумму мощностей");
    check(result[Element(std::string("x"))] && result[Element(std::string("y"))],
          "Оба исходных элемента присутствуют после объединения");
}

void test_union_with_overlap_no_duplicates()
{
    Set a; a.add(Element(std::string("x"))); a.add(Element(std::string("y")));
    Set b; b.add(Element(std::string("y"))); b.add(Element(std::string("z")));
    Set result = a + b;
    check(result.getCardinality() == 3, "Общий элемент не удваивается при объединении");
}

void test_union_self_idempotent()
{
    Set a; a.add(Element(std::string("x"))); a.add(Element(std::string("y")));
    Set result = a + a;
    check(result == a, "Объединение множества с самим собой не меняет его");
}

// =======================================================================
// 6. operator* (пересечение)
// =======================================================================
void test_intersect_with_overlap()
{
    Set a; a.add(Element(std::string("x"))); a.add(Element(std::string("y")));
    Set b; b.add(Element(std::string("y"))); b.add(Element(std::string("z")));
    Set result = a * b;
    check(result.getCardinality() == 1, "Пересечение содержит только общий элемент");
    check(result[Element(std::string("y"))], "Общий элемент 'y' присутствует в пересечении");
}

void test_intersect_disjoint_is_empty()
{
    Set a; a.add(Element(std::string("x")));
    Set b; b.add(Element(std::string("y")));
    Set result = a * b;
    check(result.isEmpty(), "Пересечение непересекающихся множеств пусто");
}

void test_intersect_with_empty()
{
    Set a; a.add(Element(std::string("x")));
    Set empty;
    Set result = a * empty;
    check(result.isEmpty(), "Пересечение с пустым множеством всегда пусто");
}

// =======================================================================
// 7. operator- (разность)
// =======================================================================
void test_difference_removes_common()
{
    Set a; a.add(Element(std::string("x"))); a.add(Element(std::string("y")));
    Set b; b.add(Element(std::string("y")));
    Set result = a - b;
    check(result.getCardinality() == 1, "Разность убирает общий элемент");
    check(result[Element(std::string("x"))] && !result[Element(std::string("y"))],
          "Остался только элемент, отсутствующий во втором множестве");
}

void test_difference_with_disjoint_unchanged()
{
    Set a; a.add(Element(std::string("x")));
    Set b; b.add(Element(std::string("z")));
    Set result = a - b;
    check(result == a, "Разность с непересекающимся множеством не меняет исходное");
}

void test_difference_full_removal_gives_empty()
{
    Set a; a.add(Element(std::string("x"))); a.add(Element(std::string("y")));
    Set result = a - a;
    check(result.isEmpty(), "Разность множества с самим собой пуста");
}

// =======================================================================
// 8. Вложенные множества как элементы (Element хранит Set)
// =======================================================================
void test_nested_set_equality()
{
    Set inner1; inner1.add(Element(std::string("a"))); inner1.add(Element(std::string("b")));
    Set inner2; inner2.add(Element(std::string("b"))); inner2.add(Element(std::string("a"))); // другой порядок

    Element e1(inner1);
    Element e2(inner2);
    check(e1 == e2, "Элементы-множества с одинаковым содержимым (в разном порядке) равны");
}

void test_nested_set_inequality()
{
    Set inner1; inner1.add(Element(std::string("a")));
    Set inner2; inner2.add(Element(std::string("b")));

    Element e1(inner1);
    Element e2(inner2);
    check(!(e1 == e2), "Элементы-множества с разным содержимым не равны");
}

void test_atom_vs_set_element_never_equal()
{
    Set inner; inner.add(Element(std::string("a")));
    Element atomEl(std::string("a"));
    Element setEl(inner);
    check(!(atomEl == setEl), "Атом и множество с тем же 'именем' — разные сущности, не равны");
}

void test_set_containing_nested_set()
{
    Set inner; inner.add(Element(std::string("a"))); inner.add(Element(std::string("b")));

    Set outer;
    outer.add(Element(std::string("x")));
    outer.add(Element(inner)); // вложенное множество как ОДИН элемент

    check(outer.getCardinality() == 2,
          "Вложенное множество считается как один элемент, а не как сумма его содержимого");
    check(outer[Element(inner)], "Вложенное множество находится через operator[]");
}

void test_union_with_nested_sets()
{
    Set inner; inner.add(Element(std::string("a")));

    Set s1; s1.add(Element(std::string("x"))); s1.add(Element(inner));
    Set s2; s2.add(Element(inner)); s2.add(Element(std::string("y")));

    Set result = s1 + s2;
    check(result.getCardinality() == 3,
          "Объединение множеств с общим вложенным множеством не дублирует его");
}

// =======================================================================
// 9. Пустое множество как элемент (важный частный случай из задания)
// =======================================================================
void test_empty_set_as_element()
{
    Set empty;
    Set outer;
    outer.add(Element(std::string("a")));
    outer.add(Element(empty)); // {} как элемент

    check(outer.getCardinality() == 2, "{} как элемент увеличивает мощность на 1, как обычный элемент");
    check(outer[Element(empty)], "Пустое множество-элемент находится через operator[]");
}

// =======================================================================
// 10. Заготовки на будущее — parse() и buildBoolean()
//     Эти тесты СЕЙЧАС будут падать, потому что методы ещё не реализованы.
//     Оставлены специально: как только допишете parse()/buildBoolean(),
//     тесты сразу же начнут проходить без каких-либо изменений в них.
// =======================================================================
void test_parse_simple_TODO()
{
    std::string description = "{a,b,c}";
    Set parsed(description);
    Set expected;
    expected.add(Element(std::string("a")));
    expected.add(Element(std::string("b")));
    expected.add(Element(std::string("c")));
    check(parsed == expected, "[TODO parse] {a,b,c} разбирается в множество из трёх атомов");
}

void test_parse_nested_TODO()
{
    std::string description = "{a,{b,c}}";
    Set parsed(description);
    check(parsed.getCardinality() == 2,
          "[TODO parse] {a,{b,c}} должно дать мощность 2 (атом + вложенное множество)");
}

void test_parse_empty_TODO()
{
    std::string description = "{}";
    Set parsed(description);
    check(parsed.isEmpty(), "[TODO parse] {} разбирается в пустое множество");
}

void test_buildBoolean_of_empty_TODO()
{
    Set empty;
    Set boolean = empty.buildBoolean();
    // булеан пустого множества — это {∅}, то есть множество из ОДНОГО элемента (самого пустого множества)
    check(boolean.getCardinality() == 1,
          "[TODO buildBoolean] Булеан пустого множества содержит один элемент — само пустое множество");
}

void test_buildBoolean_size_TODO()
{
    Set s;
    s.add(Element(std::string("a")));
    s.add(Element(std::string("b")));
    Set boolean = s.buildBoolean();
    // булеан множества мощности n должен содержать 2^n элементов
    check(boolean.getCardinality() == 4,
          "[TODO buildBoolean] Булеан множества из 2 элементов содержит 2^2 = 4 подмножества");
}

// =======================================================================
// main — запуск всех тестов и итоговый отчёт
// =======================================================================
int main()
{
    test_isEmpty_default();
    test_add_single_atom();
    test_add_duplicate_ignored();
    test_add_several_distinct();

    test_rmv_existing();
    test_rmv_nonexistent_is_noop();
    test_rmv_from_empty();

    test_clear();

    test_equality_same_order();
    test_equality_different_order();
    test_equality_different_size();
    test_equality_different_content_same_size();

    test_union_with_empty();
    test_union_disjoint();
    test_union_with_overlap_no_duplicates();
    test_union_self_idempotent();

    test_intersect_with_overlap();
    test_intersect_disjoint_is_empty();
    test_intersect_with_empty();

    test_difference_removes_common();
    test_difference_with_disjoint_unchanged();
    test_difference_full_removal_gives_empty();

    test_nested_set_equality();
    test_nested_set_inequality();
    test_atom_vs_set_element_never_equal();
    test_set_containing_nested_set();
    test_union_with_nested_sets();

    test_empty_set_as_element();

    test_parse_simple_TODO();
    test_parse_nested_TODO();
    test_parse_empty_TODO();
    test_buildBoolean_of_empty_TODO();
    test_buildBoolean_size_TODO();

    std::cout << "\n===================================\n";
    std::cout << "Пройдено: " << testsPassed << " / " << testsRun << "\n";
    std::cout << "===================================\n";

    return (testsPassed == testsRun) ? 0 : 1;
}