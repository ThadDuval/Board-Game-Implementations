#!/usr/bin/env python
import struct, string, math, sys

#this will be the game object your player will manipulate
class SudokuBoard:
    
    #the constructor for the SudokuBoard
    def __init__(self, size, board):
        self.BoardSize = size #the size of the board
        self.CurrentGameboard= board #the current state of the game board
        self.ConsistencyChecks = 0
        self.FCArray = [None] * (size * size)
        self.Assignments = []
    
    #This function will create a new sudoku board object with
    #with the input value placed on the GameBoard row and col are
    #both zero-indexed
    def set_value(self, row, col, value):
        self.CurrentGameboard[row][col]=value #add the value to the appropriate position on the board
        return SudokuBoard(self.BoardSize, self.CurrentGameboard) #return a new board of the same size with the value added

    #FUNCTIONS THAT DEAL WITH FC ARRAY below  ; these functions update the global variables Assignments and Foward Checking Array
    def removeValuesfromRCSSqinFC(self, squareToBaseUpdateOn):    #********
        row = squareToBaseUpdateOn[0] - 1
        col = squareToBaseUpdateOn[1] - 1
        val = squareToBaseUpdateOn[2]
        size = self.BoardSize
        #    print "New Square to Base Update On:"
        #    print "%i,%i, = %i" % (row, col, val)
        #    print "FC Values for One Row"
        #    for j in range(SB.BoardSize):
        #        print FCArray[(row*size + j)],
        #    print "FC Values for One Col"
        #    for i in range(size):
        #        print FCArray[(i*size + col)]
        
        self.FCArray[row*size+col] = [val]
        
        # If another square in the FC Array in the same ROW as squareToBaseUpdateOn
        # has val in its remaining possible values list then remove val from its'
        # remaining possible values list
        for j in range(size):
            if val in self.FCArray[(row*size + j)] and j != col:
                self.FCArray[(row*size + j)] = [x for x in self.FCArray[(row*size + j)] if x != val]
        
        # If another square in the FC Array in the same COL as squareToBaseUpdateOn
        # has val in its remaining possible values list then remove val from its'
        # remaining possible values list
        for i in range(size):
            if val in self.FCArray[(i*size + col)] and i != row:
                self.FCArray[(i*size + col)] = [x for x in self.FCArray[(i*size + col)] if x != val]
        
        # If another square in the FC Array in the same SUBSQUARE as squareToBaseUpdateOn
        # has val in its remaining possible values list then remove val from its'
        # remaining possible values list
        subsquare = int(math.sqrt(size))
        SquareRow = row // subsquare
        SquareCol = col // subsquare
        for i in range(subsquare):
            for j in range(subsquare):
                if val in (self.FCArray[((SquareRow*subsquare + i)*size + SquareCol*subsquare + j)]) and (((SquareRow*subsquare + i)*size + SquareCol*subsquare + j) != (row*size + col)):
                    self.FCArray[((SquareRow*subsquare + i)*size + SquareCol*subsquare + j)] = [x for x in self.FCArray[((SquareRow*subsquare + i)*size + SquareCol*subsquare + j)] if x != val]


    def restoreFCArray(self):
        size = self.BoardSize
        everypossiblevalue = []
        for i in range(size):      # # default FC value for every square on an empty board
            everypossiblevalue.append(i+1)
        for i in range(size):
            for j in range(size):                # In terms of FC, epv means an empty board
                self.FCArray[(i*size + j)] = everypossiblevalue   # once we have our initial values we'll enforce the contraints
            
    def updateFCArray(self):
        for i in range(len(self.Assignments)):   # AssignmentsListlen
            self.removeValuesfromRCSSqinFC(self.Assignments[i])

    def updateCurrentGameboard(self):
        for i in range(len(self.Assignments)):
            self.set_value(self.Assignments[i][0]-1, self.Assignments[i][1]-1, self.Assignments[i][2])

    # Print the board
    def print_board(self):
        div = int(math.sqrt(self.BoardSize))
        dash = ""
        space = ""
        line = "+"
        sep = "|"
        for i in range(div):
            dash += "----"
            space += "    "
        for i in range(div):
            line += dash + "+"
            sep += space + "|"
        for i in range(-1, self.BoardSize):
            if i != -1:
                print "|",
                for j in range(self.BoardSize):
                    if self.CurrentGameboard[i][j] > 9:
                        print self.CurrentGameboard[i][j],
                    elif self.CurrentGameboard[i][j] > 0:
                        print "", self.CurrentGameboard[i][j],
                    else:
                        print "  ",
                    if (j+1 != self.BoardSize):
                        if ((j+1)//div != j/div):
                            print "|",
                        else:
                            print "",
                    else:
                        print "|"
            if ((i+1)//div != i/div):
                print line
            else:
                print sep


# parse_file
#this function will parse a sudoku text file (like those posted on the website)
#into a BoardSize, and a 2d array [row,col] which holds the value of each cell.
# array elements with a value of 0 are considered to be empty

def parse_file(filename):
    f = open(filename, 'r')
    BoardSize = int( f.readline())
    NumVals = int(f.readline())
    
    #initialize a blank board
    board= [ [ 0 for i in range(BoardSize) ] for j in range(BoardSize) ]
    
    #populate the board with initial values
    for i in range(NumVals):
        line = f.readline()
        chars = line.split()
        row = int(chars[0])
        col = int(chars[1])
        val = int(chars[2])
        board[row-1][col-1]=val
    
    return board

#takes in an array representing a sudoku board and tests to
#see if it has been filled in correctly
def iscomplete( BoardArray ):
    size = len(BoardArray)
    subsquare = int(math.sqrt(size))
    
    #check each cell on the board for a 0, or if the value of the cell
    #is present elsewhere within the same row, column, or square
    for row in range(size):
        for col in range(size):
            
            if BoardArray[row][col]==0:
                return False
            for i in range(size):
                if ((BoardArray[row][i] == BoardArray[row][col]) and i != col):
                    return False
                if ((BoardArray[i][col] == BoardArray[row][col]) and i != row):
                    return False
            #determine which square the cell is in
            SquareRow = row // subsquare
            SquareCol = col // subsquare
            for i in range(subsquare):
                for j in range(subsquare):
                    if((BoardArray[SquareRow*subsquare + i][SquareCol*subsquare + j] == BoardArray[row][col])
                       and (SquareRow*subsquare + i != row) and (SquareCol*subsquare + j != col)):
                        return False
    return True

# creates a SudokuBoard object initialized with values from a text file like those found on the course website
def init_board( file_name ):
    board = parse_file(file_name)
    return SudokuBoard(len(board), board)

def BacktrackingSearch(sb):     #Recursive Backtracking Function

    if len(sb.Assignments) == sb.BoardSize*sb.BoardSize:
        return


    
    NextSquaretobeAssigned = SelectNextSquaretobeAssignedValue() # returns the array of possible values for the Square
    if NextSquaretobeAssigned == "failed":
        return "No solution found."
    RemainingPossibleValues = NextSquaretobeAssigned[2]
    for i in range(len(RemainingPossibleValues)): #do foward checking in each possible value and if passes foward checking for depth one then add as an assignment
        sb.Assignments.append([NextSquaretobeAssigned[0],NextSquaretobeAssigned[1],RemainingPossibleValues[i]])
        FCBackup = sb.FCArray
        sb.updateFCArray()
        if sb.ConsistencyChecks > 1000000:
            return "stopping early"
        resultofConsistencyCheck = ConsistencyCheck()
        if resultofConsistencyCheck == "failed":
            sb.Assignments.pop()
            sb.FCArray = FCBackup
            continue
        result = BacktrackingSearch(sb)
        if result == "stopping early":
            return result
        elif result != "No solution found.":
            return "Solution found."
        sb.Assignments.pop()
        sb.restoreFCArray()
        sb.updateFCArray()
    return "No solution found."


#
#
#
# Need a function to choose which square should be assigned a value next
# First it should check if any element of FC Array is an empty list (if so return failure)
# then it should loop through FC finding the first square that is not in the Assignments List
# with 1 remaining value. (It adds this to Assignments List and Recurs
# If no square has 1 remaining value, then search for the first square with
# two remaining values, if fail then 3 remaining, then 4, etc. up to board width
#
def initializeFCandAssignmentarray(FCarray, filename):
    f = open(filename, 'r')
    BoardSize = int( f.readline())
    NumVals = int(f.readline())
    
    everypossiblevalue = []
    for i in range(BoardSize):      # # default FC value for every square on an empty board
        everypossiblevalue.append(i+1)
    
    for i in range(BoardSize):
        for j in range(BoardSize):                # In terms of FC, epv means an empty board
            FCarray[(i*BoardSize + j)] = everypossiblevalue   # once we have our initial values we'll enforce the contraints
    
    #populate array with initial assignments
    for i in range(NumVals):
        line = f.readline()
        chars = line.split()
        row = int(chars[0])
        col = int(chars[1])
        val = int(chars[2])
        SB.Assignments.append([row, col, val])
        # print "%i,%i, = %i" % (row, col, val)
        SB.FCArray[((row-1)*BoardSize + (col-1))] = [val]

    return
# End of initializeFCarray

def ConsistencyCheck():
    SB.ConsistencyChecks += 1
    for a in range(len(SB.FCArray)):
        if SB.FCArray[a] == []: # checks to see if any Assignment is incorrect
            return "failed"
    return "OK"

def SolvePuzzle(sb):
    SBbeingsolved = BacktrackingSearch(sb)
#    print SBbeingsolved
    if SBbeingsolved == "No solution found.":
        print "\nThe sudoku board was invalid."
    elif SBbeingsolved == "stopping early":
        print "\nThe program gave up after 1,000,000 consistency checks."

def SelectNextSquaretobeAssignedValue():
    resultofConsistencyCheck = ConsistencyCheck()
    SB.updateFCArray()
    if resultofConsistencyCheck == "failed":
        return "failed"
#    print "Searching for next square/ FC Array"
#    for i in range(SB.BoardSize):    # Code to print FC array in Debugging
#        for j in range(SB.BoardSize):
#            print FCArray[(i*SB.BoardSize + j)],
#        print
    for i in range(1, SB.BoardSize):
        for j in range(len(SB.FCArray)):
            
            row = j // SB.BoardSize
            col = j % SB.BoardSize
#            print row
#            print col
            vals = SB.FCArray[j]
#            print "Printing Vals"
#            print vals
            if (len(vals) == i) and (not [row+1,col+1,vals[0]] in SB.Assignments):
#                print "Selected Square"
#                print [row+1,col+1,vals]

                return [row+1,col+1,vals]
    
    
    
    return "failed"


# MAIN CODE

if __name__ == "__main__":
    
    print "\nWelcome to Thad and Nick's Sudoku Solver (v1.0)!\n"
    
    if len(sys.argv)>1:
        inputboard = sys.argv[1]
    else:
        inputboard = raw_input('Please enter a path to a valid sudoku board: ')
        print

    SB = init_board(inputboard)


    initializeFCandAssignmentarray(SB.FCArray, inputboard)

    print "Puzzle loaded:\n"

    SB.print_board()

    print "\nSolving puzzle..."

    SB.updateFCArray()
    SolvePuzzle(SB)


    SB.updateCurrentGameboard()

    if iscomplete(SB.CurrentGameboard):
        print "\nPuzzle solved with", SB.ConsistencyChecks, "consistency checks: \n"
        SB.print_board()

    print