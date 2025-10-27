// CS4375 HW3 Extra Credit – Turnaround/Response time
// Joel Martínez Alvarado

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PROCESSES 5
#define WORK_ITERATIONS 10
#define WORK_AMOUNT 2000000

struct process_stats {
  int pid;
  int priority;
  int arrival_time;
  int first_run_time;
  int completion_time;
  int turnaround_time;
  int response_time;
};

// Do some CPU work
void
do_work(int amount)
{
  volatile int count = 0;
  for(int i = 0; i < amount; i++) {
    count++;
  }
}

// Child process that reports timing
void
child_process(int id, int priority, int pipe_fd)
{
  struct process_stats stats;
  
  stats.pid = getpid();
  stats.priority = priority;
  stats.arrival_time = uptime();
  
  // Set priority
  setpriority(priority);
  
  // Record first run time (immediately after setting priority)
  stats.first_run_time = uptime();
  
  // Do some work
  for(int i = 0; i < WORK_ITERATIONS; i++) {
    do_work(WORK_AMOUNT);
  }
  
  // Record completion time
  stats.completion_time = uptime();
  
  // Calculate metrics
  stats.turnaround_time = stats.completion_time - stats.arrival_time;
  stats.response_time = stats.first_run_time - stats.arrival_time;
  
  // Send stats back to parent
  write(pipe_fd, &stats, sizeof(stats));
  close(pipe_fd);
  
  exit(0);
}

int
main(int argc, char *argv[])
{
  int pipes[NUM_PROCESSES][2];
  struct process_stats stats[NUM_PROCESSES];
  int priorities[] = {0, 10, 20, 30, 40};  // Varying priorities
  int total_turnaround = 0;
  int total_response = 0;

  printf("\n=== Extra Credit: Turnaround/Response Time ===\n\n");
  printf("Testing %d processes with priorities 0, 10, 20, 30, 40\n\n", NUM_PROCESSES);

  // Create pipes for communication
  for(int i = 0; i < NUM_PROCESSES; i++) {
    if(pipe(pipes[i]) < 0) {
      printf("Error: pipe creation failed\n");
      exit(1);
    }
  }

  // Create child processes
  for(int i = 0; i < NUM_PROCESSES; i++) {
    int pid = fork();
    if(pid < 0) {
      printf("Error: fork failed\n");
      exit(1);
    }
    if(pid == 0) {
      // Child process
      close(pipes[i][0]);  // Close read end
      child_process(i, priorities[i], pipes[i][1]);
      exit(0);  // Should never reach here
    }
    close(pipes[i][1]);  // Parent closes write end
  }

  // Read statistics from all children
  for(int i = 0; i < NUM_PROCESSES; i++) {
    read(pipes[i][0], &stats[i], sizeof(struct process_stats));
    close(pipes[i][0]);
  }

  // Wait for all children
  for(int i = 0; i < NUM_PROCESSES; i++) {
    wait(0);
  }

  // Display individual statistics
  printf("Individual Process Statistics:\n");
  printf("%-4s %-8s %-8s %-10s %-14s %-12s %-12s\n",
         "ID", "PID", "Priority", "Arrival", "First Run", "Completion", "Turnaround");
  printf("--------------------------------------------------------------------------------\n");

  for(int i = 0; i < NUM_PROCESSES; i++) {
    printf("%-4d %-8d %-8d %-10d %-14d %-12d %-12d\n",
           i,
           stats[i].pid,
           stats[i].priority,
           stats[i].arrival_time,
           stats[i].first_run_time,
           stats[i].completion_time,
           stats[i].turnaround_time);
    
    total_turnaround += stats[i].turnaround_time;
    total_response += stats[i].response_time;
  }

  printf("\n");

  // Calculate and display averages
  int avg_turnaround = total_turnaround / NUM_PROCESSES;
  int avg_response = total_response / NUM_PROCESSES;

  printf("\nResults:\n");
  printf("  Avg Turnaround: %d ticks\n", avg_turnaround);
  printf("  Avg Response: %d ticks\n", avg_response);
  printf("\n");

  printf("Response by priority:\n");
  for(int i = 0; i < NUM_PROCESSES; i++) {
    printf("  Priority %2d: %d ticks\n",
           stats[i].priority, stats[i].response_time);
  }
  printf("\nExtra credit complete.\n\n");

  exit(0);
}

