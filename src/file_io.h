//file_io header file

#include <stdio.h>
#include <stdlib.h>
#include "networking.h"

FILE * open_file(int process_id);
void close_file(FILE * file);
int record_state(FILE * file, int process_id, int snapshot_id, int logical, int * vector, int money, int widgets, char ** channel, int num_processes);


