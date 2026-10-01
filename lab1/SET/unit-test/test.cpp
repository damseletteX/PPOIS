#include <gtest/gtest.h>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include "set.h"

namespace
{

Element A(const std::string &name) { return Element(name); }
Element S(const Set &s) { return Element(s); }

Set fromAtoms(std::initializer_list<std::string> names)
{
    Set s;
    for (const std::string &n : names)
        s.add(Element(n));
    return s;
}

}

TEST(Element, EqualAtomsAreEqual)
{
    EXPECT_TRUE(A("a") == A("a"));
    EXPECT_FALSE(A("a") == A("b"));
}

TEST(Element, AtomNeverEqualsSet)
{
    Set inner = fromAtoms({"a"});
    EXPECT_FALSE(A("a") == S(inner));
    EXPECT_FALSE(S(inner) == A("a"));
}

TEST(Element, NestedSetsIgnoreOrder)
{
    Set s1 = fromAtoms({"a", "b"});
    EXPECT_TRUE(S(s1) == S(fromAtoms({"b", "a"})));
    EXPECT_FALSE(S(s1) == S(fromAtoms({"a", "c"})));
    EXPECT_FALSE(S(s1) == S(fromAtoms({"a"})));
}

TEST(Element, EmptySetEqualsEmptySet)
{
    Set inner = fromAtoms({"a"});
    EXPECT_TRUE(S(Set()) == S(Set()));
    EXPECT_FALSE(S(Set()) == S(inner));
}

TEST(Element, DeepNestingComparesRecursively)
{
    Set deepA("{a, {b, {c, d}}}");
    EXPECT_TRUE(S(deepA) == S(Set("{{{d, c}, b}, a}")));
    EXPECT_FALSE(S(deepA) == S(Set("{a, {b, {c, e}}}")));
}

TEST(Element, ToStringAtomAndSet)
{
    EXPECT_EQ(A("abc").toString(), "abc");
    EXPECT_EQ(S(Set("{a, b}")).toString(), "{a, b}");
    EXPECT_EQ(S(Set()).toString(), "{}");
}

// ============================================== Set: basics ================
TEST(Set, NewSetIsEmpty)
{
    Set s;
    EXPECT_TRUE(s.isEmpty());
    EXPECT_EQ(s.getCardinality(), 0u);
}

TEST(Set, CardinalityGrowsWithDistinctElements)
{
    Set s;
    s.add(A("a"));
    EXPECT_FALSE(s.isEmpty());
    EXPECT_EQ(s.getCardinality(), 1u);
    s.add(A("b"));
    s.add(A("c"));
    EXPECT_EQ(s.getCardinality(), 3u);
}

// ================================================================== add ====
TEST(SetAdd, DuplicateAtomThrows)
{
    Set s;
    s.add(A("a"));
    EXPECT_THROW(s.add(A("a")), std::logic_error);
    EXPECT_EQ(s.getCardinality(), 1u);
}

TEST(SetAdd, NestedSetIsOneElement)
{
    Set n;
    n.add(S(fromAtoms({"x", "y"})));
    EXPECT_EQ(n.getCardinality(), 1u);
    EXPECT_THROW(n.add(S(fromAtoms({"y", "x"}))), std::logic_error);
    n.add(A("x"));
    EXPECT_EQ(n.getCardinality(), 2u);
}

TEST(SetAdd, EmptySetCanBeAnElement)
{
    Set e;
    e.add(S(Set()));
    EXPECT_EQ(e.getCardinality(), 1u);
    EXPECT_FALSE(e.isEmpty());
}

// ================================================================== rmv ====
TEST(SetRmv, RemovesExistingElement)
{
    Set s = fromAtoms({"a", "b"});
    s.rmv(A("a"));
    EXPECT_EQ(s.getCardinality(), 1u);
    EXPECT_FALSE(s[A("a")]);
    EXPECT_TRUE(s[A("b")]);
}

TEST(SetRmv, MissingElementThrows)
{
    Set s = fromAtoms({"a", "b"});
    s.rmv(A("a"));
    EXPECT_THROW(s.rmv(A("zzz")), std::logic_error);
    EXPECT_EQ(s.getCardinality(), 1u);
}

TEST(SetRmv, RemovingFromEmptySetThrows)
{
    Set empty;
    EXPECT_THROW(empty.rmv(A("a")), std::logic_error);
}

TEST(SetRmv, NestedSetRemovedByContent)
{
    Set n;
    n.add(A("x"));
    n.add(S(fromAtoms({"p", "q"})));
    n.rmv(S(fromAtoms({"q", "p"})));
    EXPECT_EQ(n.getCardinality(), 1u);
    EXPECT_TRUE(n[A("x")]);
}

TEST(SetRmv, RemovingOnlyElementGivesEmptySet)
{
    Set one = fromAtoms({"a"});
    one.rmv(A("a"));
    EXPECT_TRUE(one.isEmpty());
}

TEST(SetClear, EmptiesTheSet)
{
    Set s = fromAtoms({"a", "b"});
    s.clear();
    EXPECT_TRUE(s.isEmpty());
    EXPECT_EQ(s.getCardinality(), 0u);
    s.clear(); // clearing an already empty set is safe
    EXPECT_TRUE(s.isEmpty());
}

// ================================================================ [] ========
TEST(SetMembership, AtomsAndNestedSets)
{
    Set s;
    s.add(A("a"));
    s.add(S(fromAtoms({"b", "c"})));
    s.add(S(Set()));

    EXPECT_TRUE(s[A("a")]);
    EXPECT_FALSE(s[A("z")]);
    EXPECT_TRUE(s[S(fromAtoms({"c", "b"}))]);
    EXPECT_FALSE(s[S(fromAtoms({"b"}))]);      // {b} lies deeper, not a member
    EXPECT_FALSE(s[A("b")]);                    // elements of a nested set aren't ours
    EXPECT_TRUE(s[S(Set())]);
    EXPECT_FALSE(Set()[A("a")]);
}

// ================================================================ == ========
TEST(SetEquality, OrderDoesNotMatter)
{
    EXPECT_TRUE(fromAtoms({"x", "y"}) == fromAtoms({"x", "y"}));
    EXPECT_TRUE(fromAtoms({"x", "y"}) == fromAtoms({"y", "x"}));
    EXPECT_FALSE(fromAtoms({"x"}) == fromAtoms({"x", "y"}));
    EXPECT_FALSE(fromAtoms({"x", "y"}) == fromAtoms({"x", "z"}));
    EXPECT_TRUE(Set() == Set());
    EXPECT_FALSE(Set() == fromAtoms({"a"}));
    Set s = fromAtoms({"a", "b"});
    EXPECT_TRUE(s == s);
}

// ================================================================ + += =====
TEST(SetUnion, DoesNotDuplicateCommonElements)
{
    Set a = fromAtoms({"x", "y"});
    Set b = fromAtoms({"y", "z"});
    Set a0 = a, b0 = b;

    Set u = a + b;
    EXPECT_EQ(u.getCardinality(), 3u);
    EXPECT_TRUE(u == fromAtoms({"x", "y", "z"}));
    EXPECT_TRUE(a == a0);
    EXPECT_TRUE(b == b0); // operands unchanged
}

TEST(SetUnion, IdentityAndIdempotence)
{
    Set b = fromAtoms({"y", "z"});
    EXPECT_TRUE((Set() + b) == b);
    EXPECT_TRUE((b + Set()) == b);
    EXPECT_TRUE((Set() + Set()).isEmpty());
    Set a = fromAtoms({"x", "y"});
    EXPECT_TRUE((a + a) == a);
}

TEST(SetUnion, DisjointSetsAddCardinalities)
{
    EXPECT_EQ((fromAtoms({"p"}) + fromAtoms({"q"})).getCardinality(), 2u);
}

TEST(SetUnion, CommonNestedSetIsNotDuplicated)
{
    Set n1, n2;
    n1.add(A("x"));
    n1.add(S(fromAtoms({"a"})));
    n2.add(S(fromAtoms({"a"})));
    n2.add(A("y"));
    EXPECT_EQ((n1 + n2).getCardinality(), 3u);
}

TEST(SetUnionAssign, MutatesLeftOperandAndReturnsReference)
{
    Set a = fromAtoms({"x", "y"});
    Set b = fromAtoms({"y", "z"});
    Set &ref = (a += b);
    EXPECT_EQ(&ref, &a);
    EXPECT_TRUE(a == fromAtoms({"x", "y", "z"}));
    EXPECT_TRUE(b == fromAtoms({"y", "z"})); // right operand unchanged
}

TEST(SetUnionAssign, SelfUnionAndChaining)
{
    Set a = fromAtoms({"x", "y"});
    a += a;
    EXPECT_EQ(a.getCardinality(), 2u);
    Set b = fromAtoms({"z"});
    Set c = fromAtoms({"w"});
    (a += b) += c;
    EXPECT_EQ(a.getCardinality(), 4u);
    EXPECT_TRUE(a[A("w")]);
    Set e;
    e += Set();
    EXPECT_TRUE(e.isEmpty());
}

// ================================================================ * *= =====
TEST(SetIntersection, KeepsOnlyCommonElements)
{
    Set a = fromAtoms({"x", "y"});
    Set b = fromAtoms({"y", "z"});
    Set a0 = a, b0 = b;
    Set r = a * b;
    EXPECT_EQ(r.getCardinality(), 1u);
    EXPECT_TRUE(r[A("y")]);
    EXPECT_TRUE(a == a0);
    EXPECT_TRUE(b == b0);
}

TEST(SetIntersection, DisjointAndIdentityCases)
{
    EXPECT_TRUE((fromAtoms({"x"}) * fromAtoms({"y"})).isEmpty());
    Set a = fromAtoms({"x", "y"});
    EXPECT_TRUE((a * Set()).isEmpty());
    EXPECT_TRUE((Set() * a).isEmpty());
    EXPECT_TRUE((a * a) == a);
}

TEST(SetIntersection, NestedSetsIntersectByContent)
{
    Set n1, n2;
    n1.add(A("x"));
    n1.add(S(fromAtoms({"a", "b"})));
    n2.add(S(fromAtoms({"b", "a"})));
    n2.add(A("z"));
    Set nr = n1 * n2;
    EXPECT_EQ(nr.getCardinality(), 1u);
    EXPECT_TRUE(nr[S(fromAtoms({"a", "b"}))]);
}

TEST(SetIntersectionAssign, MutatesLeftOperandAndReturnsReference)
{
    Set a = fromAtoms({"x", "y"});
    Set b = fromAtoms({"y", "z"});
    Set &ref = (a *= b);
    EXPECT_EQ(&ref, &a);
    EXPECT_TRUE(a == fromAtoms({"y"}));
    EXPECT_TRUE(b == fromAtoms({"y", "z"}));
    a *= a;
    EXPECT_TRUE(a == fromAtoms({"y"}));
    a *= fromAtoms({"q"});
    EXPECT_TRUE(a.isEmpty());
}

// ================================================================ - -= =====
TEST(SetDifference, RemovesElementsOfOther)
{
    Set a = fromAtoms({"x", "y"});
    Set b = fromAtoms({"y", "z"});
    Set a0 = a, b0 = b;
    Set d = a - b;
    EXPECT_EQ(d.getCardinality(), 1u);
    EXPECT_TRUE(d[A("x")]);
    EXPECT_FALSE(d[A("y")]);
    EXPECT_TRUE(a == a0);
    EXPECT_TRUE(b == b0);
}

TEST(SetDifference, DisjointAndIdentityCases)
{
    Set a = fromAtoms({"x", "y"});
    EXPECT_TRUE((a - fromAtoms({"q"})) == a);
    EXPECT_TRUE((a - a).isEmpty());
    EXPECT_TRUE((a - Set()) == a);
    EXPECT_TRUE((Set() - a).isEmpty());
}

TEST(SetDifference, NestedSetSubtractedByContent)
{
    Set n;
    n.add(A("x"));
    n.add(S(fromAtoms({"a"})));
    Set sub;
    sub.add(S(fromAtoms({"a"})));
    Set nd = n - sub;
    EXPECT_EQ(nd.getCardinality(), 1u);
    EXPECT_TRUE(nd[A("x")]);
}

TEST(SetDifferenceAssign, MutatesLeftOperandAndReturnsReference)
{
    Set a = fromAtoms({"x", "y", "z"});
    Set &ref = (a -= fromAtoms({"y"}));
    EXPECT_EQ(&ref, &a);
    EXPECT_TRUE(a == fromAtoms({"x", "z"}));
    a -= a;
    EXPECT_TRUE(a.isEmpty());
    Set e;
    e -= fromAtoms({"a"});
    EXPECT_TRUE(e.isEmpty());
}

// ============================================================== copying ====
TEST(SetCopy, CopyIsIndependentFromOriginal)
{
    Set a = fromAtoms({"x", "y"});
    Set b = a;
    EXPECT_TRUE(b == a);
    EXPECT_EQ(b.getCardinality(), 2u);

    b.add(A("z"));
    EXPECT_EQ(a.getCardinality(), 2u);
    EXPECT_FALSE(a[A("z")]);

    a.rmv(A("x"));
    EXPECT_TRUE(b[A("x")]);
}

TEST(SetCopy, AssignmentCopiesContent)
{
    Set b = fromAtoms({"x", "y", "z"});
    Set c;
    c = b;
    EXPECT_TRUE(c == b);
}

TEST(SetCopy, NestedSetIsCopiedCorrectly)
{
    Set n;
    n.add(S(fromAtoms({"p"})));
    Set m = n;
    EXPECT_TRUE(m == n);
    EXPECT_EQ(m.getCardinality(), 1u);
}

// ================================================================ parse ====
TEST(SetParse, SimpleAndWhitespace)
{
    EXPECT_TRUE(Set("{a,b,c}") == fromAtoms({"a", "b", "c"}));
    EXPECT_TRUE(Set("{ a ,  b , c }") == fromAtoms({"a", "b", "c"}));
    EXPECT_EQ(Set("{a,\tb,\nc}").getCardinality(), 3u);
    EXPECT_TRUE(Set("{apple, banana}")[A("apple")]);
}

TEST(SetParse, EmptyVariants)
{
    EXPECT_TRUE(Set("{}").isEmpty());
    EXPECT_TRUE(Set("").isEmpty());
    EXPECT_TRUE(Set("   ").isEmpty());
}

TEST(SetParse, TaskExampleHasSixTopLevelElements)
{
    Set p("{a, b, c, {a, b}, {}, {a, {c}}}");
    EXPECT_EQ(p.getCardinality(), 6u);
    EXPECT_TRUE(p[A("a")]);
    EXPECT_TRUE(p[A("b")]);
    EXPECT_TRUE(p[A("c")]);
    EXPECT_TRUE(p[S(Set("{a,b}"))]);
    EXPECT_TRUE(p[S(Set("{}"))]);
    EXPECT_TRUE(p[S(Set("{a,{c}}"))]);
    EXPECT_FALSE(p[S(Set("{c}"))]); // lies deeper, not a top-level element
}

TEST(SetParse, NestingAndDeduplication)
{
    EXPECT_EQ(Set("{{}}").getCardinality(), 1u);
    EXPECT_TRUE(Set("{{}}")[S(Set())]);
    EXPECT_EQ(Set("{{{a}}}").getCardinality(), 1u);
    EXPECT_EQ(Set("{a,a}").getCardinality(), 1u);
    EXPECT_EQ(Set("{{},{}}").getCardinality(), 1u);
    EXPECT_EQ(Set("{{a,b},{b,a}}").getCardinality(), 1u);
}

TEST(SetParse, InvalidFormatThrows)
{
    const char *bad[] = {"{a", "a}", "{a}}", "a,b", "{a}b", "}{",
                          "{a,{b}", "a", ",a", "{a},b", ",", "{a},", "}"};
    for (const char *text : bad)
        EXPECT_THROW(Set s(text), std::invalid_argument) << "input: " << text;
}

// ============================================================== toString ===
TEST(SetToString, RoundTripsThroughParse)
{
    EXPECT_EQ(Set().toString(), "{}");
    EXPECT_EQ(fromAtoms({"a", "b"}).toString(), "{a, b}");
    EXPECT_EQ(Set("{a,{b,c}}").toString(), "{a, {b, c}}");
    EXPECT_EQ(Set("{{}}").toString(), "{{}}");
    Set p("{a, b, c, {a, b}, {}, {a, {c}}}");
    EXPECT_TRUE(Set(p.toString()) == p);
}

// ================================================================ boolean ==
TEST(SetBoolean, EmptySetGivesOneElement)
{
    Set b0 = Set().buildBoolean();
    EXPECT_EQ(b0.getCardinality(), 1u);
    EXPECT_TRUE(b0[S(Set())]);
}

TEST(SetBoolean, SmallSetsMatchExpectedSubsets)
{
    EXPECT_TRUE(fromAtoms({"a"}).buildBoolean() == Set("{{}, {a}}"));

    Set two = fromAtoms({"a", "b"});
    Set b2 = two.buildBoolean();
    EXPECT_EQ(b2.getCardinality(), 4u);
    EXPECT_TRUE(b2 == Set("{{}, {a}, {b}, {a,b}}"));
    EXPECT_FALSE(b2[A("a")]);       // atoms themselves are not in the result
    EXPECT_TRUE(b2[S(two)]);
    EXPECT_TRUE(b2[S(Set())]);
    EXPECT_TRUE(two == fromAtoms({"a", "b"})); // original unchanged

    EXPECT_TRUE(fromAtoms({"a", "b", "c"}).buildBoolean() ==
                Set("{{}, {a}, {b}, {c}, {a,b}, {a,c}, {b,c}, {a,b,c}}"));
    EXPECT_EQ(fromAtoms({"a", "b", "c", "d"}).buildBoolean().getCardinality(), 16u);
}

TEST(SetBoolean, ScalesAsPowerOfTwo)
{
    Set ten;
    for (int i = 0; i < 10; i++)
        ten.add(A("e" + std::to_string(i)));
    EXPECT_EQ(ten.buildBoolean().getCardinality(), 1024u);
}

TEST(SetBoolean, WorksWithNestedElements)
{
    Set nested;
    nested.add(A("a"));
    nested.add(S(fromAtoms({"b"})));
    Set bn = nested.buildBoolean();
    EXPECT_EQ(bn.getCardinality(), 4u);
    EXPECT_TRUE(bn[S(nested)]);
    Set onlyNested;
    onlyNested.add(S(fromAtoms({"b"})));
    EXPECT_TRUE(bn[S(onlyNested)]);
}

TEST(SetBoolean, BooleanOfBoolean)
{
    EXPECT_EQ(fromAtoms({"a"}).buildBoolean().buildBoolean().getCardinality(), 4u);
}

TEST(SetBoolean, TooLargeSetThrowsLengthError)
{
    for (int n : {63, 64})
    {
        Set big;
        for (int i = 0; i < n; i++)
            big.add(A("e" + std::to_string(i)));
        EXPECT_THROW(big.buildBoolean(), std::length_error) << "n=" << n;
    }
}