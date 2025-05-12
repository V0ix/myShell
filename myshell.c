#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 64

// Function to split input by pipe
int parse_pipe(char *input, char **commands) {
    int count = 0;
    commands[count] = strtok(input, "|");
    while (commands[count] && count < MAX_ARGS - 1) {
        commands[++count] = strtok(NULL, "|");
    }
    return count;
}

// Function to split command into arguments
void parse_command(char *cmd, char **args) {
    int i = 0;
    args[i] = strtok(cmd, " \t");
    while (args[i] && i < MAX_ARGS - 1) {
        args[++i] = strtok(NULL, " \t");
    }
}

// Execute a command, handle redirection
void execute_command(char *cmd) {
    char *args[MAX_ARGS];
    int in_fd = -1, out_fd = -1;
    char *in_file = NULL, *out_file = NULL;
    
    // Handle I/O redirection
    char *in_ptr = strchr(cmd, '<');
    char *out_ptr = strchr(cmd, '>');

    if (in_ptr) {
        *in_ptr = '\0';
        in_file = strtok(in_ptr + 1, " \t");
    }

    if (out_ptr) {
        *out_ptr = '\0';
        out_file = strtok(out_ptr + 1, " \t");
    }

    parse_command(cmd, args);
    if (!args[0]) return;

    // Handle built-in commands
    if (strcmp(args[0], "exit") == 0){
        printf("bye buddy...\n");
        exit(0);
    }
    if (strcmp(args[0], "cd") == 0) {
        if (args[1]) chdir(args[1]);
        else fprintf(stderr, "mysh: expected path\n");
        return;
    }
    if (strcmp(args[0], "help") == 0) {
        printf("mysh: custom shell\nBuilt-ins: cd, exit, help\nSupports: I/O redirection, piping, standard commands\n");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process: handle redirection
        if (in_file) {
            in_fd = open(in_file, O_RDONLY);
            if (in_fd < 0) { perror("Input redirection"); exit(1); }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (out_file) {
            out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) { perror("Output redirection"); exit(1); }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        execvp(args[0], args);
        perror("mysh");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
}

// Function to execute piped commands
void execute_pipeline(char *input) {
    char *commands[MAX_ARGS];
    int num_cmds = parse_pipe(input, commands);

    int i, fd[2], in_fd = 0;

    for (i = 0; i <= num_cmds; i++) {
        pipe(fd);
        pid_t pid = fork();

        if (pid == 0) {
            dup2(in_fd, 0);  // Read from previous
            if (i != num_cmds) dup2(fd[1], 1);  // Write to next
            close(fd[0]);

            execute_command(commands[i]);
            exit(0);
        } else {
            wait(NULL);
            close(fd[1]);
            in_fd = fd[0]; // Next command reads from here
        }
    }
}

// Get current directory prompt
void print_prompt() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\033[1;31m🐧%s\033[0m$ ", cwd);
    fflush(stdout);
}

// Main shell loop
int main(int argc, char *argv[]) {
    char input[MAX_CMD_LEN];

    while (1) {
        print_prompt();
        if (!fgets(input, sizeof(input), stdin)) break;

        input[strcspn(input, "\n")] = 0;
        if (strchr(input, '|'))
            execute_pipeline(input);
        else
            execute_command(input);
    }

    return 0;
}
