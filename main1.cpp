#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdio.h>  
#include <string.h>
#include <stdlib.h>

#define T_CS 8

class process{
public:
	process(char* id, int itime, int burst, int num, int io);
	char* getid();
	int getitiem();
	int getbursttime();
	int getnumburst();
	int getiotime();
	int gettmptesk();
	int getburstst();
	int getiot();
	
	void finishonce();
	void burstchange();
	void burst_one();
	void iochange();
	void io_one();
	void changeio(int a);
	
	char* proc_id;
	int initial_time, burst_time, num_bursts, io_time, burstst, iot, tmptesk;
};

process::process(char* id, int itime, int burst, int num, int io){
	proc_id = id;
	initial_time = itime;
	burst_time = burst;
	num_bursts = num;
	io_time = io;
	burstst = burst;
	iot = io;
	tmptesk = num;
}

char* process::getid(){return proc_id;}
int process::getitiem(){return initial_time;}
int process::getbursttime(){return burst_time;}
int process::getnumburst(){return num_bursts;}
int process::getiotime(){return io_time;}
int process::gettmptesk(){return tmptesk;}
int process::getburstst(){return burstst;}
int process::getiot(){return iot;}

void process::finishonce(){tmptesk--;}
void process::burstchange(){burstst = burst_time;}
void process::burst_one(){burstst--;}
void process::iochange(){iot = io_time;}
void process::io_one(){iot--;}
void process::changeio(int a){iot += a;}

bool FCFS_Sort(process a, process b){return a.getitiem()<b.getitiem();}
bool SJF_Sort(process a, process b){return a.getbursttime()<b.getbursttime();}

void print_queue(std::vector<process> queue);
void FCFS(std::vector<process> order_q);
void SJF(std::vector<process> order_q);

int main(int argc, char* argv[])
{
	if(argc != 2)
    {
        fprintf(stderr, "ERROR: Invalid arguments\nUSAGE: ./a.out <input-file>\n");
        return EXIT_FAILURE;
    }

    FILE* input_file;
    input_file = fopen(argv[1], "r");
    
    std::vector<process> order_q;
    
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
    	    //printf("%s", in_line);
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
    	
    	order_q.push_back(aprocess);
    }
    std::sort(order_q.begin(), order_q.end(), FCFS_Sort);
    FCFS(order_q);
    printf("\n");
    std::sort(order_q.begin(), order_q.end(), SJF_Sort);
    SJF(order_q);
}

void print_queue(std::vector<process> queue){
    printf("[Q");
    if(!queue.empty()){
        for (unsigned int i = 0; i < queue.size();i++){
            printf(" %s",queue[i].getid());
        }
    }
    printf("]\n");
}

void FCFS(std::vector<process> order_q){
    std::vector<process> waiting_q, doing_q, io_q, finished, holding;
    waiting_q = order_q;
    int t = 0;
    printf("time %dms: Simulator started for FCFS ", t);
    print_queue(order_q);
    int t_cs =  T_CS;
    bool notempty = false;
    t = -1;
    while(1){
        if(waiting_q.empty() && doing_q.empty() && holding.empty()&&io_q.empty()){
            printf("time %dms: Simulator for FCFS ended [Q]\n", t);
            break;
        }
        
        std::vector<process> io_buffer;
        t = t+1;
        if(!doing_q.empty()){
            doing_q[0].burst_one();
            if(doing_q[0].getburstst()== 0 && doing_q[0].gettmptesk()== 1){
                finished.push_back(doing_q[0]);
                printf("time %dms: %s terminated ", t, doing_q[0].getid());
                print_queue(waiting_q);
                doing_q.clear();
            }
            else if(doing_q[0].getburstst()== 0 && doing_q[0].gettmptesk()!= 1){
                doing_q[0].finishonce();
                printf("time %dms: %s completed its CPU burst ", t, doing_q[0].getid());
                print_queue(waiting_q);
                printf("time %dms: %s performing I/O ", t, doing_q[0].getid());
                print_queue(waiting_q);
                doing_q[0].burstchange();
                doing_q[0].changeio(1);
                io_q.push_back(doing_q[0]);
                doing_q.clear();
            }
        }
        if(!holding.empty()){
            t_cs--;
            if (t_cs == 0){
                doing_q.push_back(holding[0]);
                printf("time %dms: %s started using the CPU ", t, holding[0].getid());
                print_queue(waiting_q);
                holding.clear();
                t_cs = T_CS;
                notempty = false;
            }
        }
        if(!io_q.empty()){
            for (unsigned int i = 0; i< io_q.size(); i++){
                io_q[i].io_one();
                if(io_q[i].getiot() == 0){
                    io_q[i].iochange();
                    if(doing_q.empty() && notempty == false){
                        notempty = true;
                        bool nopush = false;
                        for(unsigned int w = 0; w < waiting_q.size(); w++){
                            if(waiting_q[w].getid() == io_q[i].getid()){
                                nopush = true;
                                waiting_q[w] = io_q[i];
                            }
                        }
                        if(nopush == false){
                            waiting_q.push_back(io_q[i]);
                        }
                        printf("time %dms: %s completed I/O ", t, io_q[i].getid());
                        print_queue(waiting_q);
                    }
                    else{
                        bool nopush = false;
                        for(unsigned int w = 0; w < waiting_q.size(); w++){
                            if(waiting_q[w].getid() == io_q[i].getid()){
                                nopush = true;
                                waiting_q[w] = io_q[i];
                            }
                        }
                        if(nopush == false){
                            waiting_q.push_back(io_q[i]);
                        }
                        printf("time %dms: %s completed I/O ", t, io_q[i].getid());
                        print_queue(waiting_q);
                    }
                    io_buffer.push_back(io_q[i]);
                }
            }
        }
        
        if(!io_buffer.empty()){
            for(unsigned int i = 0; i < io_buffer.size(); i++){
                for(unsigned int w = 0; w < io_q.size(); w++){
                    if(io_buffer[i].getid() == io_q[w].getid()){io_q.erase(io_q.begin()+w);}
                }
            
            }
        }
        if(doing_q.empty()&&holding.empty()&&!waiting_q.empty()){
            holding.push_back(waiting_q[0]);
            notempty = true;
            waiting_q.erase(waiting_q.begin());
            //t_cs--;
        }
    }
    
}

void SJF(std::vector<process> order_q){
    std::vector<process> waiting_q, doing_q, io_q, finished, holding;
    waiting_q = order_q;
    int t = 0;
    printf("time %dms: Simulator started for SJF ", t);
    print_queue(order_q);
    int t_cs =  T_CS;
    bool notempty = false;
    t = -1;
    while(1){
        if(waiting_q.empty() && doing_q.empty() && holding.empty()&&io_q.empty()){
            printf("time %dms: Simulator for SJF ended [Q]\n", t);
            break;
        }
        
        std::vector<process> io_buffer;
        t = t+1;
        if(!doing_q.empty()){
            doing_q[0].burst_one();
            if(doing_q[0].getburstst()== 0 && doing_q[0].gettmptesk()== 1){
                finished.push_back(doing_q[0]);
                printf("time %dms: %s terminated ", t, doing_q[0].getid());
                print_queue(waiting_q);
                doing_q.clear();
            }
            else if(doing_q[0].getburstst()== 0 && doing_q[0].gettmptesk()!= 1){
                doing_q[0].finishonce();
                printf("time %dms: %s completed its CPU burst ", t, doing_q[0].getid());
                print_queue(waiting_q);
                printf("time %dms: %s performing I/O ", t, doing_q[0].getid());
                print_queue(waiting_q);
                doing_q[0].burstchange();
                doing_q[0].changeio(1);
                io_q.push_back(doing_q[0]);
                doing_q.clear();
            }
        }
        if(!holding.empty()){
            t_cs--;
            if (t_cs == 0){
                doing_q.push_back(holding[0]);
                printf("time %dms: %s started using the CPU ", t, holding[0].getid());
                print_queue(waiting_q);
                holding.clear();
                t_cs = T_CS;
                notempty = false;
            }
        }
        if(!io_q.empty()){
            for (unsigned int i = 0; i< io_q.size(); i++){
                io_q[i].io_one();
                if(io_q[i].getiot() == 0){
                    io_q[i].iochange();
                    if(doing_q.empty() && notempty == false){
                        notempty = true;
                        bool nopush = false;
                        for(unsigned int w = 0; w < waiting_q.size(); w++){
                            if(waiting_q[w].getid() == io_q[i].getid()){
                                nopush = true;
                                waiting_q[w] = io_q[i];
                            }
                        }
                        if(nopush == false){
                            waiting_q.push_back(io_q[i]);
                        }
                        printf("time %dms: %s completed I/O ", t, io_q[i].getid());
                        print_queue(waiting_q);
                    }
                    else{
                        bool nopush = false;
                        for(unsigned int w = 0; w < waiting_q.size(); w++){
                            if(waiting_q[w].getid() == io_q[i].getid()){
                                nopush = true;
                                waiting_q[w] = io_q[i];
                            }
                        }
                        if(nopush == false){
                            waiting_q.push_back(io_q[i]);
                        }
                        printf("time %dms: %s completed I/O ", t, io_q[i].getid());
                        print_queue(waiting_q);
                    }
                    io_buffer.push_back(io_q[i]);
                }
            }
        }
        
        if(!io_buffer.empty()){
            for(unsigned int i = 0; i < io_buffer.size(); i++){
                for(unsigned int w = 0; w < io_q.size(); w++){
                    if(io_buffer[i].getid() == io_q[w].getid()){io_q.erase(io_q.begin()+w);}
                }
            
            }
        }
        if(doing_q.empty()&&holding.empty()&&!waiting_q.empty()){
            holding.push_back(waiting_q[0]);
            notempty = true;
            waiting_q.erase(waiting_q.begin());
            //t_cs--;
        }
    }
}