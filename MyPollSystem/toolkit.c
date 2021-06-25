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
#define LENGTH 1000000

//==================================================================================================
//basically a box full of useful tools that we can call and utilize

typedef struct resi{
    int id, nod, zc;
    float ic;
    char fn[20], ln[20];
    struct resi *next;
}resi; //I use linked list to store data, in order to improve the merging sort's efficiency

int getSubstring(const char *pSrcString,const int index,char *pSubString,const int pSubStringSize){
//I directly copy this one from PROJ1, in order to make the reading process easier

    int space_cnt = 0;
    int i = 0;
    int j = 0;
    int find_index = 0;

    for(i = 0; pSrcString[i] != '\n';i++)
    {
        if(space_cnt == index)
        {
            break;
        }
        if(' ' == pSrcString[i] && ' ' != pSrcString[i+1])
        {
            space_cnt++;
        }
    }
    if('\n' == pSrcString[i])
    {
        return -1;
    }
    find_index = i;
    for(j = 0;('\n' != pSrcString[j + find_index]);j++)
    {
        if(j < pSubStringSize)
        {
            if(' ' == pSrcString[j + find_index])
            {
                break;
            }
            else
            {
                pSubString[j] = pSrcString[j + find_index];
            }
        }
        else
        {
            printf("Input size reaches the maximum!\n");
            break;
        }
    }
    pSubString[j] = '\0';
    return 0;
}

int read_data(char *fname, resi *data){//noticing this function is for filename input, not file descriptor input
  char buff[70], obj1[10], obj2[20], obj3[20], obj4[10], obj5[10], obj6[10];
  int n = 0, p, i, prid;
  FILE *fp;

  if ((fp = fopen(fname, "r")) == NULL)
  {printf("Input file doesn't exist!\n"); exit(0);}
  resi *newone, *cur;
  cur = data;
  while (cur->next != NULL){cur = cur->next;}

  while(1){
          if(feof(fp)) break;
          fgets(buff, 70, (FILE*)fp);
          if (getSubstring(buff,0,obj1,10) == -1 || getSubstring(buff,1,obj2,20) == -1 || getSubstring(buff,2,obj3,20) == -1 || getSubstring(buff,3,obj4,10) == -1 || getSubstring(buff,4,obj5,10) == -1 || getSubstring(buff,5,obj6,10) == -1)
          {
                printf("Invalid data entered!\n");
                break;
          }
          if (prid == atoi(obj1)) break;
          else prid = atoi(obj1);

          newone = (resi*)malloc(sizeof(resi));
          newone->id = atoi(obj1);
          p = strlen(obj2);
          for (i = 0; i < p; i++){
            newone->fn[i] = obj2[i];
          }
          p = strlen(obj3);
          for (i = 0; i < p; i++){
            newone->ln[i] = obj3[i];
          }
          newone->nod = atoi(obj4);
          newone->ic = atof(obj5);
          newone->zc = atoi(obj6);
          cur->next = newone;
          cur = newone;
          n++;

      }
      fclose(fp);

  return n; //this reading would return a size of the data
}

int read_data_pipe(int fd, resi *data, int m){ //using pipe to read data, most of it is similar to read_data
  char buff[70], obj1[10], obj2[20], obj3[20], obj4[10], obj5[10], obj6[10];
  int n = 0, p, i, prid,k;
  resi *newone, *cur;
  cur = data;
  while (cur->next != NULL){cur = cur->next;}


  for (k=0; k<m; k++)
  {
          read(fd, buff, 70);
          if (getSubstring(buff,0,obj1,10) == -1 || getSubstring(buff,1,obj2,20) == -1 || getSubstring(buff,2,obj3,20) == -1 || getSubstring(buff,3,obj4,10) == -1 || getSubstring(buff,4,obj5,10) == -1 || getSubstring(buff,5,obj6,10) == -1)
          {
                break;
          }
          if (prid == atoi(obj1)) break;
          else prid = atoi(obj1);

          newone = (resi*)malloc(sizeof(resi));
          newone->id = atoi(obj1);
          p = strlen(obj2);
          for (i = 0; i < p; i++){
            newone->fn[i] = obj2[i];
          }
          p = strlen(obj3);
          for (i = 0; i < p; i++){
            newone->ln[i] = obj3[i];
          }
          newone->nod = atoi(obj4);
          newone->ic = atof(obj5);
          newone->zc = atoi(obj6);
          cur->next = newone;
          cur = newone;
          n++;

      }
      close(fd);

  return n;
}

void write_data(char *fname, resi *data, int n){ //same as read_data, only for filename
  FILE *fp;
  resi *cur;

  fp = fopen(fname, "w");
  cur = data;
  for (int i=0; i<n; i++){
    fprintf(fp, "%d %s %s %d %.2f %d\n", cur->id, cur->fn, cur->ln, cur->nod, cur->ic ,cur->zc);
    cur = cur->next;
  }
  fclose(fp);
}

void write_data_pipe(int fd, resi *data, int n){ // writing in the pipe
  resi *cur;
  char op[70];
  cur = data;
  for (int i=0; i<n; i++){
    sprintf(op, "%d %s %s %d %.2f %d\n", cur->id, cur->fn, cur->ln, cur->nod, cur->ic ,cur->zc);
    write(fd, op, 70);
    cur = cur->next;
  }
  close(fd);
}

float get_feature(resi *data, int attr){ // for a given attribute number, get its value of corresponding attribute
  switch (attr){
    case 0:
      {return data->id;
      break;}
    case 3:
      {return data->nod;
      break;}
    case 4:
      {return data->ic;
      break;}
    case 5:
      {return data->zc;
      break;}
  }
  //notice that I wrote this part after writing two sorting programs, which could have greatly strengthen their concision; and that's why my sortings are
  //a little bit lengthy XD
}

//two sorting programs below: one is bubble_sort and one is select_sort, the most brutal ones
//using butches of switch/case... god I should figure the get_feature gunction out earlier
float bubble_sort(resi *data, int size, int order, int attri){
  resi temp, *di, *dj;
  int i, j;
  double t1, t2, ticspersec;
  struct tms tb1, tb2;

  ticspersec = (double) sysconf(_SC_CLK_TCK);
  t1 = (double) times(&tb1);

  if (data == NULL || data->next == NULL) return 0;
  di = data;
  switch (attri){
    case 0:{
      for (i = 0; i < size-1; i++){
        dj = di->next;
        for (j = i+1; j<size; j++){
          if ((di->id-dj->id)*order > 0){
            temp = *di;
            *di = *dj;
            *dj = temp;
            temp.next = di->next;
            di->next = dj->next;
            dj->next = temp.next;
          }
          dj = dj->next;
        }
        di = di->next;
      }
      break;
    }
    case 3:{
        for (i = 0; i < size-1; i++){
          dj = di->next;
          for (j = i+1; j<size; j++){
            if ((di->nod-dj->nod)*order > 0){
              temp = *di;
              *di = *dj;
              *dj = temp;
              temp.next = di->next;
              di->next = dj->next;
              dj->next = temp.next;
            }
            dj = dj->next;
          }
          di = di->next;
        }
        break;
      }
      case 4:{
          for (i = 0; i < size-1; i++){
            dj = di->next;
            for (j = i+1; j<size; j++){
              if ((di->ic-dj->ic)*order > 0){
                temp = *di;
                *di = *dj;
                *dj = temp;
                temp.next = di->next;
                di->next = dj->next;
                dj->next = temp.next;
              }
              dj = dj->next;
            }
            di = di->next;
          }
          break;
        }
          case 5:{
            for (i = 0; i < size-1; i++){
              dj = di->next;
              for (j = i+1; j<size; j++){
                if ((di->zc-dj->zc)*order > 0){
                  temp = *di;
                  *di = *dj;
                  *dj = temp;
                  temp.next = di->next;
                  di->next = dj->next;
                  dj->next = temp.next;
                }
                dj = dj->next;
              }
              di = di->next;
            }
            break;
          }
  }

  t2 = (double) times(&tb2);

  return (t2 - t1) / ticspersec; //return the sorting time
}

float select_sort(resi *data, int size, int order, int attri){
  resi temp, *di, *dj, *dm;
  int i, j;
  float m;
  double t1, t2, ticspersec;
  struct tms tb1, tb2;

  ticspersec = (double) sysconf(_SC_CLK_TCK);
  t1 = (double) times(&tb1);

  if (data == NULL || data->next == NULL) return 0;
  di = data;
  switch (attri){
    case 0:{
      for (i = 0; i < size-1; i++){
        dj = di;
        if (order > 0) m = 100000000; else m = 0;
        for (j = i; j<size; j++){
          if ((m-dj->id)*order > 0){
            dm = dj;
            m = dj->id;
          }
          dj = dj->next;
        }
          temp = *di;
          *di = *dm;
          *dm = temp;
          temp.next = di->next;
          di->next = dm->next;
          dm->next = temp.next;
        di = di->next;
      }
      break;
    }
    case 3:{
      for (i = 0; i < size-1; i++){
        dj = di;
        if (order > 0) m = 100000000; else m = 0;
        for (j = i; j<size; j++){
          if ((m-dj->nod)*order > 0){
            dm = dj;
            m = dj->nod;
          }
          dj = dj->next;
        }
          temp = *di;
          *di = *dm;
          *dm = temp;
          temp.next = di->next;
          di->next = dm->next;
          dm->next = temp.next;
        di = di->next;
      }
      break;
    }
    case 4:{
      for (i = 0; i < size-1; i++){
        dj = di;
        if (order > 0) m = 100000000; else m = 0;
        for (j = i; j<size; j++){
          if ((m-dj->ic)*order > 0){
            dm = dj;
            m = dj->ic;
          }
          dj = dj->next;
        }
          temp = *di;
          *di = *dm;
          *dm = temp;
          temp.next = di->next;
          di->next = dm->next;
          dm->next = temp.next;
        di = di->next;
      }
      break;
    }
    case 5:{
      for (i = 0; i < size-1; i++){
        dj = di;
        if (order > 0) m = 100000000; else m = 0;
        for (j = i; j<size; j++){
          if ((m-dj->zc)*order > 0){
            dm = dj;
            m = dj->zc;
          }
          dj = dj->next;
        }
          temp = *di;
          *di = *dm;
          *dm = temp;
          temp.next = di->next;
          di->next = dm->next;
          dm->next = temp.next;
        di = di->next;
      }
      break;
    }
  }

  t2 = (double) times(&tb2);

  return (t2 - t1) / ticspersec; //return the sorting time
}

//gracefully release all the memory allocated in a linked list
void destroy(resi *data){
  resi *cur, *prev;
  cur = data;
  while (cur != NULL){
    prev = cur;
    cur = cur->next;
    free(prev);
  }
  data = NULL;
}

/*int main(){

  int m = 0, id;
  resi *data, *cur;
  data = (resi*)malloc(sizeof(resi));


  m += read_data("data/1000.csv", data);
  if (data->id == 0) data = data->next;

  float sortTime = bubble_sort(data, m, 1, 4);

  write_data("ans.txt", data, m);

  destroy(data);
}*/
