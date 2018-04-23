
/*
Name : Priyam Banerjee
UTA ID : 1001529497
Course # CSE 3320
Lab : Operating System || Process Execution
References : Code by Professor Levine, StackOverflow.com, WikiCode, TutorialPoint, linux.die.net and
System Calls referred from kernel.org, man7.org; Book : Linux manual by Mike Saunders
*/

/* The following conditions are satisfied in the code :-
    1. Parameter handling
    2. Appropriate comments
    3. Error handling
    4. No use of fixed path
    5. Buffer overflow issues
    6. Security checks, especially for direct calls to Kernel routine
    7. Handling system calls
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#define name_max_size 1024  // To be used for malloc()
#define max_size 1024       // To be used for malloc()

#define MAX_SIZE 1024       // To be used for malloc()
#define FILE_SIZE 255       // To be used for malloc()

/* function initializations below */
int change_directory();
int edit_file();
int file_date_time_filesize();
int run_file();
int sort_files();
int sort_fileNfolder_by_date();
int sort_fileNfolder_by_size();

static int CompareStr(const void * a,const void * b){ /*Comparator function to compare two subsequent strings for qsort() */
    return strcmp(*(const char **)a,*(const char **)b);
}

int main(){

clock_t start_time,end_time;
double prog_time;
start_time = clock();

pid_t child;
DIR *d;
struct dirent *de;
int i,c,k,f,j;
char cmd[256];
char *working_dir=malloc(name_max_size);
const char* filename;
char date[36];
struct stat stbuf;
char spec_filename[255];
char char_inp;

char ch;
char chr;
time_t time_now;
time_t t;

    time_now = time(NULL);
    printf(" \nTime Now : %s \n",ctime(&time_now));

    getcwd(working_dir,1024);
    printf("Current Working Directory : %s \n \n",working_dir);

    c = 1;
    printf("------------------------------------\n");
    printf("Directories Listing \n");
    printf("------------------------------------\n \n");
    d = opendir(".");
    while((de=readdir(d))){
        if((de->d_type) & DT_DIR)
            printf("%d. %s \n",c++,de->d_name);
    }
    closedir(d);
    d = opendir(".");
    c=1;
    f=1;
    printf("\n------------------------------------\n");
    printf("Files Listing \n");
    printf("------------------------------------\n");
    printf("Press \'N\' or \'n\' to get the next set of files : \n");
    printf("------------------------------------\n \n");
    while((de=readdir(d))){

        if(f%10==0){
           if(((de->d_type) & DT_REG))
            scanf(" %c",&ch);
            if(ch=='N' || ch=='n'){

                if(((de->d_type) & DT_REG)){
                printf(" %d . %s \n",c++,de->d_name);
                f++;
                }
            }
        }
        else{
            if(((de->d_type) & DT_REG)){
            printf(" %d . %s \n",c++,de->d_name);
            f++;
            }
        }
    }
        file_date_time_filesize(); // Function call to access size, access time and modification time of a file

        printf("\n------------------------------------\n");
        printf("Press E to Edit a file \n");
        printf("Press Q to  Quit \n");
        printf("Press R to run an executable program \n");
        printf("Press C to change directory \n");
        printf("Press S to sort file and directory listing \n");
        printf("-------------------------------------\n");
        scanf(" %c",&chr);

        switch(tolower(chr)){ /* SWitch..case to access the commands */
        case 'q': exit(0);
        case 'e':
            edit_file();    // Function call to edit a file
            break;
        case 'r':
            run_file();     // Function call to run a file
            break;
        case 'c':
            change_directory(); /* Function to change directory. Contains system call chdir()  */
            break;
        case 's':
            printf("Enter \'s\' to sort by size or \'d\' to sort by date : ");
            scanf(" %c",&char_inp);
            if(char_inp=='s' || char_inp=='S') sort_fileNfolder_by_size(); // Function call to sort files and folders by size (System call involved)
            else
            if(char_inp=='d' || char_inp=='D') sort_fileNfolder_by_date(); // Function call to sort files and folders by date (System call involved)
            else
            printf("Wrong Input");
            break;
         default:
            printf("Wrong choice");
            break;
        }
end_time = clock();
prog_time = ((double)end_time-start_time)/CLOCKS_PER_SEC;
printf("CPU Execution Time of Program : %f \n",prog_time);
return 0;
}

int change_directory(){                 /* Function contains system call chdir() and getcwd() to change working directory  */
    char *path=malloc(max_size);
    char buf[max_size];
    printf("Enter a path : ");
    scanf(" %[^\n]s",path);

    if(chdir(path)==-1){
        fprintf(stderr,"Error : Could not change to directory : %s <<Recheck the directory path>> \n",path);
        return 1;
    }
    getcwd(buf,max_size);   // System call getcwd() to get the current working directory
    printf("Current Working Directory is : %s \n",getcwd(buf,max_size));
    return 0;
}

int edit_file(){
    char *file_name=malloc(FILE_SIZE);  //  Dynamic allocation of memory
    char *input_text=malloc(MAX_SIZE);  // Dynamic allocation of memory
    printf("Enter the file name to be edited : ");
    scanf(" %[^\n]s",file_name);
    int filedesc = open(file_name,O_WRONLY | O_APPEND); // Open file for REad/Write
    if(filedesc < 0) return -1;
    printf("\n Enter some data to put in \n");
    scanf(" %[^\n]s",input_text);

    if(write(filedesc,input_text,strlen(input_text))!=strlen(input_text)){ // Function write() attempts to write the content
        write(2,"There was an error\n",19); // Error message is write is unsuccessful
        return -1;
    }
}

char* formatdate(char* str,time_t val) {    /* Calling function from file_date_time_filesize() to reference local time */
    strftime(str,36,"%m.%d.%Y Time : %H.%M.%S",localtime(&val));
    return str;
}

int file_date_time_filesize(){

    char *filename = malloc(FILE_SIZE); // Dynamic allocation of the filename w.r.t the file size
    char i;
    FILE *fp;
    long int size=0;
    struct stat st;
    printf("Enter a filename to fetch it\'s details (Size of File and Dates):\n");
    scanf(" %[^\n]s",filename);
    struct stat *file_info = malloc(sizeof(struct stat));
    if(lstat(filename,file_info)!=0){
        perror("Error");
        printf("Terminating program\n");
        exit(1);
    }
    char date[36];
    printf("Access Date : %s \n",formatdate(date,file_info->st_atime)); // Printing the Access Date
    printf("Modified Date : %s \n",formatdate(date,file_info->st_mtime)); // Printing the Modified Date
    free(file_info);

    stat(filename, &st);
    size=st.st_size; // To access the size of the file

    if(size)
        printf("File size is : %ld \n",size);
    else
        printf("Error while retrieving the file. Hence size is unavailable");

    return 0;
}

int run_file(){     // Function to run an executable file. Error handled on non-execution
    int status;
    char *exec_file_name = malloc(FILE_SIZE) ; // Dynamic allocation of memory
    printf("Enter the executable file name (Prefix the file name with ./) \n");
    scanf(" %[^\n]s",exec_file_name);
    if(fork() == 0){
        status = system(exec_file_name);
        printf("Status Code returned from system call : %d \n",status);
        if(status == 6912){
            printf("File found and run successful\n");
        }
        else
        if(status == 32512){
            printf("File is not found or non-executable\n");
            exit(-1);
        }
        else
        if(status == 32256){
            printf("File is non-executable\n");
            exit(-1);
        }
        else{
            printf("Error handled : File cannot be executed");
            exit(-1);
        }

    }
}

int sort_files(){   /* Function to sort files or folders (whichever applicable). Array of pointers used  */
    char ch;
    char *FolderNames[1024];    // Max 1024
    char *FileNames[1024];      // Max 1024
    DIR *dir;
    int i=0,j=0,k;
    struct dirent *entry;
    dir = opendir(".");
    if(dir != NULL){
        while((entry = readdir(dir))!= NULL){
            if((entry->d_type) & DT_DIR){
            FolderNames[i]=entry->d_name; // Storing the folder names in an array of pointers
            i++;
            }
            if((entry->d_type) & DT_REG){
            FileNames[j]= entry->d_name; // Storing the file names in an array of pointers
            j++;
            }
        }
    closedir(dir);
    }
    else{
        perror("Could not open directory"); // perror for handling the open issue
    }

    qsort(FileNames,j,sizeof(const char *),CompareStr); // qsort() using the comparator function CompareStr
    qsort(FolderNames,i,sizeof(const char *),CompareStr); // qsort() using the comparator function CompareStr
    printf("Enter f to sort files and d to sort directories : ");
    scanf(" %c",&ch);
    if(ch=='F' || ch=='f'){
    printf("\n-------------------------------------------\n");
    printf("Sorted Files By Names:\n");
    printf("-------------------------------------------\n\n");
        for(k=0;((k<j) && k<1024);k++)
            {
                printf("%s \n",FileNames[k]);
            }
    }
    else if(ch=='D' || ch=='d'){
    printf("\n-------------------------------------------\n");
    printf("Sorted Folder by Names :\n");
    printf("-------------------------------------------\n\n");
        for(k=0;((k<i) && k<1024);k++)
            {
                printf("%s \n",FolderNames[k]);
            }
    }
    else {
            printf("Wrong Input\n");
            exit(0);
    }
    return 0;
}

int sort_fileNfolder_by_date(){

    int status_file,status_folder;
    status_file = system("ls -lt -r");
    if(status_file == 0){
        printf("\n---------------------------------------------------------------\n");
        printf("File Sorting by date is successful\n");
        printf("----------------------------------------------------------------\n");
    }
    else{
        printf("\n----------------------------------------------------------------\n");
        printf("File Sorting by date unsuccessful. Error/exception encountered\n");
        printf("----------------------------------------------------------------\n");
    }

    status_folder = system("ls -ltr | grep ^d");
    if(status_folder ==0){
        printf("\n----------------------------------------------------------------\n");
        printf("Directory Sorting by date is successful\n");
        printf("----------------------------------------------------------------\n");
    }
    else{
        printf("\n----------------------------------------------------------------\n");
        printf("Directory Sorting by date unsuccessful. Error/exception encountered\n");
        printf("----------------------------------------------------------------\n");
    }
return 0;
    }

int sort_fileNfolder_by_size(){

    int status_file,status_folder;
    status_file = system("ls -lS -r");
    if(status_file == 0){
        printf("\n----------------------------------------------------------------\n");
        printf("File Sorting by size is successful\n");
        printf("----------------------------------------------------------------\n");
    }
    else{
        printf("\n----------------------------------------------------------------\n");
        printf("File Sorting by size unsuccessful. Error/exception encountered\n");
        printf("----------------------------------------------------------------\n");
    }
    status_folder = system("du -h | sort -h");   //
    if(status_folder == 0){
        printf("\n----------------------------------------------------------------\n");
        printf("Folder Sorting by size is successful\n");
        printf("----------------------------------------------------------------\n");
    }
    else{
        printf("\n----------------------------------------------------------------\n");
        printf("Folder Sorting by size unsuccessful. Error/exception encountered\n");
        printf("----------------------------------------------------------------\n");
    }

return 0;
}
