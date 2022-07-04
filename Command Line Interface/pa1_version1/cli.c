#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

// Mert Ekici 26772
// CS307 Operating Systems - PA1
// ekicimert@sabanciuniv.edu

#define FILENAME "commands.txt"
char *prevdir = "";

// command struct related
struct command
{
    char *name;
    char *input;
    char *input2;
    char *option;
    char rd;
    char *rdFile;
    char bg;
    int size;
};

void defaultCommand(struct command *cmd)
{
    cmd->name = NULL;
    cmd->input = NULL;
    cmd->input2 = NULL;
    cmd->option = NULL;
    cmd->rd = '\0';
    cmd->rdFile = NULL;
    cmd->bg = '\0';
}

void sizeUpdate(struct command *cmd)
{
    cmd->size = 0;
    if (cmd->name)
    {
        cmd->size++;
    }
    if (cmd->input)
    {
        cmd->size++;
    }
    if (cmd->input2)
    {
        cmd->size++;
    }
    if (cmd->option)
    {
        cmd->size++;
    }
}

// command array
void commandArray(char *arglist[], struct command cmd, int opt)
{

    for (int i = 0; i < cmd.size; i++)
    {
        if (i == 0)
        {
            arglist[i] = strdup(cmd.name);
        }
        else if (i == 1)
        {
            if (opt == 0)
            {
                if (cmd.input)
                {
                    arglist[i] = strdup(cmd.input);
                }
                else if (cmd.option)
                {
                    arglist[i] = strdup(cmd.option);
                }
            }
            else if (opt == 1)
            {
                arglist[i] = strdup(cmd.option);
            }
            else if (opt == 2)
            {
                if (cmd.input)
                {
                    arglist[i] = strdup(cmd.input);
                }
            }
            else if (opt == 3)
            {
                if (cmd.option)
                {
                    arglist[i] = strdup(cmd.option);
                }
            }
        }
        else if (i == 2)
        {
            if (opt == 0)
            {
                if (cmd.input2)
                {
                    arglist[i] = strdup(cmd.input2);
                }
                else if (cmd.option)
                {
                    arglist[i] = strdup(cmd.option);
                }
            }
            else if (opt == 1)
            {
                if (cmd.input)
                {
                    arglist[i] = strdup(cmd.input);
                }
            }
            else if (opt == 2)
            {
                if (cmd.option)
                {
                    arglist[i] = strdup(cmd.option);
                }
            }
            else if (opt == 3)
            {
                if (cmd.input)
                {
                    arglist[i] = strdup(cmd.input);
                }
            }
        }
        else
        {
            if (opt == 0)
            {
                if (cmd.option)
                {
                    arglist[i] = strdup(cmd.option);
                }
            }
            else if (opt == 1)
            {
                if (cmd.input2)
                {
                    arglist[i] = strdup(cmd.input2);
                }
            }
            else if (opt == 2)
            {
                if (cmd.input2)
                {
                    arglist[i] = strdup(cmd.input2);
                }
            }
            else if (opt == 3)
            {
                if (cmd.input2)
                {
                    arglist[i] = strdup(cmd.input2);
                }
            }
        }
    }
    arglist[cmd.size] = NULL;
}

void commandPrinter(char *arglist[], size_t s)
{
    for (int i = 0; i < s; i++)
    {
        printf("%s", arglist[i]);
        printf(" ");
    }
}

// special character escape functions: escape character
void escapeChar(char *line, char chr)
{
    int index = 0;

    while (line[index] != '\0')
    {
        if (line[index] == chr)
        {
            line[index] = ' ';
        }
        index++;
    }
}

// special character escape functions: special character check and remove
void specialChar(char *line)
{
    escapeChar(line, '\n');
    escapeChar(line, '\r');
    escapeChar(line, '\"');
    escapeChar(line, '\'');
    escapeChar(line, '\v');
    escapeChar(line, '\t');
}

void promptedCommand(struct command cmdO, struct command *cmdN, char *line)
{
    escapeChar(line, '\n');
    escapeChar(line, '\r');
    escapeChar(line, '\v');
    escapeChar(line, '\t');

    char *sepcmd = strtok(line, " ");
    cmdN->name = sepcmd;
    sepcmd = strtok(NULL, " ");

    while (sepcmd != NULL)
    {
        if (cmdO.input)
        {
            if (strstr(sepcmd, cmdO.input))
            {
                cmdN->input = strdup(sepcmd);
            }
        }
        if (cmdO.input2)
        {
            if (strstr(sepcmd, cmdO.input2))
            {
                cmdN->input2 = strdup(sepcmd);
            }
        }
        if (cmdO.option)
        {
            if (strstr(sepcmd, cmdO.option))
            {
                cmdN->option = strdup(sepcmd);
            }
        }
        sepcmd = strtok(NULL, " ");
    }
    cmdN->bg = cmdO.bg;
    cmdN->rd = cmdO.rd;
}

// main prompter
void prompter(struct command cmd, int opt)
{
    printf("----------\n");
    printf("Command: ");
    fflush(stdout);
    printf("%s", cmd.name);
    fflush(stdout);
    printf("\n");

    printf("Inputs: ");
    fflush(stdout);
    if (cmd.input && (opt != 1) && (opt != 3))
    {
        printf("%s", cmd.input);
        fflush(stdout);
    }
    if (cmd.input2 && (opt != 2) && (opt != 3))
    {
        printf(" %s", cmd.input2);
        fflush(stdout);
    }
    printf("\n");

    printf("Options: ");
    fflush(stdout);
    if (cmd.option)
    {
        printf("%s", cmd.option);
        fflush(stdout);
        if (opt == 1)
        {
            printf(" %s", cmd.input);
            fflush(stdout);
        }
        else if (opt == 2)
        {
            printf(" %s", cmd.input2);
            fflush(stdout);
        }
        else if (opt == 3)
        {
            printf(" %s", cmd.input);
            fflush(stdout);
            printf(" %s", cmd.input2);
            fflush(stdout);
        }
    }
    printf("\n");

    printf("Redirection: ");
    fflush(stdout);
    fflush(stdout);
    if (cmd.rd != '\0')
    {
        printf("%c", cmd.rd);
        fflush(stdout);
    }
    else
    {
        printf("%c", '-');
        fflush(stdout);
    }
    printf("\n");

    printf("Background: ");
    fflush(stdout);
    if (cmd.bg != '\0')
    {
        printf("%c", 'y');
        fflush(stdout);
    }
    else
    {
        printf("%c", 'n');
        fflush(stdout);
    }
    printf("\n");
    printf("----------\n");
}

int main(int argc, char *argv[])
{

    // File read variables
    char *line = NULL;
    size_t len = 0;
    FILE *file = fopen(FILENAME, "r");

    // File error
    if (file == NULL)
    {
        perror("File Error!");
        exit(1);
    }

    // Line by line reading
    while (getline(&line, &len, file) != -1)
    {
        struct command cmd;
        defaultCommand(&cmd);

        // for the prompter
        char *pureLine;
        struct command pureCmd;
        defaultCommand(&pureCmd);

        pureLine = strdup(line);
        // printf("%s",pureLine);

        // redirection dummy
        int rd = 0;
        // background dummy
        int bg = 0;
        // option input order variable
        int option_input = 0;

        specialChar(line);

        // Dividing the line by each whitespace
        char *sepcmd = strtok(line, " ");

        // First part of the line is "cmd_name" all the time
        // printf(" %s\n", sepcmd);
        if (sepcmd)
        {
            cmd.name = sepcmd;
            sepcmd = strtok(NULL, " ");

            while (sepcmd != NULL)
            {

                // Option
                if (strchr(sepcmd, '-') && sepcmd[0] == '-')
                {
                    if (!cmd.option)
                    {
                        cmd.option = strchr(sepcmd, '-');
                    }

                    else
                    {
                        if (!cmd.input)
                        {
                            cmd.input = sepcmd;

                            // first input is option's input

                            option_input = 1;
                        }
                        else
                        {
                            cmd.input2 = sepcmd;

                            // second input is option's input
                            if (option_input == 0)
                            {
                                option_input = 2;
                            }

                            // both inputs are option's inputs
                            else if (option_input == 1)
                            {
                                option_input = 3;
                            }
                        }
                    }
                }
                // Redirection1
                else if (strchr(sepcmd, '>') && !sepcmd[1])
                {
                    cmd.rd = '>';
                    sepcmd = strtok(NULL, " ");
                    cmd.rdFile = sepcmd;
                }
                // Redirection2
                else if (strchr(sepcmd, '<') && !sepcmd[1])
                {
                    cmd.rd = '<';
                    sepcmd = strtok(NULL, " ");
                    cmd.rdFile = sepcmd;
                }
                // Background
                else if (strchr(sepcmd, '&') && !sepcmd[1])
                {
                    cmd.bg = '&';
                }
                else
                {
                    if (!cmd.input)
                    {
                        cmd.input = sepcmd;

                        // first input is option's input
                        if (cmd.option)
                        {
                            option_input = 1;
                        }
                    }
                    else
                    {
                        cmd.input2 = sepcmd;

                        // second input is option's input
                        if (cmd.option && option_input == 0)
                        {
                            option_input = 2;
                        }

                        // both inputs are option's inputs
                        else if (cmd.option && option_input == 1)
                        {
                            option_input = 3;
                        }
                    }
                }

                sepcmd = strtok(NULL, " ");
            }

            // Parsed Command Prompter
            sizeUpdate(&cmd);

            promptedCommand(cmd, &pureCmd, pureLine);

            prompter(pureCmd, option_input);

            // background and redirection markers
            if (cmd.bg)
            {
                bg = 1;
            }
            if (cmd.rd == '>')
            {
                rd = 1;
            }
            else if (cmd.rd == '<')
            {
                rd = 2;
            }

            // wait handler
            if (!strcmp(cmd.name, "wait"))
            {

                while (wait(NULL) > 0)
                    ;
            }

            // cd handler
            else if (!strcmp(cmd.name, "cd"))
            {
                if (cmd.option)
                {
                    if (chdir(prevdir) < 0)
                    {
                        printf("%s", prevdir);
                        perror(prevdir);
                    }
                }
                else
                {
                    prevdir = getcwd(NULL, 0);
                    if (chdir(cmd.input) < 0)
                    {
                        perror(cmd.input);
                    }
                }
            }

            // NO REDIRECTION
            else if (rd == 0)
            {
                char *argument_list[cmd.size + 1];
                commandArray(argument_list, cmd, option_input);
                if (bg == 0)
                {

                    // printf(" ");
                    // commandPrinter(argument_list,cmd.size);
                    // printf("\n");

                    int rc = fork();
                    if (rc < 0)
                    {
                        fprintf(stderr, "fork failed\n");
                        exit(1);
                    }
                    else if (rc == 0)
                    {
                        execvp(argument_list[0], argument_list);
                    }
                    else
                    {
                        waitpid(rc, NULL, 0);
                    }
                }
                else if (bg == 1)
                {
                    int rc = fork();
                    if (rc < 0)
                    {
                        fprintf(stderr, "fork failed\n");
                        exit(1);
                    }
                    else if (rc == 0)
                    {
                        execvp(argument_list[0], argument_list);
                    }
                    else
                    {
                    }
                }
            }

            // OUTPUT REDIRECTION
            else if (rd == 1)
            {
                char *argument_list[cmd.size + 1];
                commandArray(argument_list, cmd, option_input);
                if (bg == 0)
                {

                    // printf(" ");
                    // commandPrinter(argument_list,cmd.size);
                    // printf("\n");

                    int rc = fork();
                    if (rc < 0)
                    {
                        fprintf(stderr, "fork failed\n");
                        exit(1);
                    }
                    else if (rc == 0)
                    {
                        close(STDOUT_FILENO);
                        open(cmd.rdFile, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
                        execvp(argument_list[0], argument_list);
                    }
                    else
                    {
                        waitpid(rc, NULL, 0);
                    }
                }
                else if (bg == 1)
                {
                    int rc = fork();
                    if (rc < 0)
                    {
                        fprintf(stderr, "fork failed\n");
                        exit(1);
                    }
                    else if (rc == 0)
                    {
                        close(STDOUT_FILENO);
                        open(cmd.rdFile, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
                        execvp(argument_list[0], argument_list);
                    }
                    else
                    {
                    }
                }
            }

            
        }
    }

    while (wait(NULL) > 0)
        ;

    fclose(file);
    free(line);
    return 0;
}