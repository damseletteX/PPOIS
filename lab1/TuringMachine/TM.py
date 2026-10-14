"""Binary increment/decrement Turing machine: tape, head and orchestrating machine."""

BLANK, LEFT, RIGHT, STAY = None, -1, 1, 0
START, DONE = "S", "Y"

Rules = dict[tuple[str, int | None], tuple[str, int | None, int]]
INCREMENT_RULES: Rules = {
    (START, 1):     (START, 0, RIGHT), 
    (START, 0):     (DONE,  1, STAY),  
    (START, BLANK): (DONE,  1, STAY),  
}

DECREMENT_RULES: Rules = {
    (START, 0): (START, 1, RIGHT),     
    (START, 1): (DONE,  0, STAY),       
}

class Tape:
    """A binary number, least-significant-bit first, surrounded by blanks.

    Invariant (restored by normalize): [BLANK, d0, d1, ..., dn, BLANK],
    with at least one digit and no leading zeros (except the number 0 itself).
    """

    def __init__(self, number: int):
        """Initialize the tape with a non-negative integer representation.

        :param number: Decimal integer to convert to binary tape.
        :raises ValueError: If the number is negative.
        """
        if number < 0:
            raise ValueError("Only non-negative numbers are supported.")
        self._cells = self.createTape(number)

    @staticmethod
    def createTape(number: int) -> list:
        """Create a list of tape cells from a decimal number (LSB first).

        :param number: Decimal integer.
        :raises ValueError: None directly, but expects non-negative ints.
        :return: List representing the tape with boundary BLANKs.
        """
        bits = [int(b) for b in reversed(bin(number)[2:])]
        return [BLANK, *bits, BLANK]

    def read(self, pos: int):
        """Read the symbol at the specified tape position.

        :param pos: Index on the tape.
        :return: The symbol (0, 1, or BLANK).
        """
        """Anything outside the stored cells counts as blank."""
        if 0 <= pos < len(self._cells):
            return self._cells[pos]
        return BLANK

    def write(self, pos: int, symbol) -> None:
        """Write a symbol at the given position, expanding the tape if necessary.

        :param pos: Target position index.
        :param symbol: Symbol to write (0, 1, or BLANK).
        :raises IndexError: If the head moves off the left end (pos < 0).
        """
        """Write any symbol; the tape grows to the right on demand."""
        if pos < 0:
            raise IndexError("Head moved off the left end of the tape.")
        while pos >= len(self._cells):
            self._cells.append(BLANK)
        self._cells[pos] = symbol

    def flip(self, pos: int) -> None:
        """Flip a real digit (0 <-> 1) at the given position.

        :param pos: Index of the cell to flip.
        :raises ValueError: If position is out of bounds or points to a blank cell.
        """
        """Flip a real digit (0 <-> 1). Borders and out-of-range are rejected."""
        if not 1 <= pos <= len(self._cells) - 2:
            raise ValueError("No such cell.")
        if self._cells[pos] is BLANK:
            raise ValueError("Cannot flip a blank cell.")
        self._cells[pos] ^= 1
        self.normalize()

    def normalize(self) -> None:
        """Restore the tape invariant: blanks at both ends, no leading zeros."""
        digits = self.digits
        while len(digits) > 1 and digits[-1] == 0:
            digits.pop()
        self._cells = [BLANK, *digits, BLANK]

    @property
    def digits(self) -> list:
        """Get the digits without boundary blanks (safe-to-modify copy)."""
        return [c for c in self._cells if c is not BLANK]

    def toInt(self) -> int:
        """Convert the tape contents back to a decimal integer."""
        return sum(bit << i for i, bit in enumerate(self.digits))

    def __len__(self) -> int:
        """Return the number of significant binary digits."""
        return len(self.digits)

    def __str__(self) -> str:
        """Return the binary string representation (Most-significant bit first)."""
        return "".join(str(d) for d in reversed(self.digits))


class Head:
    """Holds the state and position, and applies a rules table to a tape."""

    def __init__(self, rules: Rules):
        """Initialize the Turing machine head with a given ruleset.

        :param rules: Dictionary mapping (state, symbol) to (new_state, write_symbol, move_direction).
        """
        self.rules = rules
        self.state = START
        self.pos = 1

    def step(self, tape: Tape) -> None:
        """Execute a single transition step on the provided tape.

        :param tape: The Tape instance being operated on.
        :raises ValueError: If no rule matches the current state and scanned symbol.
        """
        symbol = tape.read(self.pos)
        key = (self.state, symbol)
        if key not in self.rules:
            raise ValueError(f"No rule for state={self.state!r}, symbol={symbol!r}")
        self.state, write, move = self.rules[key]
        tape.write(self.pos, write)
        self.pos += move

    @property
    def halted(self) -> bool:
        """Check whether the head has reached the terminal state."""
        return self.state == DONE


class TuringMachine:
    """Orchestrating machine combining a head and a tape."""

    def __init__(self, num: int):
        """Initialize the Turing machine with an initial decimal number.

        :param num: Initial non-negative integer.
        """
        self.theTape = Tape(num)

    @property
    def theNum(self) -> int:
        """The current number, always derived from the tape (never stale)."""
        return self.theTape.toInt()

    def _run(self, rules: dict) -> None:
        """Execute the machine loop using the specified rule set until halted."""
        head = Head(rules)                 
        while not head.halted:
            head.step(self.theTape)
        self.theTape.normalize()

    def increment(self) -> None:
        """Increment the stored binary number by 1 using the Turing rules."""
        self._run(INCREMENT_RULES)

    def decrement(self) -> None:
        """Decrement the stored binary number by 1.

        :raises ValueError: If attempting to decrement 0.
        """
        if self.theNum == 0:
            raise ValueError("Cannot decrement 0.")
        self._run(DECREMENT_RULES)

    def showRules(self) -> None:
        """Print the current increment and decrement rule tables to stdout."""
        print(f"Increment rules: {INCREMENT_RULES}\n"
            f"Decrement rules: {DECREMENT_RULES}")

    def toInt(self) -> int:
        """Return the decimal representation of the tape's value."""
        return self.theTape.toInt()

    def flipBit(self, index: int) -> None:
        """Manually flip a specific bit on the tape by its index.

        :param index: Target bit index.
        """
        self.theTape.flip(index)

    def showTape(self) -> None:
        """Print the current state of the tape, binary string, and decimal value."""
        print(f"Tape (LSB first, index from 1): {self.theTape.digits}\n"
              f"As binary: {self.theTape}, which represents {self.theNum}.")