#include "parser/ast.h"
#include "shell.h"

//Mert Ekici 2741471
//Operating Systems Assignment 1 - Shell

#include "signal.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <limits.h>
#include <pwd.h>

#define DATA_SIZE 100

//Global and Environmental Variables
char *prevdir = "";
char *envvar = "PS1";

//struct for collecting set-unset data
struct setdata
{
    char *info;
    int s;
};

struct setdata data[DATA_SIZE];
int numsetdata = 0;

//track the pid's of foreground and background processes and group ids
pid_t fore_pid = 1;
pid_t back_pid = 1;

pid_t fore_pgid;
pid_t back_pgid;

//background sequence's next node to run after catching previous one when it ends
node_t *b_node;

//replace function for char arrays
char *replace_substr(const char *mainstring, const char *old, const char *new)
{

    int i, count = 0;

    int oldlen = strlen(old);
    int newlen = strlen(new);

    char *result;

    // find the number of old's occurrence in mainstring
    for (i = 0; mainstring[i] != '\0'; i++)
    {
        if (strstr(&mainstring[i], old) == &mainstring[i])
        {
            count++;

            //if found step over to next index
            i += oldlen - 1;
        }
    }

    // arranging the size of result based on the count
    result = (char *)malloc(i + count * (newlen - oldlen) + 1);

    i = 0;
    while (*mainstring)
    {
        //if occurrence found, copy it into result
        if (strstr(mainstring, old) == mainstring)
        {
            strcpy(&result[i], new);
            mainstring += oldlen;
            i += newlen;
        }
        //else, increment i and copy the character
        else
            result[i++] = *mainstring++;
    }

    result[i] = '\0';
    return result;
}

//signal handlers for ctrl-c ctrl-z
void signal_handler_c()
{
    kill(fore_pid, SIGKILL);
}

void signal_handler_z()
{
    kill(-fore_pid, SIGSTOP);
    back_pid = fore_pid;
    fore_pid = 1;
}

void ctrl_c()
{
    struct sigaction s;
    s.sa_handler = signal_handler_c;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_RESTART;
    sigaction(SIGINT, &s, NULL);
}

//job control functions
void ctrl_z()
{
    signal(SIGTSTP, signal_handler_z);
}

void bg_handler()
{

    kill(-back_pid, SIGCONT);
}

void fg_handler()
{

    ctrl_z();

    fore_pid = back_pid;
    back_pid = 1;
    kill(fore_pid, SIGCONT);

    waitpid(fore_pid, NULL, WUNTRACED);
}

//set, unset, cd, exit functions
void set_handler(node_t *node, int sub)
{
    if (putenv(node->command.argv[1]) < 0)
    {
        perror(node->command.argv[1]);
    }
    //if not subshell collect data
    if (!sub)
    {
        struct setdata sdata;
        sdata.info = node->command.argv[1];
        sdata.s = 1;
        data[numsetdata] = sdata;
        numsetdata++;
    }
}

void unset_handler(node_t *node, int sub)
{
    if (unsetenv(node->command.argv[1]) < 0)
    {
        perror(node->command.argv[1]);
    }
    //if not subshell collect data
    if (!sub)
    {
        struct setdata sdata;
        sdata.info = node->command.argv[1];
        sdata.s = 0;
        data[numsetdata] = sdata;
        numsetdata++;
    }
}

void exit_handler(node_t *node)
{

    if (node->command.argv[1])
    {
        while (wait(NULL) > 0)
            ;
        exit(atoi(node->command.argv[1]));
    }
    else
    {
        exit(0);
    }
}

void cd_handler(node_t *node, int sub)
{
    if (chdir(node->command.argv[1]) < 0)
    {
        perror(node->command.argv[1]);
    }
    if (!sub)
    {
        prevdir = node->command.argv[1];
    }
}

//command handler function, if type is command this function is called
void command_handler(node_t *node)
{

    if (strcmp(node->command.program, "exit") == 0)
    {
        exit_handler(node);
    }
    else if (strcmp(node->command.program, "cd") == 0)
    {
        cd_handler(node, 0);
    }
    else if (strcmp(node->command.program, "set") == 0)
    {
        set_handler(node, 0);
    }
    else if (strcmp(node->command.program, "unset") == 0)
    {
        unset_handler(node, 0);
    }
    else if (strcmp(node->command.program, "bg") == 0)
    {
        bg_handler();
    }
    else if (strcmp(node->command.program, "fg") == 0)
    {
        fg_handler();
    }

    else
    {
        fore_pid = fork();
        int status;

        if (fore_pid < 0)
        {
            perror("fork error");
        }

        else if (fore_pid == 0)
        {
            setpgid(0, fore_pgid);
            if (execvp(node->command.program, node->command.argv) < 0)
            {
                perror(node->command.program);
                exit(1);
            }
            //execvp(node->command.program, node->command.argv);
        }
        else
        {
            waitpid(fore_pid, &status, WUNTRACED);
        }
    }
}

//pipe handler function and other pipe utility functions
int seq_counter(node_t *node, int count)
{

    //counts the commands in the sequence (to wait after pipe ends)
    node_t *f_node = node->sequence.first;
    node_t *s_node = node->sequence.second;

    if (f_node->type == NODE_COMMAND)
    {
        count++;
    }

    if (f_node->type == NODE_SEQUENCE)
    {
        count += seq_counter(node, count);
    }

    if (s_node->type == NODE_COMMAND)
    {

        count++;
    }

    if (s_node->type == NODE_SEQUENCE)
    {
        count += seq_counter(node, count);
    }
    return count;
}

void pseq_handler(node_t *node, int fds[], int counter, int k, int npipes)
{
    //pipe sequence handler
    node_t *f_node = node->sequence.first;
    node_t *s_node = node->sequence.second;

    if (f_node->type == NODE_COMMAND)
    {
        if (strcmp(node->command.program, "exit") == 0)
        {
            exit_handler(node);
        }
        else if (strcmp(node->command.program, "cd") == 0)
        {
            cd_handler(node, 0);
        }
        else if (strcmp(node->command.program, "set") == 0)
        {
            set_handler(node, 0);
        }
        else if (strcmp(node->command.program, "unset") == 0)
        {
            unset_handler(node, 0);
        }
        else if (strcmp(node->command.program, "bg") == 0)
        {
            bg_handler();
        }
        else if (strcmp(node->command.program, "fg") == 0)
        {
            fg_handler();
        }

        else
        {
            pid_t pid = fork();

            if (pid == 0)
            {
                setpgid(0, fore_pgid);
                if (k != npipes)
                {
                    dup2(fds[counter + 1], STDOUT_FILENO);
                }
                if (k != 0)
                {
                    dup2(fds[counter - 2], STDIN_FILENO);
                }

                for (int j = 0; j < 2 * npipes; j++)
                {
                    close(fds[j]);
                }

                if (execvp(f_node->command.program, f_node->command.argv) < 0)
                {
                    exit(1);
                }
            }

            else if (pid < 0)
            {
                exit(1);
            }
        }
    }

    if (f_node->type == NODE_SEQUENCE)
    {
        pseq_handler(f_node, fds, counter, k, npipes);
    }

    if (s_node->type == NODE_COMMAND)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            setpgid(0, fore_pgid);

            if (k != npipes)
            {
                dup2(fds[counter + 1], STDOUT_FILENO);
            }
            if (k != 0)
            {
                dup2(fds[counter - 2], STDIN_FILENO);
            }

            for (int j = 0; j < 2 * npipes; j++)
            {
                close(fds[j]);
            }

            if (execvp(s_node->command.program, s_node->command.argv) < 0)
            {
                exit(1);
            }
        }
        else if (pid < 0)
        {
            exit(1);
        }
    }

    if (s_node->type == NODE_SEQUENCE)
    {
        pseq_handler(s_node, fds, counter, k, npipes);
    }
}

void pipe_handler(node_t *node)
{

    int npipes = node->pipe.n_parts - 1;
    pid_t pid;
    int status;
    int fds[2 * (npipes)];
    int excom = 0;

    for (int i = 0; i < npipes; i++)
    {
        if (pipe(fds + i * 2) < 0)
        {
            exit(1);
        }
    }

    int counter = 0;
    for (int k = 0; k < npipes + 1; k += 1)
    {

        if (node->pipe.parts[k]->type == NODE_COMMAND)
        {
            if (strcmp(node->command.program, "exit") == 0)
            {
                exit_handler(node);
            }
            else if (strcmp(node->command.program, "cd") == 0)
            {
                cd_handler(node, 0);
            }
            else if (strcmp(node->command.program, "set") == 0)
            {
                set_handler(node, 0);
            }
            else if (strcmp(node->command.program, "unset") == 0)
            {
                unset_handler(node, 0);
            }
            else if (strcmp(node->command.program, "bg") == 0)
            {
                bg_handler();
            }
            else if (strcmp(node->command.program, "fg") == 0)
            {
                fg_handler();
            }

            else
            {
                pid = fork();

                if (pid < 0)
                {
                    perror("fork error");
                }

                else if (pid == 0)
                {

                    if (k != npipes)
                    {
                        dup2(fds[counter + 1], STDOUT_FILENO);
                    }
                    if (k != 0)
                    {
                        dup2(fds[counter - 2], STDIN_FILENO);
                    }

                    for (int j = 0; j < 2 * npipes; j++)
                    {
                        close(fds[j]);
                    }

                    if (execvp(node->pipe.parts[k]->command.program, node->pipe.parts[k]->command.argv) < 0)
                    {
                        exit(1);
                    }
                }
            }
        }

        if (node->pipe.parts[k]->type == NODE_SEQUENCE)
        {
            pseq_handler(node->pipe.parts[k], fds, counter, k, npipes);
            excom += seq_counter(node->pipe.parts[k], 0) - 1;
        }

        counter += 2;
    }

    for (int j = 0; j < 2 * npipes; j++)
    {
        close(fds[j]);
    }

    //wait for commands (extra commands in sequence are included)
    for (int i = 0; i < npipes + 1 + excom; i++)
    {
        wait(&status);
    }
}

//redirection handler function
void redirection_handler(node_t *node)
{

    node_t *c_node = node->redirect.child;
    char *target = node->redirect.target;

    if (c_node->type == NODE_COMMAND && node->redirect.mode != REDIRECT_DUP)
    {
        int status;
        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            perror("fork error");
            return;
        }
        else if (pid > 0)
        {
            wait(&status);
        }
        else if (pid == 0)
        {

            if (node->redirect.mode == REDIRECT_INPUT)
            {
                //open file with read only
                int in = open(target, O_RDONLY);
                dup2(in, 0);
                close(in);
            }
            if (node->redirect.mode == REDIRECT_OUTPUT)
            {
                //read & write for the user, only read for groups and others
                int out = creat(target, 0644);
                dup2(out, 1);

                close(out);
            }
            if (node->redirect.mode == REDIRECT_APPEND)
            {
                int append = open(target, O_CREAT | O_APPEND | O_RDWR, S_IRGRP | S_IROTH | S_IRUSR | S_IWUSR);
                dup2(append, 1);
                close(append);
            }
            if (execvp(c_node->command.program, c_node->command.argv) < 0)
            {
                exit(1);
            }
        }
    }

    // 2&>1 - > - command

    if (c_node->type == NODE_REDIRECT && node->redirect.mode == REDIRECT_DUP)
    {

        target = c_node->redirect.target;
        int status;
        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            perror("fork error");
            return;
        }
        else if (pid > 0)
        {
            wait(&status);
        }
        else if (pid == 0)
        {
            dup2(1, 2);
            //read & write for the user, only read for groups and others
            int out = creat(target, 0644);
            dup2(out, 1);
            close(out);
            if (execvp(c_node->redirect.child->command.program, c_node->redirect.child->command.argv) < 0)
            {
                exit(1);
            }
        }
    }

    // > - command - 2&>1
    if (c_node->type == NODE_REDIRECT && node->redirect.mode == REDIRECT_OUTPUT)
    {
        int out = creat(target, 0644);
        int status;
        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            perror("fork error");
            return;
        }
        else if (pid > 0)
        {
            wait(&status);
        }
        else if (pid == 0)
        {
            dup2(out, 1);
            dup2(out, 2);
            close(out);
            if (execvp(c_node->redirect.child->command.program, c_node->redirect.child->command.argv) < 0)
            {
                exit(1);
            }
        }
    }
}

//detach handler function and detach utility functions
void dcom_handler(node_t *node)
{
    if (strcmp(node->command.program, "exit") == 0)
    {
        exit_handler(node);
    }
    else if (strcmp(node->command.program, "cd") == 0)
    {
        cd_handler(node, 0);
    }
    else if (strcmp(node->command.program, "set") == 0)
    {
        set_handler(node, 0);
    }
    else if (strcmp(node->command.program, "unset") == 0)
    {
        unset_handler(node, 0);
    }

    else
    {
        back_pid = fork();

        if (back_pid < 0)
        {
            perror("fork error");
        }

        else if (back_pid == 0)
        {
            setpgid(0, back_pgid);
            if (execvp(node->command.program, node->command.argv) < 0)
            {
                perror(node->command.program);
                exit(1);
            }
        }
    }
}

void dseq_handler(node_t *node); //definition to prevent errors (for nested functions)

void sigchld_handler()
{
    int status;
    pid_t pid;
    pid = waitpid(-1, &status, WNOHANG);
    if (pid == back_pid && back_pid != 1 && b_node)
    {
        if (b_node->type == NODE_COMMAND)
        {
            dcom_handler(b_node);
            b_node = NULL;
        }
        else if (b_node->type == NODE_SEQUENCE)
        {
            dseq_handler(b_node);
        }
    }
}

void sigchld_catch()
{
    //when a part of background sequence is done, signal gets caught and handled as execution of next command
    struct sigaction s;
    s.sa_handler = sigchld_handler;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &s, NULL);
}

void dseq_handler(node_t *node)
{
    node_t *f_node = node->sequence.first;
    b_node = node->sequence.second;

    if (f_node->type == NODE_COMMAND)
    {
        dcom_handler(f_node);
    }

    if (f_node->type == NODE_SEQUENCE)
    {
        dseq_handler(f_node);
    }

    sigchld_catch();
}

void detach_handler(node_t *node)
{

    if (node->detach.child->type == NODE_COMMAND)
    {
        dcom_handler(node->detach.child);
    }

    if (node->detach.child->type == NODE_SEQUENCE)
    {
        dseq_handler(node->detach.child);
    }
}

//subshell handler and its utility functions
void subshell_handler(node_t *node); //definition to prevent errors (for nested functions)

void command_handler_subshell(node_t *node)
{

    if (strcmp(node->command.program, "exit") == 0)
    {
        //do not exit from the main shell
    }
    else if (strcmp(node->command.program, "cd") == 0)
    {
        cd_handler(node, 1);
    }
    else if (strcmp(node->command.program, "set") == 0)
    {
        set_handler(node, 1);
    }
    else if (strcmp(node->command.program, "unset") == 0)
    {
        unset_handler(node, 1);
    }

    else
    {
        int pid = fork();
        int status;

        if (pid < 0)
        {
            perror("fork error");
        }
        else if (pid == 0)
        {
            if (execvp(node->command.program, node->command.argv) < 0)
            {
                perror(node->command.program);
                exit(1);
            }
        }
        else
        {
            wait(&status);
        }
    }
}

void sequence_handler_subshell(node_t *node)
{
    node_t *f_node = node->sequence.first;
    node_t *s_node = node->sequence.second;

    if (f_node->type == NODE_COMMAND)
    {
        command_handler_subshell(f_node);
    }

    if (f_node->type == NODE_SEQUENCE)
    {
        sequence_handler_subshell(f_node);
    }

    if (f_node->type == NODE_PIPE)
    {
        pipe_handler(f_node);
    }

    if (f_node->type == NODE_REDIRECT)
    {
        redirection_handler(f_node);
    }
    if (f_node->type == NODE_DETACH)
    {
        detach_handler(f_node);
    }
    if (f_node->type == NODE_SUBSHELL)
    {
        subshell_handler(f_node);
    }

    if (s_node->type == NODE_COMMAND)
    {

        command_handler_subshell(s_node);
    }

    if (s_node->type == NODE_SEQUENCE)
    {
        sequence_handler_subshell(s_node);
    }

    if (s_node->type == NODE_PIPE)
    {
        pipe_handler(s_node);
    }
    if (s_node->type == NODE_REDIRECT)
    {
        redirection_handler(s_node);
    }
    if (s_node->type == NODE_DETACH)
    {
        detach_handler(s_node);
    }
    if (s_node->type == NODE_SUBSHELL)
    {
        subshell_handler(s_node);
    }
}

void shell_refresh()
{
    //after subshell is done, refreshes the current shell and information and turns it into the version on main shell
    if (strcmp(prevdir, "") != 0)
    {
        chdir(prevdir);
    }

    clearenv();
    for (int i = 0; i <= numsetdata; i++)
    {

        if (data[i].s)
        {
            putenv(data[i].info);
        }
        else
        {
            unsetenv(data[i].info);
        }
    }
}

void run_command_subshell(node_t *node)
{

    /* Print parsed input for testing - comment this when running the tests! */
    //print_tree(node);

    ctrl_c();

    if (node->type == NODE_COMMAND)
    {
        //printf("c");
        command_handler_subshell(node);
    }

    //remains same
    if (node->type == NODE_PIPE)
    {
        //printf("p");
        pipe_handler(node);
    }

    if (node->type == NODE_SEQUENCE)
    {

        sequence_handler_subshell(node);
    }

    //remains same
    if (node->type == NODE_REDIRECT)
    {
        redirection_handler(node);
    }

    if (node->type == NODE_DETACH)
    {
        detach_handler(node);
    }

    //remains same
    if (node->type == NODE_SUBSHELL)
    {
        subshell_handler(node);
    }

    if (prompt)
        prompt = "vush$ ";
}

void subshell_handler(node_t *node)
{
    node_t *c_node = node->subshell.child;
    run_command_subshell(c_node);
    shell_refresh();
}

//sequence handler function - directs its parts to other handler functions
void sequence_handler(node_t *node)
{
    node_t *f_node = node->sequence.first;
    node_t *s_node = node->sequence.second;

    if (f_node->type == NODE_COMMAND)
    {
        command_handler(f_node);
    }

    if (f_node->type == NODE_SEQUENCE)
    {
        sequence_handler(f_node);
    }

    if (f_node->type == NODE_PIPE)
    {
        pipe_handler(f_node);
    }

    if (f_node->type == NODE_REDIRECT)
    {
        redirection_handler(f_node);
    }
    if (f_node->type == NODE_DETACH)
    {
        detach_handler(f_node);
    }
    if (f_node->type == NODE_SUBSHELL)
    {
        subshell_handler(f_node);
    }

    if (s_node->type == NODE_COMMAND)
    {

        command_handler(s_node);
    }

    if (s_node->type == NODE_SEQUENCE)
    {
        sequence_handler(s_node);
    }

    if (s_node->type == NODE_PIPE)
    {
        pipe_handler(s_node);
    }
    if (s_node->type == NODE_REDIRECT)
    {
        redirection_handler(s_node);
    }
    if (s_node->type == NODE_DETACH)
    {
        detach_handler(s_node);
    }
    if (s_node->type == NODE_SUBSHELL)
    {
        subshell_handler(s_node);
    }
}

//PS1 environment variable checker (if its declared or changed updates the prompt)
char *PS1_checker(char *prompt)
{

    if (!getenv(envvar))
    {
        return "vush$ ";
    }
    else
    {

        prompt = getenv(envvar);

        char hostname[HOST_NAME_MAX + 1];
        gethostname(hostname, HOST_NAME_MAX + 1); //get host name

        char path[200];
        getcwd(path, 200); //get directory

        struct passwd *pwd;
        pwd = getpwuid(geteuid()); //get username from the struct passwd (passwrd->pw_name)

        prompt = replace_substr(prompt, "\\h", hostname);
        prompt = replace_substr(prompt, "\\w", path);
        prompt = replace_substr(prompt, "\\u", pwd->pw_name);

        return prompt;
    }
}

//initial functions
void initialize(void)
{

    /* This code will be called once at startup */
    if (prompt)
    {
        prompt = PS1_checker(prompt);
    }

    ctrl_c();
    ctrl_z();
    sigchld_catch();
}

void run_command(node_t *node)
{

    /* Print parsed input for testing - comment this when running the tests! */
    //print_tree(node);

    ctrl_c();
    ctrl_z();
    sigchld_catch();

    if (node->type == NODE_COMMAND)
    {
        //printf("c");
        command_handler(node);
    }

    if (node->type == NODE_PIPE)
    {
        //printf("p");
        pipe_handler(node);
    }

    if (node->type == NODE_SEQUENCE)
    {
        //printf("s");
        sequence_handler(node);
    }

    if (node->type == NODE_REDIRECT)
    {
        //printf("r");
        redirection_handler(node);
    }

    if (node->type == NODE_DETACH)
    {
        //printf("d");
        detach_handler(node);
    }

    if (node->type == NODE_SUBSHELL)
    {
        //printf("sub");
        subshell_handler(node);
    }

    if (prompt)
    {
        prompt = PS1_checker(prompt);
    }
}
