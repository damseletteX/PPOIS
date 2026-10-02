from io import StringIO
from copy import copy

import pytest

from tape import Tape


def test_tape_read_write_and_blank():
    tape = Tape("101")
    assert tape.read(0) == "1"
    assert tape.read(10) == "_"
    tape.write(1, "0")
    assert tape.to_binary() == "101"
    tape.write(1, "_")
    assert tape.read(1) == "_"
    assert tape.to_binary() == "1_1"


def test_tape_invalid_symbol_and_blank():
    with pytest.raises(ValueError):
        Tape("102")
    with pytest.raises(ValueError):
        Tape("1", "0")
    with pytest.raises(ValueError):
        Tape("1").write(0, "x")


def test_tape_copy_and_equality():
    first = Tape("101")
    second = first.copy()
    third = copy(first)
    assert first == second == third
    second.write(0, "0")
    assert first != second
    assert first != object()
    assert first == third
    first.copy_from(second)
    assert first == second
    with pytest.raises(TypeError):
        first.copy_from("bad")


def test_tape_text_and_stream_io():
    source = Tape("101")
    text = source.to_text()
    restored = Tape.from_text(text)
    assert restored == source
    stream = StringIO()
    source.write_to(stream)
    assert Tape.read_from(StringIO(stream.getvalue())) == source
