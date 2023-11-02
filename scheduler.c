#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <time.h>


// TODO: Add more fields to this struct
struct job {
    int id;
    int arrival;
    int length;
    int tickets;
    float remTime;
    struct job *next;
};

struct timings{
  int time;
  int jobId;
  int arrivedAt;
  int timeWorkedOn;
  struct timings *next;
};

struct qTs{
  int responseTime;
  int turnAroundTime;
  int flag;
  int timeArrived;
  int work;
};
/*** Globals ***/ 
int seed = 100;

//This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;
struct timings* headT = NULL;
int numOfJobs =0;

/*** Globals End ***/

/*Function to append a new job to the list*/
void append(int id, int arrival, int length, int tickets){
  struct job *tmp = (struct job*) malloc(sizeof(struct job));
  if (tmp == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  numOfJobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;
  tmp->tickets = tickets;
  tmp -> remTime = length;
  tmp->next = NULL;
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
  int tickets = 0;
  struct job **head_ptr = malloc(sizeof(struct job*));
  if( (fp = fopen(filename, "r")) == NULL)
    exit(EXIT_FAILURE);
  while ((read = getline(&line, &len, fp)) > 1) {
    arrival = strtok(line, ",\n");
    length = strtok(NULL, ",\n");
    tickets += 100;
    assert(arrival != NULL && length != NULL);    
    append(id++, atoi(arrival), atoi(length), tickets);
  }
  fclose(fp);
  assert(id > 0);
  return;
}

void printExecutionTrace(struct timings* timingsHead) {
    struct timings* temp = timingsHead;
    while (temp != NULL) {
        printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n", temp->time, temp->jobId, temp->arrivedAt, temp->timeWorkedOn);
        temp = temp->next;
    }
}

int checkToWork (struct job *head){
  struct job* curr = head;
  while(curr!= NULL){
    if(curr->remTime >0){
      return 1;
    }
    curr = curr -> next;
  }
  return 0;
}
void policy_STCF(struct job *head, int slice) {
  struct job *curr = head;
  struct timings* timingsCurr = NULL;
  // 1 Tick = 1s
  int ticks =0;
  int timeWorked =slice;
  while(checkToWork(head))
  {
    
    struct job *temp = head;
      int currRemTime = 10000;
    if(ticks%slice == 0){

      while(temp!=NULL){
          if(ticks >= temp->arrival && temp->remTime >0){
          if(temp-> remTime < currRemTime){
            curr = temp;
            currRemTime = temp-> remTime;
          }
        }
        temp = temp->next;
      }

      if (headT == NULL) {
                headT = (struct timings*)malloc(sizeof(struct timings));
                timingsCurr = headT;
            } else if(headT!=NULL) {
                timingsCurr->next = (struct timings*)malloc(sizeof(struct timings));
                timingsCurr = timingsCurr->next;
            }
            timingsCurr->time = ticks;
            timingsCurr->jobId = curr->id;
            timingsCurr->arrivedAt = curr->arrival;
            if(curr->remTime > slice)
            timingsCurr->timeWorkedOn = slice;
            else
            timingsCurr->timeWorkedOn = curr->remTime;
            // printf("%d  %d\n",timingsCurr->jobId,timingsCurr->timeWorkedOn);
            timingsCurr->next = NULL;
      // reinitialize timeWorked
      // timeWorked = 0;
    }
    if(curr->remTime>0){
    curr->remTime -= 1;
    // timeWorked++;
    }
    ticks++;
  }

  printExecutionTrace(headT);
  return;
}

void policy_RR(struct job *head, int slice) {
  struct job *curr = head;
  struct timings* timingsCurr = NULL;
  int ticks =0;
  int timeWorked =slice;
  while(checkToWork(head))
  {
    if(curr->remTime >= slice){
      timeWorked =slice;
      curr->remTime-= slice;
    }
    else{
      timeWorked = curr-> remTime;
      curr->remTime -=  timeWorked;
    }  
      if (headT == NULL) {
                headT = (struct timings*)malloc(sizeof(struct timings));
                timingsCurr = headT;
            } else if(headT!=NULL) {
                timingsCurr->next = (struct timings*)malloc(sizeof(struct timings));
                timingsCurr = timingsCurr->next;
            }
            timingsCurr->time = ticks;
            timingsCurr->jobId = curr->id;
            timingsCurr->arrivedAt = curr->arrival;
            timingsCurr->timeWorkedOn = timeWorked;
            timingsCurr->next = NULL;
    
    // logic to move the node
    if(curr -> next != NULL){
      while(curr->next!=NULL)
      if(curr-> next -> remTime >0){
        curr = curr->next;
        break;
      }
      else{
        curr=curr-> next;
      }
    }
    else{
      curr = head;
      while(curr!= NULL){
        if(curr->remTime>0){
          break;
        }
        else{
          if(curr->next!=NULL)
          curr=curr->next;
          else{
            break;
          }
        }
      }
    }
    ticks+= timeWorked;
  }

  printExecutionTrace(headT);
  return;
}

void policy_LT(struct job* head,int slice) {
    int ticks =0;
    struct job *curr = head;
    struct timings* timingsCurr = NULL;
    int timeWorked =slice;
    int totalTickets =0;
    while(curr != NULL) {
        totalTickets = curr->tickets;
        curr = curr->next;
        }
      srandom(time(NULL));
    while(checkToWork(head)){
      curr = head;
      
      int randomNum = rand()% totalTickets;      
      while(curr!=NULL){
       if (randomNum >= (curr->tickets - 100) && randomNum < curr->tickets && curr->remTime > 0){
          // do the job
          if (headT == NULL) {
                headT = (struct timings*)malloc(sizeof(struct timings));
                timingsCurr = headT;
            } else if(headT!=NULL) {
                timingsCurr->next = (struct timings*)malloc(sizeof(struct timings));
                timingsCurr = timingsCurr->next;
            }
            timingsCurr->time = ticks;
            timingsCurr->jobId = curr->id;
            timingsCurr->arrivedAt = curr->arrival;
            // timingsCurr->timeWorkedOn = timeWorked;
            if(curr-> remTime > slice){
              timingsCurr->timeWorkedOn = timeWorked;
              curr->remTime -= slice;
              ticks += slice;
            }else{
              timingsCurr->timeWorkedOn = curr->remTime;
              curr->remTime -= timingsCurr->timeWorkedOn ;
              ticks += timingsCurr->timeWorkedOn ;

            }
            timingsCurr->next = NULL;
            break;
        }
        else{
          curr = curr-> next;
        }
        
      }
    }
    printExecutionTrace(headT);
    
}
void freeJobList() {
    struct job* current = head;
    while (current != NULL) {
        struct job* temp = current;
        current = current->next;
        free(temp); // Free the current job node
    }
}





void freeTimingsList() {
    struct timings* current = headT;
    while (current != NULL) {
        struct timings* temp = current;
        current = current->next;
        free(temp); // Free the current timings node
    }
}



void analyze(int slice, char* what ) {
  float averageResponse=0;
  float averageTurnAround=0;
  float averageWait=0;
  struct qTs qT[numOfJobs];
  for(int i=0; i<numOfJobs; i++){
    qT[i].flag=0;
    qT[i].work =0;
  }

  struct timings* curr = headT;
  while(curr!=NULL){
    if(qT[curr->jobId].flag==0){
      qT[curr->jobId].responseTime = curr->time - curr->arrivedAt;
      qT[curr->jobId].flag=1;
    }
    qT[curr->jobId].turnAroundTime = curr->time - curr->arrivedAt + curr->timeWorkedOn;

    qT[curr->jobId].work += curr->timeWorkedOn;
    curr=curr->next;
  }
  printf("Begin analyzing %s\n", what);
  for(int i =0; i<numOfJobs; i++){
    printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", i, qT[i].responseTime, qT[i].turnAroundTime, qT[i].turnAroundTime-qT[i].work);
    averageResponse+= (float)qT[i].responseTime/numOfJobs;
    averageTurnAround+=(float)qT[i].turnAroundTime/numOfJobs;
    averageWait+=((float)qT[i].turnAroundTime-(float)qT[i].work)/numOfJobs;
  }
  printf("Average -- Response: %.2f  Turnaround %.2f  Wait %.2f\n", averageResponse, averageTurnAround, averageWait);
  printf("End analyzing %s.\n", what);
  return;
}

int main(int argc, char **argv) {

 if (argc < 5) {
    fprintf(stderr, "missing variables\n");
    fprintf(stderr, "usage: %s analysis-flag policy workload-file slice-length\n", argv[0]);
		exit(EXIT_FAILURE);
  }
  int analysis = atoi(argv[1]);
  char *policy = argv[2],
       *workload = argv[3];
  int slice = atoi(argv[4]);
  // Note: we use a global variable to point to 
  // the start of a linked-list of jobs, i.e., the job list 
  read_workload_file(workload);
  if (strcmp(policy, "STCF") == 0 ) {
    policy_STCF(head, slice);
    if (analysis) {
      analyze(slice, policy);
    }
  }
  else if(strcmp(policy, "RR") == 0){
    policy_RR(head, slice);
    if(analysis){
      analyze(slice, policy);
    }
  }
  else if(strcmp(policy, "LT") == 0){
    policy_LT(head,slice);
    if(analysis){
      analyze(slice,policy);
    }
  }

  freeJobList();
  freeTimingsList();

	exit(EXIT_SUCCESS);
}
