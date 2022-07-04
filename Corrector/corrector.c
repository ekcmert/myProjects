#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>

// Mert Ekici 26772 ekicimert@sabanciuniv.edu
// CS307 - PA5 File Systems

//node struct for the linked list which will hold the information stored in database
struct node
{
    struct node *next;
    char gender;
    char *name;
    char *surname;
};

//head of the database linkedlist
struct node *head;

//creates database linked-list by reading database.txt in the root directory of the executable
int create_DB()
{
    DIR *dir = opendir("."); // see all the directories and files in the current directory
    if (dir == NULL)
    {
        printf("Directory cannot be found!");
    }

    struct dirent *ent;
    ent = readdir(dir);

    while (ent != NULL)
    {
        //if database is found
        if (!strcmp(ent->d_name, "database.txt"))
        {

            FILE *fp;
            //open database.txt
            fp = fopen(ent->d_name, "r+");
            char line[256];
            while (fgets(line, sizeof(line), fp))
            {
                //remove new line, carriage return characters from the line to avoid possible string problems
                size_t lindex = strlen(line) - 2;
                if (line[lindex] == '\n' || line[lindex] == '\r' )
                {
                    line[lindex] = '\0';
                }
                size_t lsindex = strlen(line) - 1;
                if (line[lsindex] == '\n' || line[lsindex] == '\r')
                {
                    line[lsindex] = '\0';
                }

                //first slice is gender
                char *sepline = strtok(line, " ");

                //create a newnode and allocate a dynamic memory for it
                struct node *newnode = (struct node *)malloc(sizeof(struct node));
                if (sepline)
                {

                    newnode->next = head;
                    newnode->gender = *sepline;
                }

                //second slice is name
                sepline = strtok(NULL, " ");
                if (sepline)
                {
                    newnode->name = strdup(sepline);
                }

                //third slice is surname
                sepline = strtok(NULL, " ");
                if (sepline)
                {
                    newnode->surname = strdup(sepline);
                }

                //update the head (reverse insert)
                head = newnode;
            }
            closedir(dir);
            fclose(fp);
            return 1;
        }
        ent = readdir(dir);
    }
    closedir(dir);

    return 0;
}

//prints the database linked-list's content
void print_DB()
{
    struct node *curr = head;

    while (curr)
    {
        printf("Gender: %c ", curr->gender);
        printf("Name: %s ", curr->name);
        printf("Surname: %s\n", curr->surname);
        curr = curr->next;
    }
}

//searches for the entered name in the database linked-list, if there is a match returns the address of the node otherwise returns NULL pointer
struct node *find_DB(char *name)
{
    struct node *curr = head;

    while (curr)
    {
        if (!strcmp(curr->name, name))
        {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}

//main correction function - fixes the wrong prefixes and surnames by checking them on the database using ftell fseek and fputs
void correction(const char *dirName, int root)
{
    DIR *dir = opendir(dirName); // see all the directories and files in the current directory
    if (dir == NULL)
    {
        printf("Directory cannot be found!");
    }

    struct dirent *ent;
    ent = readdir(dir);

    while (ent != NULL)
    {
        // if a txt file is found other than database.txt or a database.txt found from a non-root directory
        if (strlen(ent->d_name) >= 4)
        {
            if ((!strcmp(strrchr(ent->d_name, '\0') - 4, ".txt") && strcmp(ent->d_name, "database.txt")) || (!strcmp(ent->d_name, "database.txt") && !root))
            {
                //create a path for opening the txt file
                char path[100] = {0};
                strcat(path, dirName);
                strcat(path, "/");
                strcat(path, ent->d_name);
            
                //open the file
                FILE *fp;
                fp = fopen(path, "r+");
                char line[256];

                char c;
                char word[100];
                int index = 0;

                //read the txt character by character into the word char array using char c and int index
                do
                {
                    c = (char)fgetc(fp);

                    // word found do the checks
                    if (c == ' ' || c == '\n' || c == '\0' || c == '\t' || c == '\r')
                    {
                        struct node *res = find_DB(word);
                        // string matches with one of the names --> check previous and next strings with fseek and update with fputs if necessary
                        if (res)
                        {
                            //Debug Prints (name)
                            //printf("word: %s", word);
                            //printf(" fd: %d\n", ftell(fp));

                            //prefix: go backwards (as the size of the name plus 5 -> 2 for whitespaces 3 for prefix) check prefix and fix if necessary 

                            fseek(fp, -(5 + strlen(word)), SEEK_CUR);

                            //Debug Prints (ftell)
                            //printf(" fdb: %d\n", ftell(fp));
                            //printf(" tell: %d\n", ftell(fp));

                            int pindex = 0;
                            char pc;
                            char pword[100];

                            do
                            {
                                pc = (char)fgetc(fp);

                                pword[pindex++] = pc;
                                pword[pindex] = 0;

                            } while (!(pc == ' ' || pc == '\n' || pc == '\0' || pc == '\t' || pc == '\r'));

                            pword[--pindex] = 0;

                            //Debug Prints (prefix)
                            //printf("prefix: %s", pword);
                            //printf(" fd: %d\n", ftell(fp));

                            //male but Ms.
                            if(res->gender == 'm' && !strcmp(pword, "Ms.")){
                                //if update is necessary go back again -> size plus 1 for space
                                fseek(fp, -(1+strlen(pword)), SEEK_CUR);

                                //printf("fdu: %d\n", ftell(fp));
                                fputs("Mr.",fp);
                                fseek(fp, 1, SEEK_CUR);
                                //printf("fdu: %d\n", ftell(fp));
                            }
                            //or female but Mr.
                            else if(res->gender == 'f' && !strcmp(pword, "Mr.")){
                                //if update is necessary go back again -> size plus 1 for space
                                fseek(fp, -(1+strlen(pword)), SEEK_CUR);

                                //printf("fdu: %d\n", ftell(fp));
                                fputs("Ms.",fp);
                                fseek(fp, 1, SEEK_CUR);
                                //printf("fdu: %d\n", ftell(fp));

                            }

                            //reset pword
                            pword[0] = 0;
                            pindex = 0;

                            //surname: go forward (as the size of the name plus 1 -> 1 whitespace) check prefix and fix if necessary 

                            fseek(fp, (1 + strlen(word)), SEEK_CUR);
                            //printf(" fdb: %d\n", ftell(fp));
                            //printf(" tell: %d\n", ftell(fp));

                            int nindex = 0;
                            char nc;
                            char nword[100];
                    

                            do
                            {
                                nc = (char)fgetc(fp);

                                nword[nindex++] = nc;
                                nword[nindex] = 0;

                            } while (!(nc == ' ' || nc == '\n' || nc == '\0' || nc == '\t' || pc == '\r'));

                            nword[--nindex] = 0;

                            //Debug Prints (surname)
                            //printf("surname: %s", nword);
                            //printf(" fd: %d\n", ftell(fp));

                            if(strcmp(nword, res->surname)){
                                //if update is necessary go back again -> size plus 1 for space
                                fseek(fp, -(1+strlen(nword)), SEEK_CUR);

                                //printf("fdu: %d\n", ftell(fp));
                                fputs(res->surname,fp);
                                fseek(fp, 1, SEEK_CUR);
                                //printf("fdu: %d\n", ftell(fp));
                            }

                            //reset nword
                            nword[0] = 0;
                            nindex = 0;
                        }

                        word[0] = 0;
                        index = 0;
                    }

                    //if word is not complete add the current char c to word char array
                    else
                    {
                        word[index++] = c;
                        word[index] = 0;
                    }
                    // characterfunction(c);
                } while (c != EOF);

                fclose(fp);
            }
        }

        //if a directory is found (only the directories inside)
        if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
        {
            char path[100] = {0};
            strcat(path, dirName);
            strcat(path, "/");
            strcat(path, ent->d_name);
            //call correction recursively to the path but root is 0 now to include inner database.txt files
            correction(path, 0);
        }
        ent = readdir(dir);
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{

    //create the database linked-list
    create_DB();

    //print the database linked-list (debug purposes)
    //print_DB();

    //do the corrections
    correction(".", 1);

    return 0;
}
