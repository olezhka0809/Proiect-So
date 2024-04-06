#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>


void listare_director(const char * dirname ){
    DIR *dir;
    struct dirent*entry;
    struct stat info;
    dir = opendir(dirname);
    if(!dir){
        perror("opendir");
        return;
    }
    

    while ((entry = readdir(dir))!=NULL)
    {
        char path[512];
        sprintf(path , "%s/%s" , dirname, entry->d_name);
        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0){
            continue;
        }
        printf("Nume : %s\n", entry->d_name);
        if(stat(path, &info)==-1){
            perror("stat");
            continue;
        }
        if(S_ISDIR(info.st_mode)){
            char output_Snap[512];
            printf("Tip : Director \n");
            printf("Data ultimei modificari: %s\n", ctime(&info.st_mtime));
            printf("\n");

            sprintf(output_Snap, "%s/%s/SnapshotSubDir.txt", dirname,entry->d_name); 
            FILE*file = fopen(output_Snap, "w");
            if(file == NULL){
                printf("Nu s-a putut creea file-ul!");
                closedir(dir);
                return;
            }


            fprintf(file, "Tip : Director %s\n", entry->d_name);
            fprintf(file, "Data ultimei modificari: %s\n\n", ctime(&info.st_mtime));
            fclose(file);
            listare_director(path);
        }else{
            char SnapshotFisiere[512];
            printf("Tip: Fisier\n");
            printf("Dimensiune: %ld bytes\n", info.st_size);
            printf("Data ultimei modificari: %s\n", ctime(&info.st_mtime));
            printf("\n");

            char numeFisier[512];
            sprintf(numeFisier, "%s-SnapshotFisier.txt", entry->d_name);

            sprintf(SnapshotFisiere, "%s/%s", dirname, numeFisier);

            FILE *file2 = fopen(SnapshotFisiere, "w");
            if (file2 == NULL) {
                perror("Eroare creare snapshot fisiere !");
                continue;
            }
            fprintf(file2, "Tip: Fisier\n");
            fprintf(file2, "Dimensiune: %ld bytes\n", info.st_size);
            fprintf(file2, "Data ultimei modificari: %s\n\n", ctime(&info.st_mtime));
            fclose(file2);
        } 
    }
    closedir(dir);
}



int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Introduceti denumirea unui director \n");
        return 1;
    }
    if(argc!= 2){
        fprintf(stderr , "Usage: %s directory \n", argv[0]);

    }
    listare_director(argv[1]);
     return(0);
}

