from cli import run_cli


def test_cli_increment_and_exit(monkeypatch, capsys):
    inputs = iter(["1", "101", "2", "0"])
    monkeypatch.setattr("builtins.input", lambda _: next(inputs))
    run_cli()
    output = capsys.readouterr().out
    assert "Результат: 110" in output
    assert "Состояние: halt" in output


def test_cli_invalid_command_and_bad_input(monkeypatch, capsys):
    inputs = iter(["9", "1", "102", "0"])
    monkeypatch.setattr("builtins.input", lambda _: next(inputs))
    run_cli()
    output = capsys.readouterr().out
    assert "Неизвестная команда." in output
    assert "Ошибка:" in output
