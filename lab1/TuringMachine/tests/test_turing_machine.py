from copy import copy
from io import StringIO

import pytest

from turing_machine import TuringMachine


@pytest.mark.parametrize(
    ("source", "expected"),
    [("0", "1"), ("1", "10"), ("10", "11"), ("101", "110"), ("111", "1000")],
)
def test_increment(source, expected):
    machine = TuringMachine(source)
    assert machine.run() == expected
    assert machine.state == TuringMachine.HALT_STATE


def test_step_by_step():
    machine = TuringMachine("10")
    assert machine.step() == "10"
    assert machine.state == TuringMachine.INCREMENT_STATE
    assert machine.step() == "11"
    assert machine.step() == "11"


def test_copy_assignment_and_equality():
    original = TuringMachine("101")
    original.step()
    copied = original.copy()
    shallow = copy(original)
    assert copied == original == shallow
    copied.run()
    assert copied != original
    original.copy_from(copied)
    assert original == copied
    assert original != object()
    with pytest.raises(TypeError):
        original.copy_from("bad")


def test_text_and_stream_io():
    machine = TuringMachine("111")
    machine.step()
    text = machine.to_text()
    restored = TuringMachine.from_text(text)
    assert restored == machine
    stream = StringIO()
    machine.write_to(stream)
    assert TuringMachine.read_from(StringIO(stream.getvalue())) == machine


def test_invalid_binary():
    for value in ("", "2", "10a", "00", "01"):
        with pytest.raises(ValueError):
            TuringMachine(value)
