#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_DIRECTORIES 10
#define MAX_CORRUPTED_FILES 100

void analyze_file(const char *filename, const char *izolated_space_dir, int pipe_fd) {
    char command[1024];
    sprintf(command, "/home/kolah/Desktop/SOproject/ProiectSo/verify_for_malicious.sh %s %s", filename, izolated_space_dir);
    int status = system(command);
    if (status != 0) {
        fprintf(stderr, "Eroare la verificarea fișierului %s\n", filename);
        
        write(pipe_fd, filename, strlen(filename) + 1); 
    }
    else {
        
        char safe_msg[] = "SAFE\n";
        write(pipe_fd, safe_msg, strlen(safe_msg) + 1); 
    }
}

void create_snapshot(const char *name, const char *output_dir, const struct stat *info) {
    char snapshot_info[1024];
    sprintf(snapshot_info, "Nume: %s\n", name);

    if (S_ISDIR(info->st_mode)) {
        sprintf(snapshot_info + strlen(snapshot_info), "Tip: Director\n");
    } else if (S_ISREG(info->st_mode)) {
        sprintf(snapshot_info + strlen(snapshot_info), "Tip: Fișier\n");
        sprintf(snapshot_info + strlen(snapshot_info), "Data ultimei modificări: %s\n\n", ctime(&info->st_mtime));
    }

    char snapshot_file[1024];
    sprintf(snapshot_file, "%s/%s_snapshot.txt", output_dir, name);
    int snapshot_fd = open(snapshot_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (snapshot_fd == -1) {
        perror("Eroare la crearea fișierului de snapshot!");
        return;
    }
    write(snapshot_fd, snapshot_info, strlen(snapshot_info));
    close(snapshot_fd);
}

void update_snapshot(const char *dirname, const char *output_dir, const char *izolated_space_dir, int *corrupted_files_count, char corrupted_file_names[MAX_CORRUPTED_FILES][256], int pipe_fd) {
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    pid_t child_pid;

    dir = opendir(dirname);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        sprintf(path, "%s/%s", dirname, entry->d_name);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (stat(path, &info) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(info.st_mode)) {
            
            child_pid = fork();
            if (child_pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (child_pid == 0) {
                
                update_snapshot(path, output_dir, izolated_space_dir, corrupted_files_count, corrupted_file_names, pipe_fd);
                exit(EXIT_SUCCESS);
            } else {
               
                wait(NULL);
            }
            
            create_snapshot(entry->d_name, output_dir, &info);
        } else if (S_ISREG(info.st_mode)) {
            
            analyze_file(path, izolated_space_dir, pipe_fd);
            
            if (access(path, F_OK) == 0) {
                create_snapshot(entry->d_name, output_dir, &info);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 5 || argc > MAX_DIRECTORIES + 4) {
        printf("Utilizare: %s -o director_iesire -s izolated_space_dir dir1 [dir2 ... dirN]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") != 0 || strcmp(argv[3], "-s") != 0) {
        printf("Opțiunile pentru directorul de ieșire și directorul izolat lipsesc sau sunt incorecte!\n");
        return 1;
    }

    const char *output_dir = argv[2];
    const char *izolated_space_dir = argv[4];

    int parent_to_child_pipe[2];
    if (pipe(parent_to_child_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 5; i < argc; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            
            close(parent_to_child_pipe[0]); 

            update_snapshot(argv[i], output_dir, izolated_space_dir, NULL, NULL, parent_to_child_pipe[1]);
            exit(EXIT_SUCCESS);
        }
    }

    close(parent_to_child_pipe[1]); 

    
    int corrupted_files_count = 0;
    char corrupted_file_names[MAX_CORRUPTED_FILES][256]; 
    char buffer[256];
    while (read(parent_to_child_pipe[0], buffer, sizeof(buffer)) > 0) {
        if (strcmp(buffer, "SAFE") != 0) {
            
            strcpy(corrupted_file_names[corrupted_files_count], buffer);
            corrupted_files_count++;
        }
    }

    close(parent_to_child_pipe[0]); 

    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0) {
        printf("Procesul copil cu PID-ul %d s-a încheiat cu codul de ieșire %d", wpid, WEXITSTATUS(status));
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            printf(" și cu %d fișiere corupte.\n", corrupted_files_count);
            for (int i = 0; i < corrupted_files_count; i++) {
                printf("%s ", corrupted_file_names[i]);
            }
            printf("\n");
        } else {
            printf(" și fără fișiere corupte.\n");
        }
    }

    return 0;
}
