"""console interface for Turing Machine operation"""
from TM import TuringMachine

def startCli() -> None:
    num = int(input("Turing machine operator. Enter a number to create new machine: "))
    newTM = TuringMachine(num)
    while True:
        try:
            choice = int(input("Choose an operation:\n1) Start new machine\t 2) Show tape\t 3) Perform increment\t 4) Perform decrement\t 5) Show rules\t 6) Show current number\t 7) Change tape\n8) Exit\n"))
        except Exception as error:
            print(f"! {error}. HINT: Input must be an integer.\n")                
            continue
        match choice:
            case 1:  
                try:
                    num = int(input("Turing machine operator. Enter a number to create new machine: "))
                    newTM = TuringMachine(num)               
                except Exception as error:
                    print(f"! {error}. HINT: Input must be an integer.\n")                
                    continue
            case 2:
                newTM.showTape()
            case 3:
                before = str(newTM.theTape)
                newTM.increment()
                print(f"! Increment completed.\nInitial tape: {before}\nNew tape: {newTM.theTape}\nNew number: {newTM.toInt()}\n")
            case 4:
                before = str(newTM.theTape)
                try:
                    newTM.decrement()
                except ValueError as error:
                    print(f"! {error}")
                else:
                    print(f"! Decrement completed.\nInitial tape: {before}\nNew tape: {newTM.theTape}\nNew number: {newTM.toInt()}\n")
            case 5:
                newTM.showRules()
            case 6:
                print(f"! Current number is {newTM.theNum}.\n")
            case 7:
                newTM.changeTape()
                newTM.showTape()
            case 8:
                print("! Bye-bye :)\n")
                return
            case _:
                print("Invalid choice")