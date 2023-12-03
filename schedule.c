#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "queue.h"

#define MAX_JOBS 100

/*      Variables       */

struct Job 
{
    int jobid;
    int status; // 0 waiting 1 running 2 done -1 error
    pid_t pid;
    char command[1024];
};

struct Job jobs[MAX_JOBS];
int numJobs = 0;
int runningJobs = 0;
int maxRunningJobs;

queue *jobQueue;

/*      Code starts      */

void executeJob(struct Job *job) 
{
    job->status = 1;
    pid_t pid = fork();

    if (pid == -1) 
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) // child
    {
        char outFileName[20], errFileName[20];
        sprintf(outFileName, "%d.out", job->jobid);
        sprintf(errFileName, "%d.err", job->jobid);

        freopen(outFileName, "w", stdout);
        freopen(errFileName, "w", stderr);

        execlp(job->command, job->command, (char *)NULL);

        // execlp fail
        perror("Execution failed");
        job->status = -1;
        exit(EXIT_FAILURE);
    } 
    else 
    {
        job->pid = pid;
        runningJobs++;
    }
}


void updateJobStatus() 
{ 
    // check for completed jobs
    for (int i = 0; i < numJobs; ++i) 
    {
        if (jobs[i].status == 1) 
        {
            int status;
            pid_t result = waitpid(jobs[i].pid, &status, 0);
            if (result == -1) 
            {
                perror("Waitpid");
                exit(EXIT_FAILURE);
            } 
            else if (result > 0) 
            {
                if (WIFEXITED(status))
                {
                    int exitStatus = WEXITSTATUS(status);
                    if (exitStatus == 0)
                        jobs[i].status = 2;
                    else
                        jobs[i].status = -1;
                }
                else
                    jobs[i].status = -1;
                runningJobs -= 1;

                // update queue with next waiting status job
                int jobid = queue_delete(jobQueue);
                if (jobid != -1)
                    executeJob(&jobs[jobid - 1]);
            }
        }
    }
}


void submitJob(char *command) 
{
    if (numJobs < MAX_JOBS) 
    {
        struct Job job;
        job.jobid = numJobs + 1;
        strcpy(job.command, command);
        job.status = 0;
        jobs[numJobs++] = job;

        printf("Job %d added to the queue\n", job.jobid);

        // insert into queue
        if (queue_insert(jobQueue, job.jobid) == -1) 
        {
            printf("Error: Failed to insert job into the queue\n");
            exit(EXIT_FAILURE);
        }

        // execute if slot availiable
        if (runningJobs < maxRunningJobs) 
        {
            int jobid = queue_delete(jobQueue);
            if (jobid != -1)
                executeJob(&jobs[jobid - 1]);
        }
    } 
    else
        printf("Max number of jobs reached\n");
}


void showJobs() 
{
    // loop through each active job
    printf("jobid \t\tcommand \t\tstatus\n");
    for (int i = 0; i < numJobs; ++i) 
    {
        if (jobs[i].status <= 1) 
        {
            printf("%d \t%s \t", jobs[i].jobid, jobs[i].command);
            switch(jobs[i].status)
            {
                case(0):
                {
                    printf("Waiting\n");
                    break;
                }
                case(1):
                {
                    printf("Running\n");
                    break;
                }
                case(-1):
                {
                    printf("Error\n");
                    break;
                }
                default:
                    break;
            }
        }
    }
}

void showHistory()
{
    printf("*** Job History: ***\n");
    printf("jobid \t\tcommand \t\tstatus\n");
    for(int i = 0; i< numJobs; ++i)
    {
        printf("%d \t%s\t", jobs[i].jobid, jobs[i].command);
        switch(jobs[i].status)
        {
            case(0):
            {
                printf(" Waiting\n");
                break;
            }
            case(1):
            {
                printf(" Running\n");
                break;
            }
            case(2):
            {
                printf(" Completed\n");
                break;
            }
            case(-1):
            {
                printf(" Error\n");
                break;
            }
            default:
                break;
        }
    }
}

int main(int argc, char **argv) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <number_of_cores>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if(atoi(argv[1]) > 8 || atoi(argv[1]) < 1)
    {
        fprintf(stderr, "Incorrect amount of cores\n");
        exit(EXIT_FAILURE);
    }

    maxRunningJobs = atoi(argv[1]);  // num cores = max running jobs
    jobQueue = queue_init(MAX_JOBS); // init queue

    while (1) // main loop
    {
        char command[256];
        printf("Enter command> ");
        fgets(command, sizeof(command), stdin);

        // remove \n
        int i = 0;
        while (command[i] != '\0')
        {
            if (command[i] == '\n') 
            {
                command[i] = '\0';
                break;
            }
            i++;
        }

        if (strncmp(command, "submit", 6) == 0) 
            submitJob(command + 7);
        else if (strcmp(command, "showjobs") == 0) 
        {
            updateJobStatus();
            showJobs();
        }
        else if (strcmp(command, "history") == 0)
        { 
            updateJobStatus();
            showHistory();
        }
        else if (strcmp(command, "exit") == 0)
        {
            printf("*** Exiting ***\n");
            exit(EXIT_SUCCESS);
        }
        else 
            printf("Unknown command\n");
    }
    queue_destroy(jobQueue);
    return 0;
}
