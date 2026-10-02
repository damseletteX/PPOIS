from turing_machine import TuringMachine

EXIT_COMMAND = "0"
INCREMENT_COMMAND = "1"
STATUS_COMMAND = "2"


def run_cli() -> None:
    machine = TuringMachine()
    while True:
        _print_menu(machine)
        command = input("Выберите пункт: ").strip()
        if command == EXIT_COMMAND:
            print("Работа завершена.")
            return
        if command == INCREMENT_COMMAND:
            _increment(machine)
        elif command == STATUS_COMMAND:
            _print_status(machine)
        else:
            print("Неизвестная команда.")


def _print_menu(machine: TuringMachine) -> None:
    print("\n=== Машина Тьюринга: +1 к двоичному числу ===")
    print(f"Текущее значение: {machine.value}")
    print("1. Ввести двоичное число и увеличить его на 1")
    print("2. Показать состояние машины")
    print("0. Выход")


def _increment(machine: TuringMachine) -> None:
    value = input("Введите двоичное число: ").strip()
    try:
        new_machine = TuringMachine(value)
        result = new_machine.run()
        machine.copy_from(new_machine)
        print(f"Результат: {result}")
    except ValueError as error:
        print(f"Ошибка: {error}")


def _print_status(machine: TuringMachine) -> None:
    print(f"Состояние: {machine.state}")
    print(f"Позиция головки: {machine.head}")
    print(f"Количество шагов: {machine.steps}")
    print(f"Лента: {machine.value}")
