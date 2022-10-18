#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

void CreateLog();
void WriteLog(char *);
void ParentProcess();
int LikeServer(int);

int main() {
    ParentProcess();
    return 0;
}

void ParentProcess() {
    CreateLog();

    for (int ii = 0; ii < 10; ii++) {
        LikeServer(ii);
        sleep(1);
    }

    // printf("Hello from Parent!\n");
}

void CreateLog() {
    // Create or clear log file
    FILE *fp;
    fp = fopen("./tmp/ParentProcessStatus.txt","w");
    fclose(fp);
}

void WriteLog(char *myString) {
    // Open log file and write string
    FILE *fp;
    fp = fopen("./tmp/ParentProcessStatus.txt","a");
    fprintf(fp, "%s", myString);
    fclose(fp);
}

int LikeServer(int serverNumber) {
    // Create child process
    int id = fork();
    
    // Child process
    if (id == 0) {
        // Seconds process lasts
        int lifetime = 5;
        clock_t start = clock();

        char buffer[30];
        sprintf(buffer, "Child %d started\n", serverNumber);
        WriteLog(buffer);

        while ((clock() - start) / CLOCKS_PER_SEC < lifetime) {

        }
        // sleep(5);

        sprintf(buffer, "Child %d ended\n", serverNumber);
        WriteLog(buffer);
        exit(0);
    } 
    return id;
}