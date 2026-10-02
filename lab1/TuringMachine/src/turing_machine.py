import copy
import json
from io import TextIOBase

from tape import Tape


class TuringMachine:
    """Turing machine that increments a binary number by one."""

    START_STATE = "start"
    INCREMENT_STATE = "increment"
    HALT_STATE = "halt"

    def __init__(self, binary_number: str = "0"):
        self._validate_binary(binary_number)
        self._tape = Tape(binary_number)
        self._head = 0
        self._state = self.START_STATE
        self._steps = 0

    @staticmethod
    def _validate_binary(binary_number: str) -> None:
        if not binary_number or any(symbol not in "01" for symbol in binary_number):
            raise ValueError("Binary number must contain only 0 and 1.")
        if len(binary_number) > 1 and binary_number.startswith("0"):
            raise ValueError("Leading zeros are not allowed.")

    @property
    def state(self) -> str:
        return self._state

    @property
    def head(self) -> int:
        return self._head

    @property
    def steps(self) -> int:
        return self._steps

    @property
    def value(self) -> str:
        return self._tape.to_binary()

    def run(self) -> str:
        while self._state != self.HALT_STATE:
            self.step()
        return self.value

    def step(self) -> str:
        if self._state == self.HALT_STATE:
            return self.value
        if self._state == self.START_STATE:
            self._move_to_right_blank()
            self._state = self.INCREMENT_STATE
        else:
            self._perform_increment()
        self._steps += 1
        return self.value

    def _move_to_right_blank(self) -> None:
        while self._tape.read(self._head) != self._tape.blank_symbol:
            self._head += 1

    def _perform_increment(self) -> None:
        symbol = self._tape.read(self._head - 1)
        self._head -= 1
        if symbol == "0":
            self._tape.write(self._head, "1")
            self._state = self.HALT_STATE
            return
        self._tape.write(self._head, "0")
        if self._head < 0:
            self._tape.write(self._head, "1")
            self._state = self.HALT_STATE

    def copy(self) -> "TuringMachine":
        return copy.copy(self)

    def copy_from(self, other: "TuringMachine") -> None:
        if not isinstance(other, TuringMachine):
            raise TypeError("Expected TuringMachine.")
        self._tape = other._tape.copy()
        self._head = other._head
        self._state = other._state
        self._steps = other._steps

    def __copy__(self) -> "TuringMachine":
        result = type(self)(self.value)
        result._tape = self._tape.copy()
        result._head = self._head
        result._state = self._state
        result._steps = self._steps
        return result

    def __eq__(self, other: object) -> bool:
        return isinstance(other, TuringMachine) and (
            self._tape == other._tape
            and self._head == other._head
            and self._state == other._state
            and self._steps == other._steps
        )

    def __ne__(self, other: object) -> bool:
        return not self == other

    def to_text(self) -> str:
        data = {
            "tape": self._tape.to_text(),
            "head": self._head,
            "state": self._state,
            "steps": self._steps,
        }
        return json.dumps(data, sort_keys=True)

    @classmethod
    def from_text(cls, text: str) -> "TuringMachine":
        data = json.loads(text)
        machine = cls("0")
        machine._tape = Tape.from_text(data["tape"])
        machine._head = data["head"]
        machine._state = data["state"]
        machine._steps = data["steps"]
        return machine

    def write_to(self, stream: TextIOBase) -> None:
        stream.write(self.to_text())

    @classmethod
    def read_from(cls, stream: TextIOBase) -> "TuringMachine":
        return cls.from_text(stream.read())
