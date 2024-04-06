#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

int main()
{
    DIR *folder;
    struct dirent *entry;
    struct stat filestat;
    char cwdpath[256];
    int count = 1;
    long long total = 0;

    folder = opendir(".");
    if(folder == NULL)
    {
        perror("Unable to read directory");
        return(1);
    }

    getcwd(cwdpath,256);
    printf(" Directory of %s\n\n",cwdpath);

    while( (entry=readdir(folder)) )
    {
        stat(entry->d_name,&filestat);
        printf("%-16s",entry->d_name);

        if( S_ISDIR(filestat.st_mode) )
            printf("%-8s  ","<DIR>");
        else
        {
            printf("%8lld  ",filestat.st_size);
            total+=filestat.st_size;
        }

        printf("%s",ctime(&filestat.st_mtime));
    }

    closedir(folder);
    printf("\n        %d File(s) for %lld bytes\n",
            count,
            total
          );

    return(0);
}