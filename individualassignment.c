#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define QTY_INTERVAL 6
#define QTY_WAIT_INTERVAL 2

void getPIN(char pin[QTY_INTERVAL + 1]) {
	srand(getpid() + getppid());
	pin[0] = 49 + rand() % 7;
for(int i = 1; i < QTY_INTERVAL; i++) {
	pin[i] = 48 + rand() % 7;
}
	pin[QTY_INTERVAL] = '\0';
}

void sigint_handler(int sig) {
    printf("\nMessage Interrupted, Not This Time!!!\n");
    exit(sig);
}

int main() {
    int pipes[QTY_INTERVAL][2]; // array of pipes
    pid_t pid[QTY_INTERVAL]; // array of child process IDs
    int i;

    // register SIGINT handler
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    // create pipes
    for (i = 0; i < QTY_INTERVAL; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // get message from user
    char s[100];
    printf("Enter a message to send to child processes: ");
    fgets(s, sizeof(s), stdin);

    // fork child processes
    for (i = 0; i < QTY_INTERVAL; i++) {
        pid[i] = fork();

        if (pid[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid[i] == 0) { // child process
            close(pipes[i][1]); // close write end of pipe
            char msg[999];
            char buffer [QTY_INTERVAL + 1];
            read(pipes[i][0], msg, sizeof(msg)); // read message from parent
            printf("***Child process %d received the  message: %s\n", i+1, msg);
	    sleep(QTY_WAIT_INTERVAL);
            close(pipes[i][0]); // close read end of pipe
            exit(EXIT_SUCCESS);
        }
        else { // parent process
            close(pipes[i][0]); // close read end of pipe
        }
    }

    // send message to child processes
    for (i = 0; i < QTY_INTERVAL; i++) {
        write(pipes[i][1], s, sizeof(s)); // write message to pipe
        close(pipes[i][1]); // close write end of pipe
    }

    // wait for child processes to exit
    for (i = 0; i < QTY_INTERVAL; i++) {
        waitpid(pid[i], NULL, 0);
    }

    return 0;
}
