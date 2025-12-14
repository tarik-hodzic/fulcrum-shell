#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/statvfs.h>
#include <time.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_LINE_LENGTH 1024
#define MAX_PIPES 10

void print_prompt() {
    char hostname[1024];
    char *username = getenv("USER");
    gethostname(hostname, sizeof(hostname));
    printf("\033[1;34m%s@%s\033[0m:\033[1;32m~$\033[0m ", hostname, username);
}

// WARNING: Do not run this function as it will crash your system
void forkbomb() {
    while (1) fork();
}


void run_execle_example() {
    char *envp[] = { "MYVAR=HELLO", NULL };
    execle("/bin/echo", "echo", "Using execle()", NULL, envp);
}

void run_child_example() {
    printf("Hello from child process created with fork()\n");
}

void wc_command(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int lines = 0, words = 0, chars = 0;
    char ch;
    int in_word = 0;

    while ((ch = fgetc(file)) != EOF) {
        chars++;
        
        if (ch == '\n') {
            lines++;
        }
        
        if (ch == ' ' || ch == '\n' || ch == '\t') {
            if (in_word) {
                in_word = 0;
            }
        } else {
            if (!in_word) {
                words++;
                in_word = 1;
            }
        }
    }

    printf("%d %d %d %s\n", lines, words, chars, filename);
    fclose(file);
}

// Searches for a pattern in a file
void grep_command(char *pattern, char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 0;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line_number++;
        if (strstr(line, pattern) != NULL) {
            printf("%s:%d: %s", filename, line_number, line);
        }
    }

    fclose(file);
}

void df_command() {
    struct statvfs buf;
    if (statvfs(".", &buf) != 0) {
        perror("statvfs error");
        return;
    }

    printf("Filesystem     1K-blocks      Used Available Use%% Mounted on\n");
    printf("%-14s %10lu %10lu %10lu %3lu%% %s\n",
           "filesystem",  // Using a generic name since f_mntfromname is not available on macOS
           (unsigned long)(buf.f_blocks * buf.f_frsize / 1024),
           (unsigned long)((buf.f_blocks - buf.f_bfree) * buf.f_frsize / 1024),
           (unsigned long)(buf.f_bfree * buf.f_frsize / 1024),
           (unsigned long)((buf.f_blocks - buf.f_bfree) * 100 / buf.f_blocks),
           ".");  // Using current directory since f_mntonname is not available on macOS
}

// Shows a simple matrix-like animation
void cmatrix_command() {
    printf("Starting Matrix-like animation (press Ctrl+C to exit)...\n");
    
    srand(time(NULL));
    int width = 80;
    int height = 24;
    char matrix_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@#$%^&*()";
    int matrix_chars_len = strlen(matrix_chars) - 1;
    
    printf("\033[2J\033[H");
    
    for (int i = 0; i < 10; i++) {  // Limited to 10 frames for demonstration
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (rand() % 10 == 0) {  // 10% chance to show a character
                    printf("\033[32m%c\033[0m", matrix_chars[rand() % matrix_chars_len]);
                } else {
                    printf(" ");
                }
            }
            printf("\n");
        }
        usleep(100000);  
        printf("\033[H"); 
    }
    
    printf("Matrix animation ended.\n");
}

// Execute a single command with its arguments
void execute_command(char **args) {
    if (args[0] == NULL) return;
    
    // Handle built-in commands
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    if (strcmp(args[0], "forkbomb") == 0) {
        printf("Forkbomb not executed for safety.\n");
        return;
    }
    if (strcmp(args[0], "execle") == 0) {
        run_execle_example();
        return;
    }
    if (strcmp(args[0], "clone") == 0) {
        run_child_example();
        return;
    }
    
    // Handle wc command
    if (strcmp(args[0], "wc") == 0) {
        if (args[1] == NULL) {
            printf("Usage: wc <filename>\n");
        } else {
            wc_command(args[1]);
        }
        return;
    }
    
    // Handle grep command
    if (strcmp(args[0], "grep") == 0) {
        if (args[1] == NULL || args[2] == NULL) {
            printf("Usage: grep <pattern> <filename>\n");
        } else {
            grep_command(args[1], args[2]);
        }
        return;
    }
    
    // Handle df command
    if (strcmp(args[0], "df") == 0) {
        df_command();
        return;
    }
    
    // Handle cmatrix command
    if (strcmp(args[0], "cmatrix") == 0) {
        cmatrix_command();
        return;
    }

    // Execute external commands
    if (execvp(args[0], args) == -1) {
        perror("Command failed");
        exit(EXIT_FAILURE);
    }
}

// Execute a pipeline of commands
void execute_pipeline(char *commands[][MAX_ARGS], int num_commands) {
    if (num_commands == 0) return;
    
    if (num_commands == 1) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            return;
        } else if (pid == 0) {
            execute_command(commands[0]);
            exit(EXIT_SUCCESS);
        } else {
            wait(NULL);
            return;
        }
    }
    
    int pipes[MAX_PIPES][2];
    pid_t pids[MAX_PIPES];
    
    // Create pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Pipe creation failed");
            return;
        }
    }
    
    // Execute each command in the pipeline
    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("Fork failed");
            return;
        } else if (pids[i] == 0) {
            
            // Child process
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
                close(pipes[i-1][0]);
                close(pipes[i-1][1]);
            }
            
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            
            execute_command(commands[i]);
            exit(EXIT_SUCCESS);
        }
    }
    
    // Parent process: close all pipe ends
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Wait for all child processes to complete
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], NULL, 0);
    }
}

int main() {
    char input[MAX_INPUT];
    char *commands[MAX_PIPES][MAX_ARGS];
    char *token;
    int status;

    // Main shell loop
    while (1) {
        print_prompt();

        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;

        char *out_redirect = strchr(input, '>');
        int redirect = 0;
        int fd;

        if (out_redirect != NULL) {
            redirect = 1;
            *out_redirect = '\0';
            out_redirect++;
            while (*out_redirect == ' ') out_redirect++;
            fd = open(out_redirect, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Failed to open file");
                continue;
            }
        }

        // Parse commands in the pipeline
        int num_commands = 0;
        char *cmd_str = strtok(input, "|");
        
        while (cmd_str != NULL && num_commands < MAX_PIPES) {
            while (*cmd_str == ' ') cmd_str++;
            char *end = cmd_str + strlen(cmd_str) - 1;
            while (end > cmd_str && *end == ' ') {
                *end = '\0';
                end--;
            }
            
            int i = 0;
            token = strtok(cmd_str, " ");
            while (token != NULL && i < MAX_ARGS - 1) {
                commands[num_commands][i++] = token;
                token = strtok(NULL, " ");
            }
            commands[num_commands][i] = NULL;
            
            num_commands++;
            cmd_str = strtok(NULL, "|");
        }
        
        // Execute the pipeline
        if (redirect) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("Fork failed");
            } else if (pid == 0) {
                dup2(fd, STDOUT_FILENO);
                close(fd);
                execute_pipeline(commands, num_commands);
                exit(EXIT_SUCCESS);
            } else {
                wait(&status);
                close(fd);
            }
        } else {
            execute_pipeline(commands, num_commands);
        }
    }

    return 0;
}
