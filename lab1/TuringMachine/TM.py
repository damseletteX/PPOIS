BLANK, LEFT, RIGHT, STAY = None, -1, 1, 0
START, REVERSE, DONE = "S", "R", "Y"

class Tape:
    """ A tape of 0s or 1s. """
    def __init__(self, number: int):
        self._tape = self.createTape(number)
        
    @staticmethod      
    def createTape(number: int) -> list:
        tape = list(int(bit) for bit in bin(number)[2:])[::-1]
        return [BLANK, *tape, BLANK]
    
    def flip(self, symbol: int) -> None:
        self._tape[self._pos] = symbol
    
class Head:
    """ Head reads a cell from the tape, stores state, and chooses the direction """
    INCREMENT_RULES = {
    (START, 1) : (START, 0, RIGHT),
    (START, 0) : (DONE, 1, STAY),
    (START, BLANK): (DONE, 1, STAY),
    }
    
    DECREMENT_RULES = {
    (START, 0): (START, 1, RIGHT),
    (START, 1): (DONE,  0, STAY),
    }
    
    def __init__(self):
        self.state = START
        self.pos = 1
        
    def move(self, direction: int):
        self.pos += direction

    def read(self, tape):
        return tape[self.pos]

    def revert(self):
        self.state = START

    def step(self, tape: Tape, Op: bool) -> list: 
        symbol = self.read()
        if(Op): #If op==true it's increment
            if(self.state==START & symbol==BLANK):
                tape._tape.append(BLANK)
            self.state, flipped, move = self.INCREMENT_RULES[(self.state, symbol)]
            tape.flip(flipped)
            self.move(move)
        else:   #if op == false it's decrement
            self.state, flipped, move = self.DECREMENT_RULES[(self.state, symbol)]
            tape.flip(flipped)
            tape.move(move)
        return tape
                    
class TuringMachine:
    """ Orchestrating machine combining a head and a tape. """
    def __init__(self, num: int):
        self.theNum = num
        self.theTape = Tape(self.theNum)
        self.q = Head()
        
    def exit(self, char: str):
        print("Bye-bye!\n")
        return
        
    def increment(self) -> list[int]:
        while self.q.state != DONE:
            self.q.step()
        return 
            
    def decrement(self) -> list[int]:
        while(self.q.state != DONE):
            self.q.step()
    
    def showRules(self) -> None:
        print(f"Default increment rules are: {self.q.INCREMENT_RULES}.\nDefault decrement rules are: {self.q.DECREMENT_RULES}.")
    
    def showTape(self):
        print(f"Current tape is {self.theTape},\n which represents the number {self.theNum}.\n")
    
    def changeTape(self):
        print("Enter the index of the number you want to flip, or erase:\n")
        for i in len(self.theTape):
            print(f"{i}) {self.theTape[i]}\t")
        choice = int(input())
        self.theTape[choice] += (self.theTape[choice] == 1)
        
    def toInt(self) -> int:
        integer = int(str(self.theTape), 2)
        return integer