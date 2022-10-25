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

void CreateLog ();
void WriteLog (char *);

int main(int argc, char *argv[]) {
    // Initialize log
    CreateLog();

    // Get server number from arguments
    int numServers = 0;
    if (argc > 1) {
        numServers = atoi(argv[1]);
    }

    // Socket variables
	int sockfd, ret;
    struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Error in connection
    if (sockfd < 0) {
        exit(1);
    }

    // Initialize socket
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Give socket local address
    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    // Error in connection
    if (ret < 0) {
        WriteLog("Failed to connect to port 4444.\n");
        exit(1);
    }

    // Prepare to accept connection to client
	listen(sockfd, 10);

    // Counting variables
    int numConnectionsEnded = 0;
    int totalLikes = 0;

    while (numConnectionsEnded < numServers) {
        // Wait for connection to client
        newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);

        // Client failed to connect
        if (newSocket < 0) {
            exit(1);
        }

        // Empty buffer
        bzero(buffer, sizeof(buffer));

        // Recieve message from client in buffer
        recv(newSocket, buffer, 1024, 0);

        // Exit responce from server
        if (strcmp(buffer, ":exit") == 0) {
            numConnectionsEnded++;
        }
        // Non exit message
        else {
            // Generate responce that the message was recieved
            char response[1034];
            sprintf(response, "recieved %s", buffer);
            
            // Add likes to total
            totalLikes += atoi(buffer + 8);
            char log[50];
            sprintf(log, "Total %d\n", totalLikes);

            // Write message and total likes to log
            WriteLog(buffer);
            WriteLog(log);
            
            // Send success message to client
            send(newSocket, response, sizeof(response), 0);
        }

        // Disconnect from client
        close(newSocket);
    }

    return 0;
}

void CreateLog() {
    // Create or clear log file
    FILE *fp;
    fp = fopen("./tmp/PrimaryLikes.txt","w");
    fclose(fp);
}

void WriteLog(char *myString) {
    // Open log file and write string
    FILE *fp;
    fp = fopen("./tmp/PrimaryLikes.txt","a");
    fprintf(fp, "%s", myString);
    fclose(fp);
}