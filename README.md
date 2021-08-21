Shell
Authored by Stav David

==Description==
The program reads commands from the user and transmits them in the appropriate format for executing the operating system. 
The program supports pipes, and signals such as Ctrl z, fg.

==functions==
1. buildArray:The method gets char [] and parses it and separates into commands. The method assigns a character ** and points to any array that has one command. The method returns an array **. 
 In a change from the previous exercise, the exercise supports quotation marks so we put in one place in the array all the words between the quotation marks without the quotation marks.
2. wordSum:The method gets a string and counts how many words there are in the string. Change from previous exercise: 
The exercise supports quotation marks so we will count all the words that are inside the quotation marks as one word.
3. foundCd:The method checks if the command we received is a 'cd'
4. foundDone:The method checks if the command we received is a 'done'
5. promptLine:The method prints the prompt line
6. freeArray:The method releases all allocations
7. pipeIsNeeded:The method checks if the string we have received needs a pipe.
8. doPipes:The method is divided into 2 - if one pipe is needed the method routes the stdout of the command before the pipe to the stdin of the next command,
  if 2 pipes are needed the method does the same thing with only 2 pipes and 3 son processes.
9. comWithoutPipe:The program reads commands from the user and transmits them in a format appropriate for the operating system to execute.
10. sig_handler: The function will wait until the son who sent ' SIGCHLD 'and release his resources
11. sig_handler2:The function saves the pid of the last suspended son
12. foundFg:The method checks if the command we received is a 'fg'

==Program Files==
ex4.c- contain the main and all the functions.

==How to compile?==
compile: gcc ex4.c -o ex4
run: ./ex4

==Input==
Sentence (max 510 characters).
for example : ls -l | sort | wc 

==Output==
Execution of a command
for example: 8      65     428