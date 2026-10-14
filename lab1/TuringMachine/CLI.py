"""Console interface for Turing Machine operation."""
from TM import TuringMachine

MENU = (
    "Choose an operation:\n"
    "1) Start new machine\t 2) Show tape\t 3) Perform increment\t 4) Perform decrement\n"
    "5) Show rules\t 6) Show current number\t 7) Change tape\t 8) Exit\n"
)


def askMachine() -> TuringMachine | None:
    """Ask for a number and build a machine; return None on invalid input.

    :return: A new TuringMachine instance initialized with the given number, or None if input was invalid.
    """
    try:
        num = int(input("Enter a non-negative number to create a new machine: "))
        return TuringMachine(num)
    except ValueError as error:     
        print(f"! {error}. HINT: Input must be a non-negative integer.\n")
        return None


def changeTape(machine: TuringMachine) -> None:
    """Display tape digits and prompt the user to flip a specific bit.

    :param machine: The active TuringMachine instance whose tape will be modified.
    """
    print("Enter the index of the bit to flip:")
    for i, bit in enumerate(machine.theTape.digits, start=1):
        print(f"{i}) {bit}")
    try:
        machine.flipBit(int(input()))
    except ValueError as error:     
        print(f"! {error}\n")
    else:
        print(f"! Bit flipped. Tape: {machine.theTape}, number: {machine.theNum}\n")


def startCli() -> None:
    """Start the main command-line interface loop for interacting with the Turing machine."""
    print("Turing machine operator.")
    machine = None
    while machine is None:         
        machine = askMachine()

    while True:
        try:
            choice = int(input(MENU))
        except ValueError as error:
            print(f"! {error}. HINT: Input must be an integer.\n")
            continue

        match choice:
            case 1:
                new = askMachine()
                if new is not None:  
                    machine = new
            case 2:
                machine.showTape()
            case 3:
                before = str(machine.theTape)
                machine.increment()
                print(f"! Increment completed.\nInitial tape: {before}\n"
                      f"New tape: {machine.theTape}\nNew number: {machine.theNum}\n")
            case 4:
                before = str(machine.theTape)
                try:
                    machine.decrement()
                except ValueError as error:
                    print(f"! {error}\n")
                else:
                    print(f"! Decrement completed.\nInitial tape: {before}\n"
                          f"New tape: {machine.theTape}\nNew number: {machine.theNum}\n")
            case 5:
                machine.showRules()
            case 6:
                print(f"! Current number is {machine.theNum}.\n")
            case 7:
                changeTape(machine)
            case 8:
                print("! Bye-bye :)\n")
                return
            case _:
                print("! Invalid choice.\n")