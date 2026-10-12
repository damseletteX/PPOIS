"""Unit tests for TM.py (no CLI involved)."""
import pytest

from TM import (
    BLANK,
    DECREMENT_RULES,
    DONE,
    INCREMENT_RULES,
    LEFT,
    RIGHT,
    START,
    STAY,
    Head,
    Tape,
    TuringMachine,
)

# ---------------------------------------------------------------- Tape

def test_create_tape_is_lsb_first_with_blank_borders():
    assert Tape.createTape(5) == [BLANK, 1, 0, 1, BLANK]


def test_create_tape_zero():
    assert Tape.createTape(0) == [BLANK, 0, BLANK]


def test_negative_number_rejected():
    with pytest.raises(ValueError, match="non-negative"):
        Tape(-1)


def test_read_inside_and_outside_tape():
    t = Tape(5)
    assert t.read(1) == 1
    assert t.read(0) is BLANK
    assert t.read(100) is BLANK        # off the right end counts as blank
    assert t.read(-1) is BLANK         # off the left end counts as blank


def test_write_overwrites_cell():
    t = Tape(5)
    t.write(2, 1)
    assert t.digits == [1, 1, 1]


def test_write_grows_tape_to_the_right():
    t = Tape(1)
    t.write(5, 1)
    assert t.read(5) == 1
    assert t.read(3) is BLANK


def test_write_left_of_tape_raises():
    with pytest.raises(IndexError):
        Tape(1).write(-1, 1)


def test_flip_changes_digit():
    t = Tape(5)                        # 101
    t.flip(1)
    assert t.toInt() == 4


def test_flip_top_bit_normalizes_leading_zeros():
    t = Tape(5)                        # 101 -> 001 -> 1
    t.flip(3)
    assert str(t) == "1"
    assert t.toInt() == 1


def test_flip_twice_restores_number():
    t = Tape(6)
    t.flip(2)
    t.flip(2)
    assert t.toInt() == 6


@pytest.mark.parametrize("pos", [0, 4, 99, -1])
def test_flip_rejects_borders_and_out_of_range(pos):
    with pytest.raises(ValueError, match="No such cell"):
        Tape(5).flip(pos)


def test_flip_rejects_blank_inside_range():
    t = Tape(5)
    t.write(2, BLANK)                  # make a blank between digits
    with pytest.raises(ValueError, match="blank"):
        t.flip(2)


def test_normalize_strips_leading_zeros():
    t = Tape(1)
    t.write(2, 0)                      # digits 1,0
    t.normalize()
    assert t.digits == [1]


def test_normalize_restores_blank_borders():
    t = Tape(1)
    t.write(2, 1)                      # tape has no trailing blank now
    t.normalize()
    assert t.read(0) is BLANK
    assert t.read(len(t) + 1) is BLANK
    assert t.toInt() == 3


def test_normalize_keeps_single_zero():
    t = Tape(0)
    t.normalize()
    assert t.digits == [0]


def test_normalize_all_zeros_collapse_to_one_zero():
    t = Tape(1)
    t.write(1, 0)
    t.write(2, 0)
    t.write(3, 0)
    t.normalize()
    assert t.digits == [0]


def test_digits_returns_copy():
    t = Tape(5)
    t.digits.append(1)
    assert t.digits == [1, 0, 1]


def test_len_str_toint():
    t = Tape(6)                        # 110
    assert len(t) == 3
    assert str(t) == "110"
    assert t.toInt() == 6


def test_str_of_zero():
    assert str(Tape(0)) == "0"


@pytest.mark.parametrize("n", range(70))
def test_tape_roundtrip(n):
    assert Tape(n).toInt() == n
    assert int(str(Tape(n)), 2) == n


# ---------------------------------------------------------------- Head

def test_head_initial_state():
    h = Head(INCREMENT_RULES)
    assert h.state == START
    assert h.pos == 1
    assert not h.halted


def test_carry_then_halt():
    t, h = Tape(1), Head(INCREMENT_RULES)
    h.step(t)                          # reads 1: write 0, move right
    assert (h.state, h.pos) == (START, 2)
    h.step(t)                          # reads blank: write 1, halt
    assert h.halted
    assert t.toInt() == 2


def test_increment_stops_at_first_zero():
    t, h = Tape(2), Head(INCREMENT_RULES)    # 10
    h.step(t)
    assert h.halted
    assert h.pos == 1                  # STAY: head did not move
    assert t.toInt() == 3


def test_decrement_borrow():
    t, h = Tape(4), Head(DECREMENT_RULES)    # 100
    while not h.halted:
        h.step(t)
    t.normalize()
    assert t.toInt() == 3


def test_missing_rule_raises():
    t, h = Tape(0), Head(DECREMENT_RULES)
    h.step(t)                          # 0 -> 1, move right
    with pytest.raises(ValueError, match="No rule"):
        h.step(t)                      # blank has no decrement rule


def test_custom_rules_and_stay():
    h = Head({(START, 1): (DONE, 1, STAY)})
    h.step(Tape(1))
    assert h.halted and h.pos == 1


def test_custom_rules_and_right():
    h = Head({(START, 1): (START, 1, RIGHT)})
    h.step(Tape(1))
    assert h.pos == 2 and not h.halted


def test_custom_rules_and_left():
    h = Head({(START, 1): (START, 1, LEFT)})
    h.step(Tape(1))
    assert h.pos == 0 and not h.halted


def test_head_running_off_left_end_raises():
    rules = {
        (START, 1):     (START, 1, LEFT),
        (START, BLANK): (START, 1, LEFT),
    }
    t, h = Tape(1), Head(rules)
    h.step(t)                          # pos 1 -> 0
    h.step(t)                          # blank at 0 is overwritten, pos -> -1
    with pytest.raises(IndexError):
        h.step(t)                      # writing at -1 is impossible


# ------------------------------------------------------- TuringMachine

@pytest.mark.parametrize("n", [0, 1, 2, 3, 7, 8, 255, 1023])
def test_increment(n):
    m = TuringMachine(n)
    m.increment()
    assert m.toInt() == n + 1


@pytest.mark.parametrize("n", [1, 2, 3, 8, 256, 1024])
def test_decrement(n):
    m = TuringMachine(n)
    m.decrement()
    assert m.toInt() == n - 1


@pytest.mark.parametrize("n", range(300))
def test_increment_matches_integer_arithmetic(n):
    m = TuringMachine(n)
    m.increment()
    assert m.theNum == n + 1
    assert str(m.theTape) == bin(n + 1)[2:]


@pytest.mark.parametrize("n", range(1, 300))
def test_decrement_matches_integer_arithmetic(n):
    m = TuringMachine(n)
    m.decrement()
    assert m.theNum == n - 1
    assert str(m.theTape) == bin(n - 1)[2:]    # no leading zeros


@pytest.mark.parametrize("n", range(40))
def test_increment_then_decrement_roundtrip(n):
    m = TuringMachine(n)
    m.increment()
    m.decrement()
    assert m.theNum == n


def test_decrement_zero_raises_and_keeps_tape():
    m = TuringMachine(0)
    with pytest.raises(ValueError, match="Cannot decrement 0"):
        m.decrement()
    assert m.theNum == 0
    assert str(m.theTape) == "0"


def test_negative_machine_rejected():
    with pytest.raises(ValueError):
        TuringMachine(-5)


def test_repeated_runs_use_fresh_head():
    m = TuringMachine(5)
    m.increment()
    m.increment()
    m.decrement()
    assert m.toInt() == 6


def test_many_increments_in_a_row():
    m = TuringMachine(0)
    for _ in range(100):
        m.increment()
    assert m.theNum == 100


def test_no_leading_zero_after_decrement():
    m = TuringMachine(8)               # 1000 -> 0111 -> 111
    m.decrement()
    assert str(m.theTape) == "111"


def test_the_num_is_derived_from_tape():
    m = TuringMachine(5)
    m.flipBit(1)
    assert m.theNum == 4
    assert m.toInt() == m.theNum


def test_flip_bit_then_increment():
    m = TuringMachine(5)               # 101
    m.flipBit(2)                       # 111 = 7
    m.increment()
    assert m.theNum == 8


def test_flip_bit_invalid_index():
    m = TuringMachine(5)
    with pytest.raises(ValueError):
        m.flipBit(0)
    assert m.theNum == 5               # tape untouched after the error


def test_show_tape_output(capsys):
    TuringMachine(5).showTape()
    out = capsys.readouterr().out
    assert "[1, 0, 1]" in out          # LSB first, no quotes, no None
    assert "As binary: 101" in out
    assert "represents 5" in out


def test_show_rules_output(capsys):
    TuringMachine(1).showRules()
    out = capsys.readouterr().out
    assert "Increment rules" in out
    assert "Decrement rules" in out