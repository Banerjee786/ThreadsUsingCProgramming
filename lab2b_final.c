/*
Priyam Banerjee
1001529497

Earthquake Dataset: sort by latitude

"Instrument" your program (time it)
Create a program that will, in turn, run multiple threads "concurrently" using a kernel level threading system.
Do the sort again, in parallel for 2 concurrent threads, then 4, and 10 threads.
Instrument these sorts.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#define BUFFER_SIZE 1024*1024


struct LineNode
{
    float lat;
    char line[300];
    struct LineNode *next, *prev;
};
typedef struct LineNode node_t;
node_t *head = NULL;
node_t *glob_head = NULL;


struct procNode
{
    node_t *list;
    struct procNode *next;
};
typedef struct procNode node_p;


node_t *getNode(int index)
{
    int i;
    node_t *temp = head, *prev_temp;

    for(i = 0; i < index; i++)
    {
        if(temp == NULL)
        {
            break;
        }
        prev_temp = temp;
        temp = temp->next;
    }
    if(temp != NULL)
    {
        return temp;
    }
    else
    {
        return prev_temp;
    }
}


void sort(void *arg)
{
    int start_index = ((int*)arg)[0];
    int end_index = ((int*)arg)[1];

    int i = start_index, j;
    float lat;
    char line[300];
    node_t *prev, *temp, *prevtemp, *next, *local_head, *end;

    end = getNode(end_index);
    prev = getNode(start_index);
    local_head = prev;
    next = prev->next;

    while(next != NULL && next != end)
    {
        temp = local_head;

        while(temp != next && temp != NULL)
        {

            if(temp->lat >= next->lat)
            {
                strcpy(line, next->line);
                lat = next->lat;
                prevtemp = next;
                while(prevtemp != temp && prevtemp != NULL)
                {
                    strcpy(prevtemp->line, prevtemp->prev->line);
                    prevtemp->lat = prevtemp->prev->lat;
                    prevtemp = prevtemp->prev;
                }
                strcpy(temp->line, line);
                temp->lat = lat;
                break;
            }

            temp = temp->next;

        }


        if(next != NULL)
        {
            next = next->next;
        }
    }

}

void merge(int start_index, int end_index)
{
    int i = start_index, j;
    float lat;
    char line[300];
    node_t *prev, *temp, *prevtemp, *next, *local_head, *end, *new_node;

    end = getNode(end_index);
    next = getNode(start_index);

    while(next != NULL && next != end->next)
    {
        if(glob_head == NULL)
        {
            glob_head = malloc(sizeof(struct LineNode));
            strcpy(glob_head->line, next->line);
            glob_head->lat = next->lat;
            glob_head->next = NULL;

        }
        else if(glob_head->lat >= next->lat)
        {
            new_node = malloc(sizeof(struct LineNode));
            strcpy(new_node->line, next->line);
            new_node->lat = next->lat;
            new_node->next = glob_head;
            glob_head = new_node;
        }
        else
        {
            prevtemp = glob_head;
            temp = glob_head->next;

            if(temp == NULL)
            {
                new_node = malloc(sizeof(struct LineNode));
                strcpy(new_node->line, next->line);
                new_node->lat = next->lat;
                new_node->next = NULL;
                glob_head->next = new_node;
            }
            else
            {
                while(temp != NULL)
                {
                    if(temp->next == NULL)
                    {
                        if(temp->lat >= next->lat)
                        {
                            new_node = malloc(sizeof(struct LineNode));
                            strcpy(new_node->line, next->line);
                            new_node->lat = next->lat;
                            new_node->next = temp;
                            prevtemp->next = new_node;
                        }
                        else
                        {
                            new_node = malloc(sizeof(struct LineNode));
                            strcpy(new_node->line, next->line);
                            new_node->lat = next->lat;
                            new_node->next = NULL;
                            temp->next = new_node;
                        }

                        break;
                    }
                    else if(temp->lat >= next->lat)
                    {
                        new_node = malloc(sizeof(struct LineNode));
                        strcpy(new_node->line, next->line);
                        new_node->lat = next->lat;
                        new_node->next = temp;
                        prevtemp->next = new_node;

                        break;
                    }

                    prevtemp = temp;
                    temp = temp->next;

                }
            }
        }

        prev = next;
        next = next->next;

    }
}


int main(int argc, char *argv[])
{
    clock_t strt,end;
    double prog_time;
    strt = clock();
    node_t *next, *temp;
    char buffer[500], num[2];
    FILE *fp;
    char *token;
    int i, j, count = 1;

    int fd = open("/Documents/Programs", O_RDWR, 0);

    if ((fp=fopen("all_week.csv", "r"))==NULL)
    {
        printf ("file cannot be opened");
        return 1;
    }

    fgets(buffer, 500, fp);
    fgets(buffer, 500, fp);
    token = strtok(buffer, ",");
    token = strtok (NULL, ",");

    head = malloc(sizeof(struct LineNode));
    strcpy(head->line, buffer);
    head->lat = atof(token);
    head->next = NULL;
    head->prev = NULL;

    next = head;

    while ((fgets(buffer, 500, fp)))
    {
        count++;
        token = strtok(buffer, ",");
        token = strtok (NULL, ",");
        // Create new node
        temp = malloc(sizeof(struct LineNode));
        temp->lat = atof(token);
        temp->next = NULL;
        temp->prev = next;
        strcpy(temp->line, buffer);

        next->next = temp;
        next = temp;

    }

    next->next = NULL;

    printf("How many threads?\n");
    scanf("%s", num);
    printf("\n");
    int procCount = atoi(num);
    int size = count / procCount;

    double seconds;
    struct timeval stop, start;
    time_t currtime1, currtime2;
    gettimeofday(&start, NULL);
    time(&currtime1);

    int id, err;
    int arr[2];
    pthread_t tid[10];


    // Sorting
    for(i = 0; i < procCount; i++)
    {

        if(i == (procCount - 1))
        {
            //sort(i*size, count-1);
            arr[0] = i * size;
            arr[1] = count - 1;
            err = pthread_create(&(tid[i]), NULL, (void*)&sort, (void*)arr);

        }
        else
        {
            //sort(i*size, (i+1) * size - 1);
            arr[0] = i * size;
            //pthread_join(tid[i], NULL);
            arr[1] = (i+1) * size - 1;
            err = pthread_create(&(tid[i]), NULL, (void*)&sort, (void*)arr);
        }
    }


    //wait();
    for (i = 0; i < procCount; i++)
    {
        pthread_join(tid[i], NULL);
    }


    // Merging
    for(i = 0; i < procCount; i++)
    {
        if(i == (procCount - 1))
        {
            merge(i*size, count-1);
        }
        else
        {
            merge(i*size, (i+1) * size - 1);
        }

    }

    next = glob_head;
    while(next != NULL)
    {
        printf("%f\n", next->lat);
        next = next->next;
    }

    end = clock();
    prog_time = ((double)(end-strt))/CLOCKS_PER_SEC;
    printf("\nProgram Time : %f\n",prog_time);

    return 0;

}

