// file_io for opening, closing, and writing to files for snapshot project

#include "file_io.h"

FILE * open_file(int process_id){
    char filename[30];
    sprintf(filename, "records/%d.id", process_id);
    FILE * file = fopen(filename, "w");
    if(file == NULL) printf("Could not open file for process %d\n", process_id);
    return file;
}

void close_file(FILE * file){
    if(file != NULL){
        fclose(file);
    }
}

void record_helper(FILE * file, int process_id, int snapshot_id, int logical, int * vector, int num_processes){
    int i;
    fprintf(file, "%d> snapshot: %2d | logical: %2d | vector: ", process_id, snapshot_id, logical);
    for(i=0; i<num_processes; i++){
        fprintf(file, "%2d ", vector[i]);
    }
    fprintf(file, " | ");
}

int record_channel(FILE * file, int process_id, int snapshot_id, int logical, int * vector, char ** channel, int num_processes){
    if(file != NULL){
        int i;
        for(i=0; i<num_processes; i++){
            if(channel[i][0] != '\0'){
                int from = channel[i][0] - 1;
                int w    = channel[i][2] - 1;
                int m    = channel[i][3] - 1;
                record_helper(file, process_id, snapshot_id, logical, vector, num_processes);
                fprintf(file, "message %d to %d | money: %4d | widgets: %4d\n", from, process_id, m, w);
            }
        }
        return 1;
    }
    else{
        printf("Could not record channel for process %d\n", process_id);
        return -1;
    }
}

int record_state(FILE * file, int process_id, int snapshot_id, int logical, int * vector, int money, int widgets, char ** channel, int num_processes){
    if(file != NULL){
        record_helper(file, process_id, snapshot_id, logical, vector, num_processes);
        fprintf(file, "     state     | money: %4d | widgets: %4d\n", money, widgets);
        record_channel(file, process_id, snapshot_id, logical, vector, channel, num_processes);
        fflush(file);
        return 1;
    }
    else{
        printf("Could not record state for process %d\n", process_id);
        return -1;
    }   
}


