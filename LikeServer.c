#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4444

void CreateLog (int);
void WriteLog (char *, int);

int main(int argc, char *argv[]) {
    // Length of process (in seconds)
    int lifetime = 5 * 60;
    // Start time of process
    clock_t start = clock();

    // Initialize random number generator
    time_t t;
    srand((unsigned) time(&t));

    // Get server number from second argument
    int serverNumber = 0;
    if (argc > 1) {
        serverNumber = atoi(argv[1]);
    }

    // Initialize log
    CreateLog(serverNumber);

    while (1) {
        // Socket variables
        int clientSocket, ret;
        struct sockaddr_in serverAddr;
        char buffer[1024];

        // Create socket
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        // Error in creation
        if(clientSocket < 0){
            exit(1);
        }

        // Initialize socket
        memset(&serverAddr, '\0', sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        // Connect to server
        ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        // Error in connection
        if (ret < 0) {
            exit(1);
        }

        // Random interval to wait
        int interval = (rand() % 5) + 1;
        clock_t intervalStart = clock();

        // Wait for interval
        while ((clock() - intervalStart) / CLOCKS_PER_SEC < interval) {
            // If the process has existed for more than it's intended lifetime
            if ((clock() - start) / CLOCKS_PER_SEC >= lifetime) {
                // Send exit message to server
                char b[1024];
                sprintf(b, "%s\n", ":exit");
                send(clientSocket, b, strlen(b), 0);
                // Close connection to server
                close(clientSocket);
                WriteLog("Done", serverNumber);
                exit(0);
            }
        }

        // Random view count
        int views = rand() % 43;
        
        // Generate and send message
        char b[32];
        sprintf(b, "Client%d %d\n", serverNumber, views);
        send(clientSocket, b, strlen(b), 0);

        // Clear buffer
        bzero(buffer, sizeof(buffer));

        // Read server response into buffer
        int rec = recv(clientSocket, buffer, 1024, 0);
        // Bad server response
        if (rec < 0) {
            exit(1);
        }
        // Write server response to log
        else {
            WriteLog(buffer, serverNumber);
            bzero(buffer, sizeof(buffer));
        }
        
        // Close socket connection
        close(clientSocket);
    }

    return 0;
}

void CreateLog(int childNumber) {
    // Create or clear log file
    FILE *fp;

    char buffer[40];
    sprintf(buffer, "./tmp/LikeServer%d.txt", childNumber);

    fp = fopen(buffer, "w");
    fclose(fp);
}

void WriteLog(char *myString, int childNumber) {
    // Open log file and write string
    FILE *fp;

    char buffer[40];
    sprintf(buffer, "./tmp/LikeServer%d.txt", childNumber);

    fp = fopen(buffer, "a");
    fprintf(fp, "%s", myString);
    fclose(fp);
}