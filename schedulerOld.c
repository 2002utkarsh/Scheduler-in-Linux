#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>


//SOURCES USED:- CHATGPT, COURSE BOOK, STACK OVERFLOW
// TODO: Add more fields to this struct
struct job {
    int id;
    int arrival;
    int length;
    struct job *next;
};


/*** Globals ***/ 
int seed = 100;
int maxJobs = 0;


//This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*** Globals End ***/

/*Function to append a new job to the list*/
void append(int id, int arrival, int length){
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job*) malloc(sizeof(struct job));

  //tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;

  // the new job is the last job
  tmp->next = NULL;

  // Case: job is first to be added, linked list is empty 
  if (head == NULL){
    head = tmp;
    return;
  }

  struct job *prev = head;

  //Find end of list 
  while (prev->next != NULL){
    prev = prev->next;
  }

  //Add job to end of list 
  prev->next = tmp;
  return;
}


/*Function to read in the workload file and create job list*/
void read_workload_file(char* filename) {
  int id = 0;
  FILE *fp;
  size_t len = 0;
  ssize_t read;
  char *line = NULL,
       *arrival = NULL, 
       *length = NULL;

  struct job **head_ptr = malloc(sizeof(struct job*));

  if( (fp = fopen(filename, "r")) == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) > 1) {
    arrival = strtok(line, ",\n");
    length = strtok(NULL, ",\n");
       
    // Make sure neither arrival nor length are null. 
    assert(arrival != NULL && length != NULL);
        
    append(id++, atoi(arrival), atoi(length));
    maxJobs = maxJobs + 1;
  }

  fclose(fp);

  // Make sure we read in at least one job
  assert(id > 0);

  return;
}


void policy_FIFO(struct job* head) {
    struct job* current = head;
    int time = 0; // Initialize the time to 0.

    printf("Execution trace with FIFO:\n");

    while (current != NULL) {
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, current->id, current->arrival, current->length);

        // Update the current time and move to the next job.
        time += current->length;
        current = current->next;
    }

    printf("End of execution with FIFO.\n");
}

void analyze_FIFO(struct job* head) {
    int totalResponseTime = 0;
    int totalTurnaroundTime = 0;
    int totalWaitTime = 0;
    int jobCount = 0;
    int time = 0;

    for (struct job* current = head; current != NULL; current = current->next) {
        int responseTime = time - current->arrival;
        int turnaroundTime = responseTime + current->length;
        int waitTime = responseTime;

        totalResponseTime += responseTime;
        totalTurnaroundTime += turnaroundTime;
        totalWaitTime += waitTime;
        jobCount++;

        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
               current->id, responseTime, turnaroundTime, waitTime);

        time += current->length;
    }

    double avgResponseTime = (double)totalResponseTime / jobCount;
    double avgTurnaroundTime = (double)totalTurnaroundTime / jobCount;
    double avgWaitTime = (double)totalWaitTime / jobCount;

    printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n",
           avgResponseTime, avgTurnaroundTime, avgWaitTime);
}


void policy_SJF(struct job* head) {
    // Sort jobs by length (and by arrival time in case of ties)
    struct job* i;
    struct job* j;

    for (i = head; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (i->length > j->length || (i->length == j->length && i->arrival > j->arrival)) {
                // Swap jobs i and j
                int tmpId = i->id, tmpArrival = i->arrival, tmpLength = i->length;
                i->id = j->id, i->arrival = j->arrival, i->length = j->length;
                j->id = tmpId, j->arrival = tmpArrival, j->length = tmpLength;
            }
        }
    }

    int time = 0;
    struct job* current = head;

    int totalResponseTime = 0;
    int totalTurnaroundTime = 0;
    int totalWaitTime = 0;
    int jobCount = 0;

    printf("Execution trace with SJF:\n");

    while (current != NULL) {
        if (time < current->arrival) {
            time = current->arrival; // Move the time to the arrival of the next job if the CPU was idle.
        }

        int responseTime = time - current->arrival;
        int turnaroundTime = responseTime + current->length;
        int waitTime = responseTime;

        totalResponseTime += responseTime;
        totalTurnaroundTime += turnaroundTime;
        totalWaitTime += waitTime;
        jobCount++;

        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", time, current->id, current->arrival, current->length);

        time += current->length;
        current = current->next;
    }

    printf("End of execution with SJF.\n");

    double avgResponseTime = (double)totalResponseTime / jobCount;
    double avgTurnaroundTime = (double)totalTurnaroundTime / jobCount;
    double avgWaitTime = (double)totalWaitTime / jobCount;

    printf("Average metrics -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n",
           avgResponseTime, avgTurnaroundTime, avgWaitTime);
}



struct JobMetrics {
    int responseTime;
    int turnaroundTime;
    int waitTime;
};

struct JobMetrics *jobMetrics = NULL;

void analyze_SJF(struct job* head) {
    int totalResponseTime = 0;
    int totalTurnaroundTime = 0;
    int totalWaitTime = 0;
    int jobCount = 0;
    int time = 0;

    // Sort the jobs by length (and by arrival time in case of ties)
    struct job* current = head;
    struct job* i;
    struct job* j;

    for (i = head; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (i->length > j->length || (i->length == j->length && i->arrival > j->arrival)) {
                // Swap jobs i and j
                int tmpId = i->id;
                int tmpArrival = i->arrival;
                int tmpLength = i->length;

                i->id = j->id;
                i->arrival = j->arrival;
                i->length = j->length;

                j->id = tmpId;
                j->arrival = tmpArrival;
                j->length = tmpLength;
            }
        }
    }

    while (current != NULL) {
        if (time < current->arrival) {
            time = current->arrival; // Move the time to the arrival of the next job if the CPU was idle.
        }

        int responseTime = time - current->arrival;
        int turnaroundTime = responseTime + current->length;
        int waitTime = responseTime;

        totalResponseTime += responseTime;
        totalTurnaroundTime += turnaroundTime;
        totalWaitTime += waitTime;
        jobCount++;

        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
               current->id, responseTime, turnaroundTime, waitTime);

        time += current->length;
        current = current->next;
    }

    double avgResponseTime = (double)totalResponseTime / jobCount;
    double avgTurnaroundTime = (double)totalTurnaroundTime / jobCount;
    double avgWaitTime = (double)totalWaitTime / jobCount;

    printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n",
           avgResponseTime, avgTurnaroundTime, avgWaitTime);
}


int main(int argc, char **argv) {

 if (argc < 4) {
    fprintf(stderr, "missing variables\n");
    fprintf(stderr, "usage: %s analysis-flag policy workload-file\n", argv[0]);
		exit(EXIT_FAILURE);
  }

  int analysis = atoi(argv[1]);
  char *policy = argv[2],
       *workload = argv[3];

  read_workload_file(workload);
  jobMetrics = (struct JobMetrics *)malloc(sizeof(struct JobMetrics) * maxJobs);
  if (!jobMetrics) {
    fprintf(stderr, "Failed to allocate memory for jobMetrics.\n");
    exit(EXIT_FAILURE);
}


  if (strcmp(policy, "FIFO") == 0 ) {
    policy_FIFO(head);
    if (analysis) {
      printf("Begin analyzing FIFO:\n");
      analyze_FIFO(head);
      printf("End analyzing FIFO.\n");
    }

    exit(EXIT_SUCCESS);
  }
  else if (strcmp(policy, "SJF") == 0){
      policy_SJF(head);
      if (analysis) {
        printf("Begin analyzing SJF:\n");
        analyze_SJF(head);
        printf("End analyzing SJF.\n");

    }
      exit(EXIT_SUCCESS);
  }

  // TODO: Add other policies 
  free(jobMetrics);
	exit(EXIT_SUCCESS);
}
