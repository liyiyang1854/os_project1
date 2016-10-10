#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdio.h>  
#include <string.h>
#include <stdlib.h>



class process{
public:
	process(char* id, int itime, int burst, int num, int io);
	char* getid();
	int getitiem();
	int getbursttime();
	int getnumburst();
	int getiotime();
	
	char* proc_id;
	int initial_time, burst_time, num_bursts, io_time;
};

process::process(char* id, int itime, int burst, int num, int io){
	proc_id = id;
	initial_time = itime;
	burst_time = burst;
	num_bursts = num;
	io_time = io;
}

char* process::getid(){
    return proc_id;
}
int process::getitiem(){
    return initial_time;
}

int process::getbursttime(){
    return burst_time;
}

int process::getnumburst(){
    return num_bursts;
}

int process::getiotime(){
    return io_time;
}
	
int main(int argc, char* argv[])
{
	if(argc != 2)
    {
        fprintf(stderr, "ERROR: Invalid arguments\nUSAGE: ./a.out <input-file>\n");
        return EXIT_FAILURE;
    }

    FILE* input_file;
    input_file = fopen(argv[1], "r");
    
    
    while (!feof(input_file))
    {
    	char * in_line;
    	size_t len = 0;
    	getline(&in_line, &len, input_file);
    	
    	char* array[5];
    	
    	if(!isalpha(in_line[0])){
    		continue;
    	}
    	else{
    	    int i = 0;
    	    printf("%s", in_line);
    	    char* tmp = NULL;
    	    tmp = strtok(in_line,"|");
    	    while(tmp!= NULL){
    	        array[i] = tmp;
    	        i++;
    	        tmp = strtok(NULL,"|");
    	    }
    	}
    	
    	int itime = atoi(array[1]);
    	int burst = atoi(array[2]); 
    	int num = atoi(array[3]);
    	int io = atoi(array[4]);
    	
    	process aprocess(array[0], itime, burst, num, io);
    	
    }
}