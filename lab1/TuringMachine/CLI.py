"""console interface for Turing Machine operation"""
from TM import TuringMachine

class CLI:
    def __init__(self):
        num = int(input("Turing machine operator. Enter a number to create new machine: "))
        newTM = TuringMachine(num)
        while(input()!=5):
            choice = int(input("Choose an operation:\n1) Start new machine\t 2) Show tape 3) Perform increment 4) Perform decrement 5) exit:"))
            choice = int(input("Do you want to perform:\n1) INCREMENT\t or\t 2) DECREMENT ?"))
            if(choice == 1):
                print(f"Increment session completed.\nInitial tape: {newTM.theTape};\nNew tape: {newTM.increment()};\nNew number: {newTM.toInt()}.")
            elif(choice == 2):
                print(f"Increment session completed.\nInitial tape: {newTM.theTape};\nNew tape: {newTM.decrement()};\nNew number: {newTM.toInt()}.")

        