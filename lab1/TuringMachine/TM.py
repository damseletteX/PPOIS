BLANK, LEFT, RIGHT, STAY = None, -1, 1, 0
START, DONE = "S", "Y"


class Tape:
    """ A binary number, least-significant-bit first, surrounded by blanks.

    Invariant (restored by normalize): [BLANK, d0, d1, ..., dn, BLANK],
    with at least one digit and no leading zeros (except the number 0 itself).
    """
    def __init__(self, number: int):
        if number < 0:
            raise ValueError("Only non-negative numbers are supported.")
        self._cells = self.createTape(number)

    @staticmethod
    def createTape(number: int) -> list:
        bits = [int(b) for b in reversed(bin(number)[2:])]
        return [BLANK, *bits, BLANK]

    def read(self, pos: int):
        """ Anything outside the stored cells counts as blank. """
        if 0 <= pos < len(self._cells):
            return self._cells[pos]
        return BLANK

    def write(self, pos: int, symbol) -> None:
        """ Write any symbol; the tape grows to the right on demand. """
        if pos < 0:
            raise IndexError("Head moved off the left end of the tape.")
        while pos >= len(self._cells):
            self._cells.append(BLANK)
        self._cells[pos] = symbol

    def flip(self, pos: int) -> None:
        """ Flip a real digit (0 <-> 1). Borders and out-of-range are rejected. """
        if not 1 <= pos <= len(self._cells) - 2:
            raise ValueError("No such cell.")
        if self._cells[pos] is BLANK:
            raise ValueError("Cannot flip a blank cell.")
        self._cells[pos] ^= 1
        self.normalize()

    def normalize(self) -> None:
        """ Restore the invariant: blanks at both ends, no leading zeros. """
        digits = self.digits
        while len(digits) > 1 and digits[-1] == 0:
            digits.pop()
        self._cells = [BLANK, *digits, BLANK]

    @property
    def digits(self) -> list:
        """ The digits without blanks (a new list, safe to modify). """
        return [c for c in self._cells if c is not BLANK]

    def toInt(self) -> int:
        return sum(bit << i for i, bit in enumerate(self.digits))

    def __len__(self) -> int:
        return len(self.digits)

    def __str__(self) -> str:
        """ Most-significant bit first, like normal binary notation. """
        return "".join(str(d) for d in reversed(self.digits))


class Head:
    """ Holds the state and position, and applies a rules table to a tape. """
    INCREMENT_RULES = {
        (START, 1):     (START, 0, RIGHT),  
        (START, 0):     (DONE,  1, STAY),
        (START, BLANK): (DONE,  1, STAY),  
    }

    DECREMENT_RULES = {
        (START, 0): (START, 1, RIGHT),      
        (START, 1): (DONE,  0, STAY),
    }

    def __init__(self, rules: dict):
        self.rules = rules
        self.state = START
        self.pos = 1                         

    def step(self, tape: Tape) -> None:
        symbol = tape.read(self.pos)
        key = (self.state, symbol)
        if key not in self.rules:
            raise ValueError(f"No rule for state={self.state!r}, symbol={symbol!r}")
        self.state, write, move = self.rules[key]
        tape.write(self.pos, write)
        self.pos += move

    @property
    def halted(self) -> bool:
        return self.state == DONE


class TuringMachine:
    """ Orchestrating machine combining a head and a tape. """
    def __init__(self, num: int):
        self.theTape = Tape(num)

    @property
    def theNum(self) -> int:
        return self.theTape.toInt()        

    def _run(self, rules: dict) -> None:
        head = Head(rules)                 
        while not head.halted:
            head.step(self.theTape)
        self.theTape.normalize()

    def increment(self) -> None:
        self._run(Head.INCREMENT_RULES)

    def decrement(self) -> None:
        if self.theNum == 0:
            raise ValueError("Cannot decrement 0.")
        self._run(Head.DECREMENT_RULES)

    def toInt(self) -> int:
        return self.theTape.toInt()

    def showRules(self) -> None:
        print(f"Increment rules: {Head.INCREMENT_RULES}\n"
              f"Decrement rules: {Head.DECREMENT_RULES}")

    def showTape(self) -> None:
        print(f"Tape (LSB first, index from 1): {list(reversed(str(self.theTape)))}\n"
              f"As binary: {self.theTape}, which represents {self.theNum}.")

    def flipBit(self, index: int) -> None:
        self.theTape.flip(index)

    def changeTape(self) -> None:
        print("Enter the index of the bit to flip:")
        for i, bit in enumerate(self.theTape.digits, start=1):
            print(f"{i}) {bit}")
        try:
            self.flipBit(int(input()))
        except ValueError as error:       
            print(f"! {error}")