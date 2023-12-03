# Job Scheduler

This is my final project for my systems programming course where I was tasked of creating a job scheduler. This scheduler runs on a queue with X many jobs running at a time at the beginning of the queue where the other tasked jobs in the queue are set as waiting. The scheduler takes in a configurable amount of max running jobs which is supposed to represent the number of cores on a machine. The jobs accepted are binary executables with an absolute path, or a system command. The output of each job is written in a `.out` and `.err` file with the corresponding jobID number.

# Compiling

```
>$ make && make run ARGS="#"
```

# Code

I implemented a lot of functions in this project with each doing a specific task.

## Outer variables and structs

I made a max job size of 100 so at any time there would not be more than 100 jobs running/waiting in the queue. Next I made a struct `Job` with the necessary data that a job would need.

- ID: the id for the job
- Status: used to help keep track of waiting/running/done
- pid: the process ID
- command: the command/exec that the job is tasked to execute

Next the variables:

- jobs: the big array of all the jobs
- numJobs: number of current jobs
- runningJobs: number of running jobs
- maxRunningJobs: # of cores
- jobQueue: the queue

## executeJob()

This function is tasked with executing the job that it is given. This function takes in a job and then makes a sets the status to running and then creates a fork.

In the child process I redirect stdout and stderr into their own files. I couldn't find a way to create one file based on the exit status of execlp so at the moment for each job it will have a `.out` and `.err` file.

At the end I set the pid and increment running jobs.

## updateJobStatus()

The main goal of this function is to update the job status. It checks each job and its status if it is running. It checks the status of the child process and if it terminated then its status is set to done and removes it from the queue and grabs the next job and executes it.

## submitJob

This one submits a job into the queue and is similar to the updateJob function just because of the way I set up the main function.

It makes a new job and creates a new id, command, and status and adds it to the big job array.

I next check for insert error and then if there is an availiable slot for running then delete and get the ID of the next waiting job. If there was a waiting job then execute it.

## showJobs

This one is easy, all it does is loop through each job and get is status and display it if it is running or waiting.

## showHistory

The same thing as showjobs but shows every job that has been scheduled including errors

## main

check for argument first, then initialize max running jobs as the argument and initialize the queue.

Next is the main loop where I set aside 256 bytes for command length and then get the input and store it into `command`. Next I take out the newline and compare first 6 chars to see if it was `submit` and if it was then I pass the command part of the string into `submitJob`.

If the string was exactly `showjobs`, then I would first update the status of the jobs and then display them. Similarly if it was `history` it would hsow the job history and then same for `exit`.

At the end I destroyed the queue.

# Examples

Here is an example of addding a couple of executables to the queue and the job history:

[adding1](/images/adding.png)

[history1](/images/history1.png)

Here is an example of adding some more commands to the queue with one being an error:

[adding2](/images/adding2.png)

[history2](/images/history2.png)

[showjobs](/images/showjobs.png)