/** @brief Set console interface */

#include "set.h"
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

static SetContainer container;

static std::string trim(const std::string &s) {
  const char *ws = " \t\n\r";
  size_t b = s.find_first_not_of(ws);
  if (b == std::string::npos)
    return "";
  size_t e = s.find_last_not_of(ws);
  return s.substr(b, e - b + 1);
}

static void checkName(const std::string &name) {
  if (name.empty() || name.find_first_of(" \t") != std::string::npos)
    throw std::invalid_argument(
        "Name must not be empty and must not contain spaces.");
}

static Element parseElement(const std::string &raw) {
  std::string t = trim(raw);
  if (t.empty())
    throw std::invalid_argument("Element cannot be empty.");
  if (t.front() == '{')
    return Element(Set(t));
  if (t.find_first_of("{}, \t") != std::string::npos)
    throw std::invalid_argument(
        "Atom must not contain characters { } , and spaces.");
  return Element(t);
}

static std::string ask(const std::string &question) {
  std::cout << question;
  std::string line;
  std::getline(std::cin, line);
  return trim(line);
}

static void printSet(const std::string &name, const Set &s) {
  std::cout << name << " = " << s.toString() << "  (cardinality "
            << s.getCardinality() << ")\n";
}

static void createNewSet() {
  std::string name = ask("New set name: ");
  checkName(name);
  std::string text = ask("Set (e.g. {a, b, {c}}): ");
  if (text.empty())
    throw std::invalid_argument("Set string is empty.");
  bool existed = container.createFromString(name, text);
  std::cout << (existed ? "Overwritten: " : "Created: ") << name << " = "
            << container.get(name).toString() << "\n";
}

static void addElement() {
  std::string name = ask("Set name: ");
  Element e = parseElement(ask("Element (atom or {set}): "));
  container.addElement(name, e);
  std::cout << "Element added. Current: " << container.get(name).toString()
            << "\n";
}

static void removeElement() {
  std::string name = ask("Set name: ");
  Element e = parseElement(ask("Element (atom or {set}): "));
  container.removeElement(name, e);
  std::cout << "Element removed. Current: " << container.get(name).toString()
            << "\n";
}

static void printSetCardinality() {
  std::string name = ask("Set name: ");
  std::cout << "Cardinality of " << name << ": " << container.cardinality(name)
            << "\n";
}

static void checkElementMembership() {
  std::string name = ask("Set name: ");
  Element e = parseElement(ask("Element (atom or {set}): "));
  std::cout << "Element "
            << (container.isMember(name, e) ? "belongs" : "does not belong")
            << " to set " << name << ".\n";
}

static void performUnion() {
  std::string firstName = ask("First set: ");
  std::string secondName = ask("Second set: ");
  std::string resultName =
      ask("Result name (leave empty to modify the first set in place): ");
  if (resultName.empty()) {
    container.uniteInPlace(firstName, secondName);
    printSet("Result", container.get(firstName));
    return;
  }
  checkName(resultName);
  Set result = container.unite(firstName, secondName);
  container.store(resultName, result);
  printSet(resultName, result);
}

static void performIntersection() {
  std::string firstName = ask("First set: ");
  std::string secondName = ask("Second set: ");
  std::string resultName =
      ask("Result name (leave empty to modify the first set in place): ");
  if (resultName.empty()) {
    container.intersectInPlace(firstName, secondName);
    printSet("Result", container.get(firstName));
    return;
  }
  checkName(resultName);
  Set result = container.intersect(firstName, secondName);
  container.store(resultName, result);
  printSet(resultName, result);
}

static void performDifference() {
  std::string firstName = ask("First set: ");
  std::string secondName = ask("Second set: ");
  std::string resultName =
      ask("Result name (leave empty to modify the first set in place): ");
  if (resultName.empty()) {
    container.differenceInPlace(firstName, secondName);
    printSet("Result", container.get(firstName));
    return;
  }
  checkName(resultName);
  Set result = container.difference(firstName, secondName);
  container.store(resultName, result);
  printSet(resultName, result);
}

static const size_t kMaxBooleanSize =
    20; // 2^20 subsets — reasonable limit for console

static void printPowerSet() {
  std::string name = ask("Set name: ");
  if (container.cardinality(name) > kMaxBooleanSize)
    throw std::invalid_argument("Set is too large for power set (maximum " +
                                std::to_string(kMaxBooleanSize) +
                                " elements).");
  std::string resultName = ask("Power set name: ");
  checkName(resultName);
  Set powerSet = container.powerSet(name);
  container.store(resultName, powerSet);
  printSet(resultName, powerSet);
}

static void printAllSets() {
  if (container.empty()) {
    std::cout << "No sets yet.\n";
    return;
  }
  for (const auto &entry : container.all())
    printSet(entry.first, entry.second);
}

int main() {
  int choice;
  do {
    std::cout << "\n{    Menu    }\n";
    std::cout << "{ 1. Create a new set               }\n";
    std::cout << "{ 2. Add an element to a set        }\n";
    std::cout << "{ 3. Remove an element from a set   }\n";
    std::cout << "{ 4. Get set cardinality            }\n";
    std::cout << "{    5. Check element membership    }\n";
    std::cout << "{    6. Unite two sets              }" << std::endl;
    std::cout << "{    7. Intersect two sets          }" << std::endl;
    std::cout << "{    8. Difference of two sets      }" << std::endl;
    std::cout << "{    9. Build power set             }" << std::endl;
    std::cout << "{    10. Show all sets              }" << std::endl;
    std::cout << "{    0. Exit                        }" << std::endl;
    std::cout << "+--> Enter operation number: ";

    std::cin >> choice;

    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Please enter a number from 0 to 10." << std::endl;
      continue;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    try {
      switch (choice) {
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
        std::cout << "Exiting program." << std::endl;
        break;
      default:
        std::cout << "Please enter a valid operation number." << std::endl;
        break;
      }
    } catch (const std::exception &e) {
      std::cout << "Error: " << e.what() << std::endl;
    }
  } while (choice != 0);

  return 0;
}