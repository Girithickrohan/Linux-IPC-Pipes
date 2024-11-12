// C Program that illustrate communication between two process using unnamed pipes
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <string.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

void server(int rfd, int wfd); 
void client(int wfd, int rfd); 

int main() { 
    int p1[2], p2[2], pid; 
    pipe(p1); 
    pipe(p2); 
    pid = fork(); 
    if (pid == 0) { 
        close(p1[1]); 
        close(p2[0]); 
        server(p1[0], p2[1]); 
        return 0;
    } 
    close(p1[0]); 
    close(p2[1]); 
    client(p1[1], p2[0]); 
    
    int status;
    wait(&status);  // Correctly wait for the child process to finish
    
    return 0; 
} 

void server(int rfd, int wfd) { 
    int n; 
    char fname[2000]; 
    char buff[2000];

    n = read(rfd, fname, sizeof(fname) - 1); // Read file name from pipe
    if (n < 0) {
        perror("Error reading file name");
        exit(1);
    }
    fname[n] = '\0'; // Null-terminate the file name

    int fd = open(fname, O_RDONLY);
    if (fd < 0) { 
        write(wfd, "can't open", strlen("can't open")); 
    } else { 
        n = read(fd, buff, sizeof(buff)); 
        if (n < 0) {
            perror("Error reading file");
            exit(1);
        }
        write(wfd, buff, n); 
        close(fd); // Close the file descriptor
    }
}

void client(int wfd, int rfd) {
    char fname[2000];
    char buff[2000];

    printf("ENTER THE FILE NAME: ");
    scanf("%s", fname);

    printf("CLIENT SENDING THE REQUEST .... PLEASE WAIT\n");
    sleep(1); // Reduced sleep for a more responsive client
    write(wfd, fname, strlen(fname) + 1); // Send file name to server

    int n = read(rfd, buff, sizeof(buff) - 1); // Read response from server
    if (n < 0) {
        perror("Error reading response from server");
        exit(1);
    }
    buff[n] = '\0'; // Null-terminate the buffer

    printf("THE RESULTS OF CLIENTS ARE ...... \n"); 
    write(1, buff, n); // Write the result to stdout
}
