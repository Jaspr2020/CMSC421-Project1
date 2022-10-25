#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define NUMSERVERS 10

void CreateLog ();
void WriteLog (char *);
int getChildNumber (int, int[]);

int main(int argc, char *argv[]) {
    // Initialize log
    CreateLog();

    // Start PrimaryLikesServer
    if (fork() == 0) {
        char numServers[3];
        sprintf(numServers, "%d", NUMSERVERS);
        char *args[] = {"./PrimaryLikesServer", numServers, NULL};
        execvp(args[0], args);
    }

    // Array of pid values for LikeServers
    int pidList[NUMSERVERS] = {};

    for (int ii = 0; ii < NUMSERVERS; ii++) {
        // Create child process
        pidList[ii] = fork();
        
        // Child process
        if (pidList[ii] == 0) {
            // Change to like server, server number passed as second argument
            char buffer[5];
            sprintf(buffer, "%d", ii);
            char *args[] = {"./LikeServer", buffer, NULL};
            execvp(args[0], args);
        } 
        // Parent process
        if (pidList[ii] > 0) {
            // Log that child was created sucessfully
            char buffer[50];
            sprintf(buffer, "Child %d created successfully!\n", ii);
            WriteLog(buffer);
        }
        // Parent process if error
        else {
            // Log error in creating child
            char buffer[50];
            sprintf(buffer, "Child %d creation failed!\n", ii);
            WriteLog(buffer);
        }
        
        // Wait 1 second
        sleep(1);
    }

    // For all child processes
    int status;
    for (int ii = 0; ii < NUMSERVERS; ii++) {
        // Wait until child process ends
        int pid = wait(&status);
        int exitStatus = WEXITSTATUS(status);
        int childNumber = getChildNumber(pid, pidList);

        // If child was a LikeServer
        if (childNumber != -1) {
            // Log the ending and status of the child process
            char buffer[40];
            sprintf(buffer, "Child %d ended with exit code %d\n", childNumber, exitStatus);
            WriteLog(buffer);
        }
        // If child was a PrimaryLikeServer
        else {
            ii--;
        }
    
    }

    printf("All LikeServers ended\n");
    return 0;
}

void CreateLog() {
    // Create tmp folder if it doesn't exist
    mkdir("./tmp", 0777);

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

int getChildNumber (int pid, int pidList[]) {
    // Iterate through array of pids
    for (int ii = 0; ii < NUMSERVERS; ii++) {
        // If pid matched one in list, return the index
        if (pid == pidList[ii]) {
            return ii;
        }
    }
    // Return -1 if no index matched
    return -1;
}