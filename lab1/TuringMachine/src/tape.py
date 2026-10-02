import copy
import json
from io import TextIOBase


class Tape:
    """A sparse one-dimensional Turing machine tape."""

    DEFAULT_BLANK = "_"
    VALID_SYMBOLS = frozenset("01_")

    def __init__(self, content: str = "", blank_symbol: str = DEFAULT_BLANK):
        if blank_symbol != self.DEFAULT_BLANK:
            raise ValueError("Blank symbol must be '_'.")
        if any(symbol not in self.VALID_SYMBOLS for symbol in content):
            raise ValueError("Tape content may contain only 0 and 1.")
        self._blank_symbol = blank_symbol
        self._cells = {
            position: symbol for position, symbol in enumerate(content)
            if symbol != blank_symbol
        }

    @property
    def blank_symbol(self) -> str:
        return self._blank_symbol

    def read(self, position: int) -> str:
        return self._cells.get(position, self._blank_symbol)

    def write(self, position: int, symbol: str) -> None:
        if symbol not in self.VALID_SYMBOLS:
            raise ValueError("Tape symbol must be 0, 1 or _.")
        if symbol == self._blank_symbol:
            self._cells.pop(position, None)
        else:
            self._cells[position] = symbol

    def to_binary(self) -> str:
        if not self._cells:
            return "0"
        left = min(self._cells)
        right = max(self._cells)
        return "".join(self.read(position) for position in range(left, right + 1))

    def copy(self) -> "Tape":
        return copy.copy(self)

    def copy_from(self, other: "Tape") -> None:
        if not isinstance(other, Tape):
            raise TypeError("Expected Tape.")
        self._blank_symbol = other._blank_symbol
        self._cells = other._cells.copy()

    def __copy__(self) -> "Tape":
        result = type(self)(blank_symbol=self._blank_symbol)
        result._cells = self._cells.copy()
        return result

    def __eq__(self, other: object) -> bool:
        return isinstance(other, Tape) and (
            self._blank_symbol == other._blank_symbol
            and self._cells == other._cells
        )

    def __ne__(self, other: object) -> bool:
        return not self == other

    def to_text(self) -> str:
        data = {"blank": self._blank_symbol, "cells": self._cells}
        return json.dumps(data, sort_keys=True)

    @classmethod
    def from_text(cls, text: str) -> "Tape":
        data = json.loads(text)
        tape = cls(blank_symbol=data["blank"])
        tape._cells = {int(key): value for key, value in data["cells"].items()}
        return tape

    def write_to(self, stream: TextIOBase) -> None:
        stream.write(self.to_text())

    @classmethod
    def read_from(cls, stream: TextIOBase) -> "Tape":
        return cls.from_text(stream.read())
