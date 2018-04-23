/*
PRIYAM BANERJEE
1001529497

Earthquake Data Set : Sorting by latitude

Either: sort by hand, or write a two loop sort program to sort the data
"Instrument" your program (time it)
Create a program that will, in turn, run multiple processes "concurrently" using "fork()" and "exec()".
Do the sort again, in parallel for 2 concurrent processes, then 4, and 10 processes.
Instrument these sorts.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
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
    node_t *temp = head;

    for(i = 0; i < index; i++)
    {
        temp = temp->next;
    }

    return temp;
}


void sort(int start_index, int end_index)
{
    //fork();

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
        //printf("%f\n", next->lat);

        //prevtemp = local_head;
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

            //prevtemp = temp;
            temp = temp->next;

        }


        if(next != NULL)
        {
            //prev = next;
            next = next->next;
        }
    }
    /*next = local_head;
    while(next != NULL && next != end)
    {
        printf("%f\n", next->lat);
        next = next->next;
    }*/
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
    time_t start,end;
    double prog_time;
    start = clock();

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

    head = mmap(NULL, sizeof(struct LineNode), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);
    strcpy(head->line, buffer);
    head->lat = atof(token);
    head->next = NULL;

    next = head;

    while ((fgets(buffer, 500, fp)))
    {
        count++;
        token = strtok(buffer, ",");
        token = strtok (NULL, ",");
        // Create new node
        temp = mmap(NULL, sizeof(struct LineNode), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);//malloc(sizeof(struct LineNode));
        temp->lat = atof(token);
        temp->next = NULL;
        strcpy(temp->line, buffer);

        next->next = temp;
        next = temp;

    }

    next->next = NULL;

    printf("How many processes?\n");
    scanf("%s", num);
    printf("\n");
    int procCount = atoi(num);
    int size = count / procCount;
    /*
    double seconds;
    time_t currtime1, currtime2;
    time(&currtime1);
    */
    int id;

    // Sorting
    for(i = 0; i < procCount; i++)
    {
        id = fork();
        if(id == 0)
        {
            if(i == (procCount - 1))
            {
                sort(i*size, count-1);
            }
            else
            {
                sort(i*size, (i+1) * size - 1);
            }
            exit(0);
        }
    }

   // wait();

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
    prog_time = (double)(end-start)/CLOCKS_PER_SEC;
    printf("\nTotal Time : %f \n", prog_time);

    return 0;

}
