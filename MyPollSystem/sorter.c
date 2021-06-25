#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "toolkit.h"

void coord(char *fin, int numOfWorkers, int ifRandom, int attributeNum, int order, char *fout, pid_t myhieid){
  //a mammoth function, containing coord node/sorter node/merger node; demarcation would be given below

    //================================THE COORD NODE==================================
    resi *data, *cur, *ans;
    data = (resi*)malloc(sizeof(resi));
    ans = (resi*)malloc(sizeof(resi));
    int arraySize = read_data(fin, data);
    if (data->id == 0) data = data->next; //the first line would be an empty input
    if (numOfWorkers == -1) numOfWorkers = 100; //if the numOfWorkers is not assigned, make one
    resi *mp[numOfWorkers];
    for(int i = 0; i < numOfWorkers; i++){ //make a table storing data from each sorter
    mp[i] = (resi*)malloc(sizeof(resi));
    }
    int workload[numOfWorkers], fd[numOfWorkers][2]; //the batch's capacity for each sorter, and their pipes (one pipe per sorter)
    int i,j;
    float sortTime;
    double t1, t2, t3, ticspersec; //to record the turnaround sorting time and merge sorting time
    struct tms tb1, tb2, tb3;

    //number of elements each process will handle
    for (i=0; i<numOfWorkers; i++){
      if (ifRandom) {workload[i]=1;} //each sorter should have at least one element to handle
      else {workload[i] = arraySize / numOfWorkers;}
    }
    workload[0] += arraySize % numOfWorkers;
    int m = arraySize - numOfWorkers - (arraySize % numOfWorkers); //m: the number of unallocated elements
    srand(time(NULL)); //init the seed
    for (i = 0; i< numOfWorkers; i++){
          if (ifRandom){
                        if (i == numOfWorkers - 1){
                          workload[i] += m; //if this is the last point, it must contain all of the rest elements
                        }
                        else{
                          workload[i] += rand() % (m / 8); //m/8 would be a appropriate upper bound to keep the batches distribution as even as possible
                        }
          }
          m-=(workload[i]-1);
      }

      for (i=0;i<numOfWorkers;i++){
        if (pipe(fd[i]) == -1) //creating pipes
        {
          fprintf(stderr, "failed to create a pipe\n");
          exit(EXIT_FAILURE);
        }

      }


    //================================THE SORTER NODE==================================

    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);

    for (int childP = 0; childP < numOfWorkers; childP++)
    {

      int child = fork(); //creating children

      if (child < 0)
      {
          fprintf(stderr, "failed to fork child %d\n", childP);
          exit(EXIT_FAILURE);
      }

      //in a child
      if (child == 0)
      {

          close(fd[childP][0]);

          cur = data;
          for (i=0; i<childP; i++)
          for (j=0; j<workload[i]; j++) cur = cur->next; //find the starting point

          if (childP % 2 == 0){ //even: bubble sort odd: select sort
            sortTime = bubble_sort(cur, workload[childP], order, attributeNum);
          }
          else{
            sortTime = select_sort(cur, workload[childP], order, attributeNum);
          }

          printf("Worker <%d> has done its batch of %d records in %f sec.\n", childP, workload[childP], sortTime); //each sorter's time cost
          write_data_pipe(fd[childP][1], cur, workload[childP]);
          kill(myhieid, SIGUSR1); //send a signal to the root node

          exit(0);
          //exit from the child because it is done with its job now
      }
    }

    //OUTSIDE children

    int status = 0;

    sleep(((arraySize/numOfWorkers)*(arraySize/numOfWorkers)/10000000)+numOfWorkers/100+1);
    //I select a estimated time value that ensures each sorter's tasks' been done, rather than using wait()
    // would talk about it more in the write up and maybe the interview
    int l = 0;



    //================================THE MERGER NODE==================================
    for (j = 0; j<numOfWorkers; j++){ // reading data from pipe[i], storing them in mp[i]
      close(fd[j][1]);
      read_data_pipe(fd[j][0], mp[j], workload[j]);
      close(fd[j][0]);
      mp[j] = mp[j]->next;
    }

    printf("All separate sortings are done now.\n");

    cur = ans;
    t2 = (double) times(&tb2);
    for (i=0; i<arraySize; i++){
      if (order > 0) m = 100000000; else m = 0; //m is for the max/min value
      l = -1;
      for (j=0; j<numOfWorkers; j++) //iterate through each sorter's data
      if (mp[j] != NULL)
      if ((m-get_feature(mp[j], attributeNum))*order > 0)
      {
        l = j;
        m = get_feature(mp[j], attributeNum);
      }
      if (l == -1) break;
      cur->next = mp[l]; //add the max/min value into the ans linked list
      cur = cur->next;
      mp[l] = mp[l]->next; //move forward
      cur->next = NULL;
    }
      t3 = (double) times(&tb3);

    printf("The whole merge process takes %f sec.\n", (t3 - t2) / ticspersec); //merging time
    printf("The turnaround time of sorting process is %f sec.\n", (t3 - t1) / ticspersec); //total time
    cur = ans->next;
    write_data(fout, cur, arraySize); //output to a file

    kill(myhieid, SIGUSR2); //send SIGUSR2
    //gracefully release memory
    destroy(data);
    destroy(ans);
}

/*int main(){
  coord("data/100000.csv", 100, 0, 4, -1, "ans.txt", getpid());
  return 0;
}*/
