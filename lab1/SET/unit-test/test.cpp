// Build and run from the console (from the project root; do NOT add src/main.cpp,
// this file has its own main()):
//   g++ -std=c++17 -Wall -Wextra unit-test/test.cpp src/set.cpp src/cli.cpp -o run_tests
//   ./run_tests
//
// Exit code: 0 - all checks passed, 1 - some failed (CI uses it to mark the step).

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <initializer_list>
#include "../include/set.h"
#include "../include/cli.h"

static int testsRun = 0;
static std::vector<std::string> failedTests;

void check(bool condition, const std::string &name)
{
    testsRun++;
    if (condition)
    {
        std::cout << "[PASS] " << name << "\n";
    }
    else
    {
        failedTests.push_back(name);
        std::cout << "[FAIL] " << name << "\n";
    }
}

template <class Ex, class F>
bool throwsAs(F f)
{
    try
    {
        f();
    }
    catch (const Ex &)
    {
        return true;
    }
    catch (...)
    {
        return false;
    }
    return false;
}

bool contains(const std::string &text, const std::string &part)
{
    return text.find(part) != std::string::npos;
}

Element A(const std::string &name) { return Element(name); }
Element S(const Set &s) { return Element(s); }

Set fromAtoms(std::initializer_list<std::string> names)
{
    Set s;
    for (const std::string &n : names)
        s.add(Element(n));
    return s;
}

std::string cli(const std::string &input, int *code = nullptr)
{
    std::istringstream in(input);
    std::ostringstream out;
    int c = runCli(in, out);
    if (code)
        *code = c;
    return out.str();
}

void test_element()
{
    check(A("a") == A("a"), "Element: equal atoms are equal");
    check(!(A("a") == A("b")), "Element: different atoms differ");

    Set inner = fromAtoms({"a"});
    check(!(A("a") == S(inner)), "Element: atom != set (atom on the left)");
    check(!(S(inner) == A("a")), "Element: atom != set (set on the left)");

    Set s1 = fromAtoms({"a", "b"});
    check(S(s1) == S(fromAtoms({"b", "a"})), "Element: same sets in a different order are equal");
    check(!(S(s1) == S(fromAtoms({"a", "c"}))), "Element: same size, different content differ");
    check(!(S(s1) == S(fromAtoms({"a"}))), "Element: different cardinality differs");
    check(S(Set()) == S(Set()), "Element: empty set equals empty set");
    check(!(S(Set()) == S(inner)), "Element: empty set != non-empty set");

    Set deepA("{a, {b, {c, d}}}");
    check(S(deepA) == S(Set("{{{d, c}, b}, a}")), "Element: deeply nested sets are equal in any order");
    check(!(S(deepA) == S(Set("{a, {b, {c, e}}}"))), "Element: a difference at depth is noticed");
}

void test_element_toString()
{
    check(A("abc").toString() == "abc", "Element::toString: atom");
    check(S(Set("{a, b}")).toString() == "{a, b}", "Element::toString: set");
    check(S(Set()).toString() == "{}", "Element::toString: empty set");
}
void test_basics()
{
    Set s;
    check(s.isEmpty(), "Set: a new set is empty");
    check(s.getCardinality() == 0, "Set: a new set has cardinality 0");
    s.add(A("a"));
    check(!s.isEmpty(), "Set: not empty after add");
    check(s.getCardinality() == 1, "Set: cardinality 1 after one add");
    s.add(A("b"));
    s.add(A("c"));
    check(s.getCardinality() == 3, "Set: three distinct elements -> cardinality 3");
}
void test_add()
{
    Set s;
    s.add(A("a"));
    check(throwsAs<std::logic_error>([&]()
                                     { s.add(A("a")); }),
          "add: duplicate atom throws logic_error");
    check(s.getCardinality() == 1, "add: a failed add leaves the set unchanged");

    Set n;
    n.add(S(fromAtoms({"x", "y"})));
    check(n.getCardinality() == 1, "add: a nested set is added as one element");
    check(throwsAs<std::logic_error>([&]()
                                     { n.add(S(fromAtoms({"y", "x"}))); }),
          "add: duplicate nested set (other order) throws");
    n.add(A("x"));
    check(n.getCardinality() == 2, "add: atom 'x' and set {x,y} are different elements");

    Set e;
    e.add(S(Set()));
    check(e.getCardinality() == 1 && !e.isEmpty(), "add: the empty set can be an element");
}

void test_rmv()
{
    Set s = fromAtoms({"a", "b"});
    s.rmv(A("a"));
    check(s.getCardinality() == 1, "rmv: cardinality decreased");
    check(!s[A("a")] && s[A("b")], "rmv: the right element removed, the other stays");
    check(throwsAs<std::logic_error>([&]()
                                     { s.rmv(A("zzz")); }),
          "rmv: missing element throws logic_error");
    check(s.getCardinality() == 1, "rmv: a failed rmv leaves the set unchanged");

    Set empty;
    check(throwsAs<std::logic_error>([&]()
                                     { empty.rmv(A("a")); }),
          "rmv: removing from an empty set throws");

    Set n;
    n.add(A("x"));
    n.add(S(fromAtoms({"p", "q"})));
    n.rmv(S(fromAtoms({"q", "p"})));
    check(n.getCardinality() == 1 && n[A("x")], "rmv: nested set is removed by content");

    Set one = fromAtoms({"a"});
    one.rmv(A("a"));
    check(one.isEmpty(), "rmv: removing the only element gives an empty set");
}

void test_clear()
{
    Set s = fromAtoms({"a", "b"});
    s.clear();
    check(s.isEmpty() && s.getCardinality() == 0, "clear: the set is empty");
    s.clear();
    check(s.isEmpty(), "clear: clearing an empty set is safe");
}

void test_membership()
{
    Set s;
    s.add(A("a"));
    s.add(S(fromAtoms({"b", "c"})));
    s.add(S(Set()));
    check(s[A("a")], "[]: atom found");
    check(!s[A("z")], "[]: missing atom not found");
    check(s[S(fromAtoms({"c", "b"}))], "[]: nested set found (order does not matter)");
    check(!s[S(fromAtoms({"b"}))], "[]: {b} does not belong (b lies deeper)");
    check(!s[A("b")], "[]: elements of a nested set are not elements of the outer one");
    check(s[S(Set())], "[]: empty set found");
    check(!Set()[A("a")], "[]: nothing is in an empty set");
}

void test_equality()
{
    check(fromAtoms({"x", "y"}) == fromAtoms({"x", "y"}), "==: same order");
    check(fromAtoms({"x", "y"}) == fromAtoms({"y", "x"}), "==: different order is still equal");
    check(!(fromAtoms({"x"}) == fromAtoms({"x", "y"})), "==: different cardinality");
    check(!(fromAtoms({"x", "y"}) == fromAtoms({"x", "z"})), "==: same size, different content");
    check(Set() == Set(), "==: empty sets are equal");
    check(!(Set() == fromAtoms({"a"})), "==: empty != non-empty");
    Set s = fromAtoms({"a", "b"});
    check(s == s, "==: a set equals itself");
}
void test_union()
{
    Set a = fromAtoms({"x", "y"});
    Set b = fromAtoms({"y", "z"});
    Set a0 = a, b0 = b;

    Set u = a + b;
    check(u.getCardinality() == 3, "+: a common element is not duplicated");
    check(u == fromAtoms({"x", "y", "z"}), "+: union content is correct");
    check(a == a0 && b == b0, "+: operands are unchanged");
    check((Set() + b) == b, "+: empty + B = B");
    check((b + Set()) == b, "+: B + empty = B");
    check((Set() + Set()).isEmpty(), "+: empty + empty = empty");
    check((fromAtoms({"p"}) + fromAtoms({"q"})).getCardinality() == 2, "+: disjoint sets - cardinalities add up");
    check((a + a) == a, "+: A + A = A");

    Set n1, n2;
    n1.add(A("x"));
    n1.add(S(fromAtoms({"a"})));
    n2.add(S(fromAtoms({"a"})));
    n2.add(A("y"));
    check((n1 + n2).getCardinality() == 3, "+: a common nested set is not duplicated");
}

void test_union_assign()
{
    Set a = fromAtoms({"x", "y"});
    Set b = fromAtoms({"y", "z"});
    Set &ref = (a += b);
    check(&ref == &a, "+=: returns a reference to the left operand");
    check(a == fromAtoms({"x", "y", "z"}), "+=: the left operand became the union");
    check(b == fromAtoms({"y", "z"}), "+=: the right operand is unchanged");
    a += a;
    check(a.getCardinality() == 3, "+=: A += A changes nothing");
    Set c = fromAtoms({"w"});
    (a += b) += c;
    check(a.getCardinality() == 4 && a[A("w")], "+=: chaining works");
    Set e;
    e += Set();
    check(e.isEmpty(), "+=: empty += empty = empty");
}
void test_intersection()
{
    Set a = fromAtoms({"x", "y"});
    Set b = fromAtoms({"y", "z"});
    Set a0 = a, b0 = b;
    Set r = a * b;
    check(r.getCardinality() == 1 && r[A("y")], "*: only the common element");
    check(a == a0 && b == b0, "*: operands are unchanged");
    check((fromAtoms({"x"}) * fromAtoms({"y"})).isEmpty(), "*: disjoint -> empty");
    check((a * Set()).isEmpty(), "*: A * empty = empty");
    check((Set() * a).isEmpty(), "*: empty * A = empty");
    check((a * a) == a, "*: A * A = A");

    Set n1, n2;
    n1.add(A("x"));
    n1.add(S(fromAtoms({"a", "b"})));
    n2.add(S(fromAtoms({"b", "a"})));
    n2.add(A("z"));
    Set nr = n1 * n2;
    check(nr.getCardinality() == 1 && nr[S(fromAtoms({"a", "b"}))], "*: nested sets intersect by content");
}

void test_intersection_assign()
{
    Set a = fromAtoms({"x", "y"});
    Set b = fromAtoms({"y", "z"});
    Set &ref = (a *= b);
    check(&ref == &a, "*=: returns a reference to the left operand");
    check(a == fromAtoms({"y"}), "*=: the left operand became the intersection");
    check(b == fromAtoms({"y", "z"}), "*=: the right operand is unchanged");
    a *= a;
    check(a == fromAtoms({"y"}), "*=: A *= A changes nothing");
    a *= fromAtoms({"q"});
    check(a.isEmpty(), "*=: intersecting with a disjoint set gives empty");
}
void test_difference()
{
    Set a = fromAtoms({"x", "y"});
    Set b = fromAtoms({"y", "z"});
    Set a0 = a, b0 = b;
    Set d = a - b;
    check(d.getCardinality() == 1 && d[A("x")] && !d[A("y")], "-: only x remains");
    check(a == a0 && b == b0, "-: operands are unchanged");
    check((a - fromAtoms({"q"})) == a, "-: subtracting a disjoint set changes nothing");
    check((a - a).isEmpty(), "-: A - A = empty");
    check((a - Set()) == a, "-: A - empty = A");
    check((Set() - a).isEmpty(), "-: empty - A = empty");

    Set n;
    n.add(A("x"));
    n.add(S(fromAtoms({"a"})));
    Set sub;
    sub.add(S(fromAtoms({"a"})));
    Set nd = n - sub;
    check(nd.getCardinality() == 1 && nd[A("x")], "-: a nested set is subtracted by content");
}

void test_difference_assign()
{
    Set a = fromAtoms({"x", "y", "z"});
    Set &ref = (a -= fromAtoms({"y"}));
    check(&ref == &a, "-=: returns a reference to the left operand");
    check(a == fromAtoms({"x", "z"}), "-=: the element is removed");
    a -= a;
    check(a.isEmpty(), "-=: A -= A gives an empty set");
    Set e;
    e -= fromAtoms({"a"});
    check(e.isEmpty(), "-=: subtracting from an empty set is safe");
}

void test_copy()
{
    Set a = fromAtoms({"x", "y"});
    Set b = a;
    check(b == a && b.getCardinality() == 2, "copy: equals the original");
    b.add(A("z"));
    check(a.getCardinality() == 2 && !a[A("z")], "copy: changing the copy leaves the original alone");
    a.rmv(A("x"));
    check(b[A("x")], "copy: changing the original leaves the copy alone");
    Set c;
    c = b;
    check(c == b, "assignment: content is copied");
    Set n;
    n.add(S(fromAtoms({"p"})));
    Set m = n;
    check(m == n && m.getCardinality() == 1, "copy of a set with a nested set");
}
void test_parse()
{
    check(Set("{a,b,c}") == fromAtoms({"a", "b", "c"}), "parse: a simple set");
    check(Set("{ a ,  b , c }") == fromAtoms({"a", "b", "c"}), "parse: spaces are ignored");
    check(Set("{a,\tb,\nc}").getCardinality() == 3, "parse: tabs and newlines are ignored");
    check(Set("{apple, banana}")[A("apple")], "parse: multi-character atoms");
    check(Set("{}").isEmpty(), "parse: {} is the empty set");
    check(Set("").isEmpty(), "parse: an empty string gives the empty set");
    check(Set("   ").isEmpty(), "parse: a blank string gives the empty set");
    check(Set("{a}").getCardinality() == 1, "parse: a single element");

    Set p("{a, b, c, {a, b}, {}, {a, {c}}}");
    check(p.getCardinality() == 6, "parse: the example from the task has 6 top-level elements");
    check(p[A("a")] && p[A("b")] && p[A("c")], "parse: the atoms of the example are present");
    check(p[S(Set("{a,b}"))], "parse: nested {a,b}");
    check(p[S(Set("{}"))], "parse: nested {}");
    check(p[S(Set("{a,{c}}"))], "parse: nested {a,{c}}");
    check(!p[S(Set("{c}"))], "parse: {c} lies deeper, it is not a top-level element");

    check(Set("{{}}").getCardinality() == 1 && Set("{{}}")[S(Set())], "parse: {{}} - a set holding the empty set");
    check(Set("{{{a}}}").getCardinality() == 1, "parse: deep nesting");
    check(Set("{a,a}").getCardinality() == 1, "parse: a duplicated atom collapses");
    check(Set("{{},{}}").getCardinality() == 1, "parse: a duplicated {} collapses");
    check(Set("{{a,b},{b,a}}").getCardinality() == 1, "parse: equal nested sets collapse");
}

void test_parse_errors()
{
    const char *bad[] = {"{a", "a}", "{a}}", "a,b", "{a}b", "}{", "{a,{b}", "a", ",a", "{a},b", ",", "{a},", "}"};
    for (const char *text : bad)
    {
        check(throwsAs<std::invalid_argument>([&]()
                                              { Set s(text); }),
              std::string("parse: invalid_argument for \"") + text + "\"");
    }
}

void test_toString()
{
    check(Set().toString() == "{}", "toString: empty set");
    check(fromAtoms({"a", "b"}).toString() == "{a, b}", "toString: atoms separated by comma and space");
    check(Set("{a,{b,c}}").toString() == "{a, {b, c}}", "toString: nested set");
    check(Set("{{}}").toString() == "{{}}", "toString: a set holding the empty set");
    Set p("{a, b, c, {a, b}, {}, {a, {c}}}");
    check(Set(p.toString()) == p, "toString: the string parses back into an equal set");
}

void test_boolean()
{
    Set b0 = Set().buildBoolean();
    check(b0.getCardinality() == 1 && b0[S(Set())], "boolean of {}: one element - the empty set");

    check(fromAtoms({"a"}).buildBoolean() == Set("{{}, {a}}"), "boolean of {a} = {{}, {a}}");

    Set two = fromAtoms({"a", "b"});
    Set b2 = two.buildBoolean();
    check(b2.getCardinality() == 4, "boolean of 2 elements: 4 subsets");
    check(b2 == Set("{{}, {a}, {b}, {a,b}}"), "boolean of {a,b} = {{}, {a}, {b}, {a,b}}");
    check(!b2[A("a")] && !b2[A("b")], "boolean: the atoms themselves are not in the result");
    check(b2[S(two)] && b2[S(Set())], "boolean contains the original and the empty set");
    check(two == fromAtoms({"a", "b"}), "boolean: the original set is unchanged");

    check(fromAtoms({"a", "b", "c"}).buildBoolean() ==
              Set("{{}, {a}, {b}, {c}, {a,b}, {a,c}, {b,c}, {a,b,c}}"),
          "boolean of {a,b,c}: all 8 subsets are correct");
    check(fromAtoms({"a", "b", "c", "d"}).buildBoolean().getCardinality() == 16, "boolean of 4: 16 subsets");

    Set ten;
    for (int i = 0; i < 10; i++)
        ten.add(A("e" + std::to_string(i)));
    check(ten.buildBoolean().getCardinality() == 1024, "boolean of 10: 1024 = 2^10");

    Set nested;
    nested.add(A("a"));
    nested.add(S(fromAtoms({"b"})));
    Set bn = nested.buildBoolean();
    Set onlyNested;
    onlyNested.add(S(fromAtoms({"b"})));
    check(bn.getCardinality() == 4 && bn[S(nested)] && bn[S(onlyNested)], "boolean with a nested element");

    check(fromAtoms({"a"}).buildBoolean().buildBoolean().getCardinality() == 4, "boolean of boolean of {a}: 4");

    for (int n : {63, 64})
    {
        Set big;
        for (int i = 0; i < n; i++)
            big.add(A("e" + std::to_string(i)));
        check(throwsAs<std::length_error>([&]()
                                          { big.buildBoolean(); }),
              "boolean: length_error for a too large set (n=" + std::to_string(n) + ")");
    }
}

void test_cli_basic()
{
    int code = -1;
    std::string out = cli("0\n", &code);
    check(code == 0 && contains(out, "Goodbye!"), "CLI: exit with 0, return code 0");
    check(contains(out, "1) Create new set") && contains(out, "9) Build boolean"), "CLI: the menu is printed");

    code = -1;
    out = cli("", &code);
    check(code == 0 && contains(out, "Input ended"), "CLI: end of input without exit is handled");

    out = cli("1\nA\n");
    check(contains(out, "Input ended"), "CLI: end of input in the middle of a dialog is handled");

    out = cli("99\n\n0\n");
    check(contains(out, "Error: Unknown menu item: 99"), "CLI: unknown menu item");
}

void test_cli_create_show()
{
    std::string out = cli("10\n0\n");
    check(contains(out, "No sets yet."), "CLI: show when there are no sets");

    out = cli("1\nA\n{a, b, {c}}\n10\n0\n");
    check(contains(out, "Created: A = {a, b, {c}}"), "CLI: create a set from a string");
    check(contains(out, "A = {a, b, {c}}  (cardinality 3)"), "CLI: show a set with its cardinality");

    out = cli("1\nA\n{a}\n1\nA\n{b}\n0\n");
    check(contains(out, "Overwritten: A = {b}"), "CLI: an existing name is overwritten");

    out = cli("1\nA\n{a\n0\n");
    check(contains(out, "Error: Invalid set string"), "CLI: invalid set string");

    out = cli("1\nA\n\n0\n");
    check(contains(out, "Error: Set string is empty."), "CLI: an empty set string is rejected");

    out = cli("1\nmy set\n0\n");
    check(contains(out, "Error: Name must be non-empty"), "CLI: a name with a space is rejected");
}

void test_cli_elements()
{
    std::string out = cli("1\nA\n{a}\n2\nA\nb\n2\nA\nb\n0\n");
    check(contains(out, "Element added. Now: {a, b}"), "CLI: add an atom");
    check(contains(out, "Error: Element already in set."), "CLI: add a duplicate");

    out = cli("1\nA\n{a}\n2\nA\n{c, d}\n0\n");
    check(contains(out, "Now: {a, {c, d}}"), "CLI: add a nested set");

    out = cli("1\nA\n{a}\n2\nA\na,b\n2\nA\n\n2\nA\n{x\n0\n");
    check(contains(out, "An atom must not contain"), "CLI: an invalid atom is rejected");
    check(contains(out, "Element must not be empty."), "CLI: an empty element is rejected");
    check(contains(out, "Error: Invalid set string"), "CLI: an invalid nested element is rejected");

    out = cli("2\nnope\n0\n");
    check(contains(out, "Error: Set 'nope' not found."), "CLI: add to a missing set");

    out = cli("1\nA\n{a, b, {c}}\n3\nA\na\n3\nA\nzzz\n3\nA\n{c}\n0\n");
    check(contains(out, "Element removed. Now: {b, {c}}"), "CLI: remove an atom");
    check(contains(out, "Error: Such element not found."), "CLI: remove a missing element");
    check(contains(out, "Element removed. Now: {b}"), "CLI: remove a nested set");

    out = cli("3\nnope\n0\n");
    check(contains(out, "not found"), "CLI: remove from a missing set");
}

void test_cli_queries()
{
    std::string out = cli("1\nA\n{a, b, {c}}\n4\nA\n0\n");
    check(contains(out, "Cardinality of A: 3"), "CLI: cardinality");
    out = cli("4\nnope\n0\n");
    check(contains(out, "not found"), "CLI: cardinality of a missing set");

    out = cli("1\nA\n{}\n11\nA\n1\nB\n{a}\n11\nB\n0\n");
    check(contains(out, "Set A is empty."), "CLI: emptiness - empty");
    check(contains(out, "Set B is not empty."), "CLI: emptiness - not empty");
    out = cli("11\nnope\n0\n");
    check(contains(out, "not found"), "CLI: emptiness of a missing set");

    out = cli("1\nA\n{a, {b}}\n5\nA\na\n5\nA\nz\n5\nA\n{b}\n0\n");
    check(contains(out, "Element belongs to set A."), "CLI: membership - yes");
    check(contains(out, "Element does not belong to set A."), "CLI: membership - no");
    out = cli("5\nnope\n0\n");
    check(contains(out, "not found"), "CLI: membership in a missing set");
    out = cli("1\nA\n{a}\n5\nA\na b\n0\n");
    check(contains(out, "An atom must not contain"), "CLI: membership - an invalid element");

    out = cli("1\nA\n{a, b}\n1\nB\n{b, a}\n1\nC\n{a}\n12\nA\nB\n12\nA\nC\n0\n");
    check(contains(out, "Sets are equal."), "CLI: compare - equal");
    check(contains(out, "Sets are not equal."), "CLI: compare - not equal");
    out = cli("12\nnope\n0\n");
    check(contains(out, "not found"), "CLI: compare, first set missing");
    out = cli("1\nA\n{a}\n12\nA\nnope\n0\n");
    check(contains(out, "Set 'nope' not found."), "CLI: compare, second set missing");

    out = cli("1\nA\n{a, b}\n13\nA\n4\nA\n0\n");
    check(contains(out, "Set A cleared.") && contains(out, "Cardinality of A: 0"), "CLI: clear a set");
    out = cli("13\nnope\n0\n");
    check(contains(out, "not found"), "CLI: clear a missing set");
}

void test_cli_operations()
{
    const std::string make = "1\nA\n{a, b}\n1\nB\n{b, c}\n";
    std::string out = cli(make + "6\nA\nB\nC\n0\n");
    check(contains(out, "C = {a, b, c}  (cardinality 3)"), "CLI: union into a new set");
    out = cli(make + "7\nA\nB\nC\n0\n");
    check(contains(out, "C = {b}  (cardinality 1)"), "CLI: intersection into a new set");
    out = cli(make + "8\nA\nB\nC\n0\n");
    check(contains(out, "C = {a}  (cardinality 1)"), "CLI: difference into a new set");

    out = cli(make + "6\nA\nB\n\n0\n");
    check(contains(out, "A = {a, b, c}  (cardinality 3)"), "CLI: union in place (A += B)");
    out = cli(make + "7\nA\nB\n\n0\n");
    check(contains(out, "A = {b}  (cardinality 1)"), "CLI: intersection in place (A *= B)");
    out = cli(make + "8\nA\nB\n\n0\n");
    check(contains(out, "A = {a}  (cardinality 1)"), "CLI: difference in place (A -= B)");

    out = cli(make + "6\nA\nB\nC\n10\n0\n");
    check(contains(out, "A = {a, b}  (cardinality 2)") && contains(out, "C = {a, b, c}"),
          "CLI: a new result is stored separately, the operands stay intact");

    out = cli(make + "6\nnope\n0\n");
    check(contains(out, "Error: Set 'nope' not found."), "CLI: operation, first set missing");
    out = cli(make + "6\nA\nnope\n0\n");
    check(contains(out, "Error: Set 'nope' not found."), "CLI: operation, second set missing");
    out = cli(make + "6\nA\nB\nbad name\n0\n");
    check(contains(out, "Error: Name must be non-empty"), "CLI: operation, invalid result name");
}

void test_cli_boolean()
{
    std::string out = cli("1\nA\n{a, b}\n9\nA\nP\n0\n");
    check(contains(out, "P = {{}, {a}, {b}, {a, b}}  (cardinality 4)"), "CLI: boolean of {a, b}");

    out = cli("9\nnope\n0\n");
    check(contains(out, "not found"), "CLI: boolean of a missing set");

    out = cli("1\nA\n{a}\n9\nA\nbad name\n0\n");
    check(contains(out, "Error: Name must be non-empty"), "CLI: boolean, invalid result name");

    std::string big = "{";
    for (int i = 0; i < 21; i++)
        big += (i ? ",e" : "e") + std::to_string(i);
    big += "}";
    out = cli("1\nBIG\n" + big + "\n9\nBIG\nP\n0\n");
    check(contains(out, "Error: Set is too large for a boolean"), "CLI: a large boolean is rejected");
}

int main()
{
    test_element();
    test_element_toString();
    test_basics();
    test_add();
    test_rmv();
    test_clear();
    test_membership();
    test_equality();
    test_union();
    test_union_assign();
    test_intersection();
    test_intersection_assign();
    test_difference();
    test_difference_assign();
    test_copy();
    test_parse();
    test_parse_errors();
    test_toString();
    test_boolean();
    test_cli_basic();
    test_cli_create_show();
    test_cli_elements();
    test_cli_queries();
    test_cli_operations();
    test_cli_boolean();

    std::cout << "\n%%%%%%%%%%%%%\n";
    std::cout << "Passed: " << (testsRun - (int)failedTests.size()) << " / " << testsRun << "\n";
    if (!failedTests.empty())
    {
        std::cout << "Failed:\n";
        for (const std::string &n : failedTests)
            std::cout << "  - " << n << "\n";
    }
    std::cout << "%%%%%%%%%%%%%%\n";
    return failedTests.empty() ? 0 : 1;
}