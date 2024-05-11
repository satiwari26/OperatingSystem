#!/usr/bin/env python3

import sys
import math
from collections import deque
from enum import Enum

class ALGORITHM(Enum):
    FIFO = 1,
    RR = 2,
    SRTN = 3

class JOB:
    def __init__(self, run_t, arrival_t, pid):
        self.pid = pid  # The process ID
        self.run_t = run_t # Burst time
        self.arrival_t = arrival_t
        self.cur_t = run_t # Current run-time
        self.wait_t = 0.0  # Wait time
        self.turnaround_t = 0.0 # Turnaround time
        self.done = False

def printJobsbyArrival(algorithm):
    global jobArray

    sortedArray = sorted(jobArray, key= lambda job : job.arrival_t)

    return sortedArray

def incrementWaitTimes(sortedArray, currentPid, timeExpended):
    for index in range (len(sortedArray)):
        #Only update the wait times for processes that have arrived and are not running at this point in time
        if (sortedArray[index].pid != currentPid and sortedArray[index].arrival_t <= timeExpended and sortedArray[index].cur_t != 0):
            sortedArray[index].wait_t+=1

def printResults(sortedArray):
    avgWait = 0
    avgTurnaround = 0

    sortedArray.sort(key= lambda job : job.pid)
    
    for curLine in range (len(sortedArray)):
        print("Job %3d -- Turnaround %3.2f  Wait %3.2f" % (sortedArray[curLine].pid, sortedArray[curLine].turnaround_t, sortedArray[curLine].wait_t))
        avgWait += sortedArray[curLine].wait_t
        avgTurnaround += sortedArray[curLine].turnaround_t
    
    avgWait /= len(sortedArray)
    avgTurnaround /= len(sortedArray)
    print("Average -- Turnaround %.2f  Wait %.2f" % (avgTurnaround, avgWait))

'''
FIFO
'''
def scheduleFIFO():

    sortedArray = printJobsbyArrival(ALGORITHM.FIFO)

    ### ACTUAL FIFO ALGORITHM ###
    current = 0
    timeExpended = 0

    for current in range (len(sortedArray)):

        while (timeExpended < sortedArray[current].arrival_t):
            timeExpended+=1
        
        while (sortedArray[current].cur_t > 0):
            incrementWaitTimes(sortedArray, sortedArray[current].pid, timeExpended)

            sortedArray[current].cur_t-=1
            timeExpended+=1
        
        sortedArray[current].turnaround_t = timeExpended - sortedArray[current].arrival_t
    
    ### END OF FIFO ALGORITHM ###
    printResults(sortedArray)

'''
RR
'''
def scheduleRR():
    global quantum

    sortedByArrivalArray = printJobsbyArrival(ALGORITHM.RR)

    jobsToProcess = deque(sortedByArrivalArray) # Converting sorted arrival-times array to a queue, to be able to pop(move) elements from this list to the ready queue 
    readyQueue = deque()
    waitingQueue = deque()

    timeExpended = 0
    localQuantum = quantum

    # Keep processing until the ready and waiting queues are empty
    while (jobsToProcess or readyQueue or waitingQueue):
        
        localQuantum = quantum
        currentJob = None

        # Keep adding newly-arrived processes to the ready queue (while removing them from the list of processes that have yet to be scheduled)
        while (jobsToProcess and jobsToProcess[0].arrival_t <= timeExpended):
            readyQueue.append(jobsToProcess.popleft())

        # If the ready queue is empty, then we know that there are still jobs but they have not yet arrived, we must wait now
        if (len(readyQueue) == 0 and len(waitingQueue) == 0 and jobsToProcess):
            timeExpended+=1
        else:
            # Process the next job from the ready queue
            if (readyQueue):
                currentJob = readyQueue.popleft()

            # Move anything from the waiting queue to the end of the ready queue
            if (waitingQueue):
                readyQueue.extend(waitingQueue)
                waitingQueue.clear()

            if (currentJob == None and readyQueue):
                    currentJob = readyQueue.popleft()

            while (currentJob != None and currentJob.cur_t > 0):
                incrementWaitTimes(sortedByArrivalArray, currentJob.pid, timeExpended)

                currentJob.cur_t-=1
                localQuantum-=1
                timeExpended+=1

                if (currentJob.cur_t <= 0 or localQuantum <= 0):
                    break

            if (currentJob != None and currentJob.cur_t <= 0): # Job finished before quantum expired
                currentJob.turnaround_t = timeExpended - currentJob.arrival_t
                # Update the contents of the shortest job in the sorted arrival times list
                for index, job in enumerate(sortedByArrivalArray):
                    if job.pid == currentJob.pid:
                        sortedByArrivalArray[index] = currentJob
            elif (currentJob != None): # Job quantum expired
                # If a job is arriving at the same time as the current process' quantum expires, put the expired process in the ready queue first
                if (jobsToProcess and jobsToProcess[0].arrival_t == timeExpended):
                    readyQueue.append(currentJob)
                # Else, put the expired process into the waiting queue
                else:
                    waitingQueue.append(currentJob)
        
    printResults(sortedByArrivalArray)
            
'''
SRTN
'''
def scheduleSRTN():
    global jobArray

    sortedByArrivalArray = printJobsbyArrival(ALGORITHM.SRTN)

    ### ACTUAL SRTN ALGORITHM ###
    done = 0
    timeExpended = 0

    while (done < len(sortedByArrivalArray)):

        srtnArray = []

        for index in range(len(sortedByArrivalArray)):
            if (sortedByArrivalArray[index].cur_t > 0 and sortedByArrivalArray[index].arrival_t <= timeExpended):
                srtnArray.append(sortedByArrivalArray[index])
        
        if (len(srtnArray) == 0):
            timeExpended+=1
        else:
            srtnArray.sort(key = lambda job : job.cur_t)

            shortestJob = srtnArray[0]

            if (shortestJob.cur_t > 0):
                incrementWaitTimes(sortedByArrivalArray, shortestJob.pid, timeExpended)
                shortestJob.cur_t-=1
                timeExpended+=1
            
            if (shortestJob.cur_t <= 0):
                if (shortestJob.done == False):
                    shortestJob.turnaround_t = timeExpended - shortestJob.arrival_t
                    shortestJob.done = True
                    done+=1
            
            # Update the contents of the shortest job in the sorted arrival times list
            for index, job in enumerate(sortedByArrivalArray):
                if job.pid == shortestJob.pid:
                    sortedByArrivalArray[index] = shortestJob

    ### END OF SRTN ALGORITHM ###

    printResults(sortedByArrivalArray)

'''
Parse the command-line arguments.
'''
def parseArgs(argv):
    global algorithm, quantum, jobFileName, jobFile

    algorithm = ALGORITHM.FIFO
    jobFile = None

    for index in range(len(argv)):
        # Check for manual algorithm selection from the user
        if argv[index] == "-p" and index < (len(argv) - 1):
            if argv[index + 1] == "RR" or argv[index + 1] == "rr":
                algorithm = ALGORITHM.RR
            if argv[index + 1] == "SRTN" or argv[index + 1] == "srtn":
                algorithm = ALGORITHM.SRTN
        # Check for manaul quantum seleciton from the user
        elif argv[index] == "-q" and index < (len(argv) - 1):
            userQuantum = float(argv[index + 1])
            quantum = userQuantum if userQuantum > 0 else 1 # Convert the quantum to a float format, ensure that it is at least 1

        if (index == 1):
            jobFileName = argv[index]
            jobFile = open(argv[index], "r")

    return jobFile

def executeProgram():
    global jobArray, jobFile, algorithm

    jobArray = []
    fileLineNum = 0
    
    for currentLine in jobFile:
        # Parse the current line to get the run and arrival times, mapped to floats
        run_t, arrival_t = map(float, currentLine.split())
        jobArray.append(JOB(run_t, arrival_t, fileLineNum))
        fileLineNum+=1

    if(algorithm == ALGORITHM.RR):
        scheduleRR()
    elif(algorithm == ALGORITHM.SRTN):
        scheduleSRTN()
    else:
        scheduleFIFO()

'''
Main program, a scheduler
'''
def main(argv):
    global algorithm

    # Checks if the file exists and returns an eror if it does not exist
    if parseArgs(argv) is None:
        print("Error: file does not exist\n")
        return 1
    else:
        executeProgram()

if __name__ == "__main__":
    args = sys.argv
    sys.exit(main(args))