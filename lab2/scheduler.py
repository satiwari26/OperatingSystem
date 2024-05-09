import argparse

class Jobs:
    def __init__(self, run_time, arrival_time):
        # job number is initialize to 0 for the beginning jobs
        self.job_number = 0 
        self.run_time = run_time
        self.arrival_time = arrival_time

# global array to store the jobs information
jobs = []

def read_jobs(filename):
    global jobs
    # lambda function for call-back in sorting function
    sort_lambda = lambda x: x.arrival_time 

    with open(filename, "r") as file:
        for line in file:
            num1, num2 = map(int, line.split())
            # add the instance of the job information in the jobs array
            jobs.append(Jobs(num1, num2))
        
        # sort the array based on the arrival time:
        sorted_jobs = sorted(jobs, key=sort_lambda)
        jobs = sorted_jobs

        # assign job number to each jobs based on their index value
        for i in range(len(jobs)):
            jobs[i].job_number = i+1


# schedules and execute the jobs in FIFO order
def FIFO_scheduler():
    global jobs
    timer = jobs[0].arrival_time # timer to keep count of jobs process
    end_time = []
    prevBurstTime = jobs[0].arrival_time
    for i in range(len(jobs)):
        timer += jobs[i].run_time
        end_time.append(timer)

    totalWaitTime = 0
    totalTurnAroundTime = 0
    
    print("Job ID  wait Time  TurnAround")
    for i in range(len(jobs)):
        turnAroundTime = (end_time[i] - jobs[i].arrival_time)
        totalTurnAroundTime += turnAroundTime
        waitTime = prevBurstTime - jobs[i].arrival_time
        totalWaitTime += waitTime
        print(jobs[i].job_number, "       ", waitTime, "        ", turnAroundTime)
        prevBurstTime += jobs[i].run_time
    
    average_wait_time = totalWaitTime / len(jobs)
    average_turnaround_time = totalTurnAroundTime / len(jobs)

    print("Average wait time: ", average_wait_time)
    print("Average turnaround time: ", average_turnaround_time)


def SRTN_scheduler():
    global jobs

    for job in jobs:
        print("job number: ", job.job_number, "job burst time: ", job.run_time, "Job arrival time", job.arrival_time)
    
    print("\n\n\n")

    timer = jobs[0].arrival_time
    # check the jobs that arrive in the time frame of the timer and then schedule it based on the SJF
    for job in jobs[:]:
        timer += job.run_time
        # list the jobs that arrive during this time frame
        list_jobs = []
        index_to_remove = []
        for j in jobs:
            if(j.arrival_time <= timer and (j != job) and (jobs.index(j) > jobs.index(job))):
                list_jobs.append(j)
                index_to_remove.append(jobs.index(j)) # keeping track of the index that we have to remove 
        
        
        # also remove the prev jobs upto job
        for j in range(jobs.index(job) + 1):
            index_to_remove.append(j)

        # sort it in decending order to perform the deletion sucessfully
        index_to_remove.sort(reverse=True)

        # sort list of the jobs based on their burst time
        list_jobs.sort(key = lambda x: x.run_time)
        updated_jobs = []

        for j in range(jobs.index(job) + 1):
            updated_jobs.append(jobs[j]) # append the elements upto current job

        updated_jobs.extend(list_jobs) # append the elements of the list jobs to updated jobs

        # remove the updated elements from the current jobs list
        for i in index_to_remove:
            del jobs[i]
        
        # once deleted now we can append the remaining of the jobs in the updated list
        updated_jobs.extend(jobs)

        # set the jobs to updated jobs
        jobs = updated_jobs
    
    for job in jobs:
        print("job number: ", job.job_number, "job burst time: ", job.run_time, "Job arrival time", job.arrival_time)







def parseArguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("filename", help="Input file containing job information")
    parser.add_argument("-p", "--algorithm", type=str, help="Algorithm to use")
    parser.add_argument("-q", "--quantum", type=int, help="Quantum value")
    
    args = parser.parse_args()
    return (args.filename, args.algorithm, args.quantum)


def main():
    (filename, algorithm, quantum) = parseArguments()

    read_jobs(filename)

    # setting the default value for the algorithm and quantum
    if(algorithm is None or (algorithm != "SRTN" and algorithm != "FIFO" and algorithm != "RR")):
        algorithm = "FIFO"
    if(quantum is None):
        quantum = 1

    if(algorithm == "FIFO"):
        FIFO_scheduler()
    elif(algorithm == "SRTN"):
        SRTN_scheduler()





if __name__ == "__main__":
    main()
