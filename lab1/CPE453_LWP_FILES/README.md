## lab1 

# Name:
Saumitra Tiwari

# How is this LWP library different from a real thread management library? Name two ways in which this LWP library API or functionality could be improved.
My LWP library is utilizing only one of the CPU cores. The context switching allows me to pause one thread and start another one, whereas the real thread management library tries to maximize resources by allowing threads to run concurrently if multiple CPU cores are present.

There are two ways in which we can improve the functionality of this LWP library:

- Using a better version of a scheduling algorithm other than round-robin, or always 0, such that it can ensure that the overall process throughput remains high at any given time, and also ensuring that priority to different processes is given based on important metrics such as the time the process has been waiting or running, or tasks that are more important and should be given higher priority than other processes.

- Figuring out how to learn about system resources and manage them better so that processes can run concurrently and maximize resource utilization.