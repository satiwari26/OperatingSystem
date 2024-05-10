import argparse
import copy

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


def SJN_scheduler():
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


def SRTN_scheduler():
    global jobs

    # sort the job based on the arrival time and if they have same arrival time sort it based on their run time
    jobs.sort(key=lambda x: (x.arrival_time, x.run_time))

    for job in jobs:
        print("job number: ", job.job_number, "job burst time: ", job.run_time, "Job arrival time", job.arrival_time)

    print("\n\n\n")

    # temp copy of jobs
    temp_jobs = copy.deepcopy(jobs)

    timer = jobs[0].arrival_time
    current_job = jobs[0]

    # to calculate turnaround and wait time:
    turn_around_time = 0
    wait_time = 0
    total_turn_around_time = 0
    total_wait_time = 0
    
    while(len(jobs) > 0): # if there are jobs in the list

        # first check if there is a job whose arrival time = timer && run-time < curr_job.run_time
        for jb in jobs:
            if(jb.arrival_time <= timer and current_job.run_time > jb.run_time):
                current_job = jb
        
        # now we will check if curr_job run time is greater than 0
        if(current_job.run_time > 0):
            timer += 1  #increament timer
            current_job.run_time -= 1 #decrement the run_time of current job

        current_job_index = 0

        # get the current index of the job on list
        for jb in jobs:
            if(current_job.job_number == jb.job_number):
                current_job_index = jobs.index(jb)
                break
        
        # update the job in the list based on the current job
        if(current_job.run_time == 0):
            turn_around_time = timer - current_job.arrival_time
            total_turn_around_time += turn_around_time

            for jb in temp_jobs:
                if(current_job.job_number == jb.job_number):
                    wait_time = turn_around_time - jb.run_time
                    total_wait_time += wait_time
                    break
            
            print("Job %3d -- Turnaround %3.2f  Wait %3.2f" % (current_job.job_number, turn_around_time, wait_time))

            jobs.pop(current_job_index)
            if(len(jobs) > 0):
                current_job = jobs[0] # update the current job
        else:
            jobs[current_job_index] = current_job
    
    print("Average -- Turnaround %.2f  Wait %.2f" % (total_turn_around_time/len(temp_jobs), total_wait_time/len(temp_jobs)))





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
    elif(algorithm == "SRTN"): # preamptive version
        SRTN_scheduler()
    elif(algorithm == "SJN"):  # non-preamptive version
        SJN_scheduler()





if __name__ == "__main__":
    main()
