//ex4
//author: Stav David
//id: 316232313

#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>

#include <string.h>

#include <pwd.h>

#include <limits.h>

#include <wait.h>

#include <signal.h>

#define MAX 512

pid_t son;
pid_t currSon;
pid_t tempSon;

char ** buildArray(char str[], int sum);
int wordSum(char str[]);
int foundCd(char str[]);
int foundDone(char str[]);
void promptLine();
void freeArray(char ** array, int sum);
int pipeIsNeeded(char str[]);
int doPipes(char str[], int neededPipe);
int comWithoutPipe(char str[], int sum, int cd);
void sig_handler(int sigNum);
void sig_handler2(int sigNum);
int foundFg(char str[]);

int main() {

    signal(SIGTSTP, sig_handler2);
    signal(SIGCHLD, sig_handler);

    char str[MAX];
    int sum, count = 0, cd, done, needPipe, counterPipes = 0;

    while (1) {

        promptLine(); //print the prompt line
        fgets(str, MAX, stdin);
        count++;
        if (strcmp(str, "\n") == 0) //Check if the user entered only \n
            count -= 1;
        cd = foundCd(str);
        done = foundDone(str);
        sum = wordSum(str);
        needPipe = pipeIsNeeded(str); //How many | were inserted

        if (done == 1) { //Print out the statistics
            printf("Number of commands: %d \n", count - 1);
            printf("Number of pipes: %d \n", counterPipes);
            printf("See you Next time!\n");
            exit(1);
        }

        if (needPipe > 2) {
            printf("not supported!\n");
            count -= 1; //The illegal order is removed from the count
            continue;
        }

        counterPipes += needPipe;

        if (needPipe > 0) {
            doPipes(str, needPipe);
            continue;
        }

        comWithoutPipe(str, sum, cd);

    }
}

char ** buildArray(char str[], int sum) {
    int z, i, j, k, counter = 0, space = 0, len = sum + 1;
    char ** array;
    array = (char ** ) malloc(sizeof(char * ) * (len));
    if (array == NULL) {
        perror("malloc failed");
        freeArray(array, len);
        exit(1);
    }
    for (z = 0; z < len; z++) {

        array[len - 1] = NULL;

        for (i = 0; i <= strlen(str) + 1; i++) {
            /*We will run on the char [] that the method received,
                                                           each time we identify a word we will alloc a new array that the array ** will point to */

            if (str[i] == '"') {
                i++;
                while (str[i] != '"') {
                    counter++;
                    i++;
                }
            }
            if (str[i] != ' ' && str[i] != '\n' && str[i] != '"' && str[i] != '\0') {
                counter++;
            } else {
                if (str[i + 1] == ' ' || str[i + 1] == '\n') {
                    space += 1;
                } else {
                    if (counter == 0) {
                        /*if counter=0 it means that all the char until now are spaces.
                                                       Reset the spaces counter and continue in the loop*/
                        space = 0;
                        continue;
                    }
                    int size = counter + 1;
                    array[z] = (char * ) malloc(sizeof(char) * size);
                    if (array[z] == NULL) {
                        perror("malloc failed");
                        free(array[z]);
                        exit(1);
                    }
                    for (k = i - counter - space, j = 0; k < i - space + 1; k++, j++) {
                        if (counter == 1) { //Case of one char
                            array[z][j] = str[i - 1 - space];
                            array[z][j + 1] = '\0';
                        }
                        if (j == size - 1) {
                            array[z][j] = '\0';
                        } else {
                            array[z][j] = str[k];
                        }
                    }
                    counter = 0;
                    space = 0;
                    z++;
                }
            }

        }
        if (i == strlen(str))
            break;
    }

    return array;
}

int wordSum(char str[]) {
    int i, sum = 0;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == '"') { //if the user entered a command with "" it is considered as one word
            int j = i + 1;
            while (str[j] != '"') {
                j++;
            }
            i = j + 1;
        }
        if (str[i] == ' ') {
            if (str[i + 1] != ' ' && str[i + 1] != '\n' && str[i + 1] != '\0') {
                sum++;
            }
        }
    }
    if ((strlen(str)) > 0) {
        /*if the string is not empty and different from spaces or line drop
         * we add 1 to sum for the first word*/
        if (str[0] != ' ' && str[0] != '\n') {
            sum += 1;
        }
    }
    return sum;
}

int foundCd(char str[]) {
    int i, found = 0;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == 'c') {
            if (i > 1 && strlen(str) > 3) {
                if (str[i - 1] != ' ') //Check if 'cd' appears in the middle of a word->false
                    found = 0;
                break;
            }
            if (str[i + 1] == 'd') {
                if (str[i + 2] == ' ' || str[i + 2] == '\n' || str[i + 2] == '\0')
                    found = 1;
            }
        }

    }
    return found;
}

int foundFg(char str[]) {
    int i, found = 0;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == 'f') {
            if (str[i + 1] == 'g' && str[i + 2] == '\n')
                found = 1;
        }
    }
    return found;
}

int foundDone(char str[]) {
    int i, found = 0, k = 0;
    if (strlen(str) > 4) {
        if (str[0] == 'd' && str[1] == 'o' && str[2] == 'n' && str[3] == 'e') { //Check if done at first
            for (i = 4; i < strlen(str); i++) {
                if (str[i] != ' ' && str[i] != '\n') {
                    found = -1;
                    break;
                }

                found = 1;
            }

        } else {
            if (str[k] == ' ') { //Check if done appears after spaces
                while (str[k + 1] == ' ') {
                    k++;
                }
                if (str[k + 1] == 'd' && str[k + 2] == 'o' && str[k + 3] == 'n' && str[k + 4] == 'e') {
                    for (i = k + 5; i < strlen(str); i++) {
                        if (str[i] != ' ' && str[i] != '\n') {
                            found = -1;
                            break;
                        }
                        found = 1;
                    }

                }
            }
        }
    }

    return found;
}

void promptLine() { //the method print the prompt line
    struct passwd * userName;
    char * userPath;
    char path[PATH_MAX + 1];
    if ((userName = getpwuid(getuid())) == NULL) {
        fprintf(stderr, "Error\n");
        //    return (EXIT_FAILURE);
    }
    userPath = getcwd(path, PATH_MAX + 1);
    if (userPath != NULL) {
        printf("%s@%s>", userName -> pw_name, userPath);
    }
    if (userName == NULL) {
        printf("null@%s>\n", userPath);
    }
}

void freeArray(char ** array, int sum) {
    for (int i = 0; i < sum; i++) {
        free(array[i]);
    }
    free(array);
}

int pipeIsNeeded(char str[]) { //identify if it exists in the string char '|'
    int i, found = 0;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == '|') {
            found++;
        }
    }
    return found;
}

int doPipes(char str[], int neededPipe) { //Create a pipe and it signals to the shell to route the
    //stdout of the command before it to the stdin of the command after it
    pid_t son1, son2, son3;
    int pipe_fd[2];
    int pipe2_fd[2];
    if (pipe(pipe_fd) == -1) { //error
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe2_fd) == -1) { //error
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    char * command1 = strtok(str, "|"); //The original string is cut to the char |
    int sum1 = wordSum(command1);

    if (foundDone(command1) == 1) {
        printf("command not found\n");
        return 1;
    }

    command1[strlen(command1)] = '\0';
    char ** firstCom = buildArray(command1, sum1); //Build the array with the single command
    son1 = fork();

    if (son1 < 0) {
        perror("fork failed");
        exit(1);
    }

    if (son1 == 0) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        if (foundCd(command1) == 1) {
            fprintf(stderr, "command not supported (Yet)\n");
            freeArray(firstCom, sum1);
            close(pipe_fd[1]);
            exit(1);
        }
        if (execvp(firstCom[0], firstCom) < 0) {
            perror("command not found\n");
            freeArray(firstCom, sum1); //free the first array and exit
            exit(1);
        }
    }

    wait(NULL);
    close(pipe_fd[1]);

    char * command2 = strtok(NULL, "|"); //The original string is cut from the char |
    int sum2 = wordSum(command2);

    if (foundDone(command2) == 1) {
        printf("command not found\n");
        freeArray(firstCom, sum1);
        close(pipe_fd[1]);
        return 1;
    }

    command2[strlen(command2)] = '\0';
    char ** secondCom = buildArray(command2, sum2); //Build the array with the single command

    son2 = fork();

    if (son2 < 0) {
        perror("fork failed");
        exit(1);
    }

    if (neededPipe == 1) {

        if (son2 == 0) {
            close(pipe_fd[1]);
            dup2(pipe_fd[0], STDIN_FILENO);
            if (foundCd(command2) == 1) {
                fprintf(stderr, "command not supported (Yet)\n");
                freeArray(firstCom, sum1);
                freeArray(secondCom, sum2);
                close(pipe_fd[0]);
                exit(1);
            }
            if (execvp(secondCom[0], secondCom) < 0) {
                perror("command not found\n");
                freeArray(firstCom, sum1);
                freeArray(secondCom, sum2);
                close(pipe_fd[0]);
                exit(1);
            }

        }
        //Now the father's job is to close the fd and free all the arrays
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        wait(NULL);
        freeArray(firstCom, sum1);
        freeArray(secondCom, sum2);
    }

    if (neededPipe == 2) {

        if (son2 == 0) {
            close(pipe_fd[1]);
            close(pipe2_fd[0]);
            dup2(pipe_fd[0], STDIN_FILENO);
            dup2(pipe2_fd[1], STDOUT_FILENO);
            if (foundCd(command2) == 1) {
                fprintf(stderr, "command not supported (Yet)\n");
                freeArray(firstCom, sum1);
                freeArray(secondCom, sum2);
                close(pipe_fd[0]);
                close(pipe2_fd[1]);
                exit(1);
            }
            if (execvp(secondCom[0], secondCom) < 0) {
                perror("command not found\n");
                freeArray(firstCom, sum1);
                freeArray(secondCom, sum2);
                close(pipe_fd[0]);
                close(pipe2_fd[1]);
                exit(1);
            }
        }

        wait(NULL);

        char * command3 = strtok(NULL, "|"); //The original string is cut from the char |
        int sum3 = wordSum(command3);

        if (foundDone(command3) == 1) {
            printf("command not found\n");
            freeArray(firstCom, sum1);
            freeArray(secondCom, sum2);
            close(pipe_fd[0]);
            close(pipe2_fd[1]);
            return 1;
        }
        command3[strlen(command3) - 1] = '\0';
        char ** thirdCom = buildArray(command3, sum3); //Build the array with the single command

        son3 = fork();

        if (son3 < 0) {
            perror("fork failed");
            exit(1);
        }
        if (son3 == 0) {
            close(pipe2_fd[1]);
            dup2(pipe2_fd[0], STDIN_FILENO);
            if (foundCd(command2) == 1) {
                fprintf(stderr, "command not supported (Yet)\n");
                freeArray(firstCom, sum1);
                freeArray(secondCom, sum2);
                freeArray(thirdCom, sum3);
                close(pipe2_fd[0]);
                exit(1);
            }
            if (execvp(thirdCom[0], thirdCom) < 0) {
                perror("command not found\n");
                freeArray(firstCom, sum1);
                freeArray(secondCom, sum2);
                freeArray(thirdCom, sum3);
                close(pipe2_fd[0]);
                exit(1);
            }
            close(pipe2_fd[0]);
        }
        //Now the father's job is to close the fd and free all the arrays
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        close(pipe2_fd[0]);
        close(pipe2_fd[1]);
        wait(NULL);
        freeArray(firstCom, sum1);
        freeArray(secondCom, sum2);
        freeArray(thirdCom, sum3);
    }

    return 0;
}

int comWithoutPipe(char str[], int sum, int cd) {
    //The method reads commands without | From the user and delivers them in a format appropriate to the operating system
    char ** array = buildArray(str, sum);
    int fg = foundFg(str);

    if (cd == 1) { // if we found a 'cd' print error and free the arr
        printf("command not supported (Yet)\n");
        freeArray(array, sum);
        return 1;
    }

    son = fork();
    currSon = son; //A global variable that keeps the son's pid

    if (fg == 1) { //son's process is returned to run
        kill(tempSon, SIGCONT);
        pause();
    }

    if (son < 0) { //if fork is failed free the arr
        perror("Fork is failed");
        freeArray(array, sum);
        exit(1);
    }
    if (son == 0) {
        signal(SIGTSTP, SIG_DFL);
        if (execvp(array[0], array) < 0) {
            if (fg != 1)
                printf("command not found\n");
            exit(1);
        }
    }

    waitpid(son, NULL, WUNTRACED);
    freeArray(array, sum);

    return 0;
}

void sig_handler(int sigNum) {
    waitpid(-1, NULL, WNOHANG);
}

void sig_handler2(int sigNum) {
    tempSon = currSon;
}