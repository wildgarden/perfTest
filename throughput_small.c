#include "utils.h"                                                              

#define SHSZ                        (sizeof(double) * (65))
                                                                                
int main(int argc, char **argv) {                                               
    if (argc == 1) {                                                            
        printf("./through_put <mnt_dir> <number of processes> \n");          
        return 1;                                                               
    }                                                                           
                                                                                
    int num_procs = atoi(argv[2]);

    int shmid;
    key_t key;
    double *shm;

    char *path = argv[1];                                              
                                                                                
    int f = 1;                                                                  
    char *filename = "file";                                                  

    int i = 1;  
 
    
    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (double *) -1) {
        perror("shmat");
        exit(1);
    }

    for (i = 0; i <= num_procs; i++) {
        shm[i] = 0;
    }

    printf("Starting reads on all files\n");
                                                                                
    for (i = 1; i <= num_procs; i++, f++) {                                 
        // Read a new file.                                                     
        char str[10];                                                           
        sprintf(str, "%d", f);                                                  
                                                                                
        char my_file[100] = {'\0'};
        strcat(my_file, path);
        strcat(my_file, "/");                                          
        strcat(my_file, filename);                                              
        strcat(my_file, str);

        // Prepare arguments                                                    
        char *args[4] = {"Command-line", my_file, str, NULL};                 
                                                                                
        // Start off new process                                                
        pid_t pid = fork();                                                     
        if (pid == 0) {                                                         
            execvp("./read_client", args);                                     
        }                                                                       
    }

    printf("Waiting till all child processes complete..\n");

    int status = 0;
    for (i = 1; i <= num_procs; i++) {
        wait(&status);
        status = 0;
    }

    double sum = 0;
    for (i = 0; i <= num_procs; i++) {
        sum += shm[i];
    }

    printf("Average time is %lf\n", sum/num_procs);

    return 0;
} 
