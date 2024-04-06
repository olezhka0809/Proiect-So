#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Introduceti denumirea unui director \n");
        return 1;
    }
    DIR *folder;
    struct dirent *entry;
    struct stat filestat;
    int count = 1;
    off_t total = 0;

    folder = opendir(argv[1]);
    if(folder == NULL)
    {
        perror("Unable to read directory");
        return(1);
    }

    printf("Directory of %s\n\n",argv[1]);

    while( (entry=readdir(folder)) )
    {
        char filepath[512];
        sprintf(filepath ," %s/%s ", argv[1], entry->d_name);

        if(strcmp(entry->d_name, "." )==0 || strcmp(entry->d_name , "..") == 0)
            continue;
        stat(filepath ,&filestat);
        //     perror("Unable to get file status");
        //     return 1;
        // }

        printf("%-16s",entry->d_name);

        if( S_ISDIR(filestat.st_mode) )
            printf("%-8s  ","<DIR>");
        else
        {
            printf("%ld  ",filestat.st_size);
            total+=filestat.st_size;
        }

        printf("%s",ctime(&filestat.st_mtime));
        count++;
    }

    closedir(folder);
    printf("\n%d File(s) for %ld bytes\n", count, total);

    return(0);
}