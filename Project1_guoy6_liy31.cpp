//==================================================================================
// This project is done by group: Yiqing Guo(guoy6), Yiyang Li(liy31)
//==================================================================================
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
#define T_S 84

//==================================================================================
class process{ //create a class for each process
public:
    //each process come with its ID, initial time, burst time, num of bursts and I/O time.
    process(char id, int itime, int btime, int num, int io);

    //these functions are used for the get the information of the process or the queue.
    char getid(){return proc_id;}
    int getinitialtime(){return initial_time;}
    int getbursttime(){return burst_time;}
    int getnumburst(){return num_bursts;}
    int getiotime(){return io_time;}
    
    int gettmptask(){return tmp_task;}
    int gettmpbursttime(){return tmp_burst_time;}
    int getiot(){return iot;}
    int getwaittime(){return wait_time;}
    

    void finishonce(){tmp_task--;}
    void burstchange(){tmp_burst_time = burst_time;}
    void burst_one(){tmp_burst_time--;}
    void iochange(){iot = io_time;}
    void io_one(){iot--;}
    void changeio(int a){iot += a;}
    void addwait_time(){wait_time++;}
    void add_newturn(int a){turnaround.push_back(a);}
    void add_to_last(int n){turnaround[turnaround.size()-1]+=n;}
    int total_tar(){
        int total = 0;
        for(unsigned int i = 0; i< turnaround.size(); i++) {total += turnaround[i];}
        return total;
    }
    
    char proc_id;
    int initial_time, burst_time, num_bursts, io_time, tmp_burst_time, iot, tmp_task, wait_time;
    std::vector<int> turnaround;
};

//----------------------------------------------------------------------------------
//Initilize the process
process::process(char id, int itime, int btime, int num, int io){
    
    proc_id = id; //ID
    initial_time = itime; //Initial time
    burst_time = btime; //CPU burst time
    num_bursts = num; //num of bursts
    io_time = io; // I/O time
    
    tmp_burst_time = btime; //have temp value for calculation
    iot = io; //have temp io for calculation
    tmp_task = num;
    wait_time = 0; //wait time
}


//==================================================================================
//Functions
void print_queue(std::vector<process> queue);

void FCFS(std::vector<process> order_q, FILE * output_file);
void SJF(std::vector<process> order_q, FILE * output_file);
void RR(std::vector<process> order_q, FILE * output_file, int t_slice);

bool FCFS_Sort(process a, process b){return a.getinitialtime()<b.getinitialtime();}
bool SJF_Sort(process a, process b){return a.getbursttime()<b.getbursttime();}


//==================================================================================
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "ERROR: Invalid arguments\nUSAGE: ./a.out <input-file> <stats-output-file>\n");
        return EXIT_FAILURE;
    }

    FILE* input_file;
    input_file = fopen(argv[1], "r");
    if (input_file == NULL){
        fprintf(stderr, "ERROR: Cannot open input file %s\n", argv[1]);
    }
    FILE * output_file;
    output_file = fopen(argv[2], "wb");
    if(output_file == NULL){
        fprintf(stderr, "ERROR: Cannot open output file %s\n", argv[2]);
    }
    
    std::vector<process> order_q;
    
    //get the processes from the input file
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
            char* tmp = NULL;
            tmp = strtok(in_line,"|");
            while(tmp!= NULL){
                array[i] = tmp;
                i++;
                tmp = strtok(NULL,"|");
            }
        }
        char name = *array[0];
        int itime = atoi(array[1]);
        int btime = atoi(array[2]); 
        int num = atoi(array[3]);
        int io = atoi(array[4]);
        
        process aprocess(name, itime, btime, num, io);
        
        order_q.push_back(aprocess);
    }
    
    //perform FCFS, SJF, RR
    std::sort(order_q.begin(), order_q.end(), FCFS_Sort);
    FCFS(order_q, output_file);
    printf("\n");

    SJF(order_q, output_file);
    printf("\n");

    RR(order_q, output_file,T_S);

}


//==================================================================================
//print the processes from the queue
void print_queue(std::vector<process> queue){
    printf("[Q");
    if(queue.empty()){
        printf(" empty");
    }
    else{
        for (unsigned int i = 0; i < queue.size();i++){
            printf(" %c", queue[i].getid());
        }
    }
    printf("]\n");
}


//==================================================================================
void RR(std::vector<process> order_q, FILE * output_file, int t_slice) {
    
    //Initilize these vectors
    //waiting_q: ready queue
    //doing_q: CPU
    //io_q: for I/O operation, not really a queue
    //finished: processes that terminated
    //holding_q: for context switch
    //waiting_for_start: store processes from the input file
    std::vector<process> waiting_q, doing_q, io_q, finished, holding_q, waiting_for_start;
    int t = 0; //initial time
    waiting_for_start = order_q; //get the processes from input file
    printf("time %dms: Simulator started for RR ", t);
    print_queue(waiting_q); // print the processes in the queue
    
    int t_cs = T_CS/2; // context switch time
    bool notempty = false; 
    bool isend = false;
    int context_s = 0;
    int preemption = 0;

    int t_slice_count = 0;

    while (1) {
        bool this_end = false; // decide the time interval is 4ms or 8ms

        //--------------------
        //check if a process would arrive
        for(unsigned int i = 0; i < waiting_for_start.size(); i++){
            //check arrival time
            if(waiting_for_start[i].getinitialtime() == t){
                waiting_for_start[i].add_newturn(1); // for avg time and total numbers
                waiting_q.push_back(waiting_for_start[i]); //add process to read queue

                printf("time %dms: Process %c arrived ", t, waiting_for_start[i].getid()); 
                print_queue(waiting_q); // print the processes in the queue
                
                waiting_for_start.erase(waiting_for_start.begin()+i); //remove the processes
                i--; // adjust the index
            }
        }

        //--------------------
        //check if it is last context switch
        if( waiting_q.empty() && doing_q.empty() && holding_q.empty() && io_q.empty() ) {
            if( isend == true ) { // check if the process ended, set the context switch time
                t_cs = T_CS/2;
                t_cs--;
                isend = false;
            }
            else {
                t_cs--; // perform last context switch
                if( t_cs == 0 ) { // all ends
                    printf("time %dms: Simulator ended for RR\n", t);
                    break; //get out of the while(1)
                }
            }
        }

        std::vector<process> io_buffer; //store the process that finished I/O
        
        //--------------------    
        // check if in the context switch
        if ( !holding_q.empty() ) { 

            t_cs--; // decrease context switch time

            if ( t_cs == 0 ) { //finish context switch
                doing_q.push_back(holding_q[0]); //add to CPU
                
                printf("time %dms: Process %c started using the CPU ", t, holding_q[0].getid());
                print_queue(waiting_q);

                holding_q.clear(); //will just have one process in holding queue 
                notempty = false; // holding queue is empty

            }
            else {
                holding_q[0].add_to_last(1); 
            }
        }

        //--------------------
        //check if current process reaches time limit
        if (t_slice_count == t_slice && doing_q[0].gettmpbursttime() != 0) { 
            t_slice_count = 0;
            
            // check if have process in the ready queue
            if ( !waiting_q.empty() ) {
                
                printf("time %dms: Time slice expired; process %c preempted with %dms to go ",t,doing_q[0].getid(),doing_q[0].gettmpbursttime() );
                doing_q[0].add_to_last(1);
                waiting_q.push_back(doing_q[0]); //add the current process to waiting_q    
                print_queue(waiting_q);
                doing_q.clear(); // preemption
                preemption ++;
                context_s++;
                holding_q.push_back(waiting_q[0]);
                waiting_q.erase( waiting_q.begin() );
                t_cs = T_CS;
                
            }
            else {

                printf("time %dms: Time slice expired; no preemption because ready queue is empty [Q empty]\n",t);
            }
        }

        //--------------------
        //check if CPU has process running
        if ( !doing_q.empty() ) {
            //check if the current process in the CPU is about to finish all bursts
            if ( doing_q[0].gettmpbursttime() == 0 && doing_q[0].gettmptask() == 1 ) {
                
                finished.push_back(doing_q[0]);
                t_slice_count = 0;
                
                printf("time %dms: Process %c terminated ", t, doing_q[0].getid());
                print_queue(waiting_q);
                
                doing_q.clear();
                isend = true;
                this_end = true;
            }
            //check if the current process in the CPU is about to finish one of bursts, not the last one
            else if ( doing_q[0].gettmpbursttime() == 0 && doing_q[0].gettmptask() != 1 ) {
                t_slice_count = 0;

                doing_q[0].finishonce();
                printf("time %dms: Process %c completed a CPU burst; %d to go ", t, doing_q[0].getid(), doing_q[0].gettmptask());
                print_queue(waiting_q);
                printf("time %dms: Process %c blocked on I/O until time %dms ", t, doing_q[0].getid(), t+doing_q[0].getiot());
                print_queue(waiting_q);
                
                doing_q[0].burstchange();
                doing_q[0].changeio(1);
                io_q.push_back(doing_q[0]);
                doing_q.clear();
                isend = true;
                this_end = true;
            }
            // check if the current process is running
            else{
            	t_slice_count ++; // count how much current process has spent in the CPU
            	doing_q[0].burst_one();
                doing_q[0].add_to_last(1);
            }
        }

        //--------------------
        //check if have processes in the io queue
        if( !io_q.empty() ) {
            for ( unsigned int i = 0; i< io_q.size(); i++ ) {
                io_q[i].io_one();
                
                if( io_q[i].getiot() == 0 ) {
                    io_q[i].iochange();

                    //check if doing_q is empty and notempty
                    if(doing_q.empty() && notempty == false){
                        
                        notempty = true;
                        if(this_end == false){isend = false;}
                        bool nopush = false;

                        for(unsigned int w = 0; w < waiting_q.size(); w++){
                            if(waiting_q[w].getid() == io_q[i].getid()){
                                nopush = true;
                                waiting_q[w] = io_q[i];
                            }
                        }
                        
                        if(nopush == false){
                            io_q[i].add_newturn(1); // for avg time and total numbers

                            waiting_q.push_back(io_q[i]);
                        }
                        
                        printf("time %dms: Process %c completed I/O ", t, io_q[i].getid());
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
                            io_q[i].add_newturn(1);
                            waiting_q.push_back(io_q[i]);
                        }
                        
                        printf("time %dms: Process %c completed I/O ", t, io_q[i].getid());
                        print_queue(waiting_q);
                    }
                    io_buffer.push_back(io_q[i]);
                }
            }
        }

        //--------------------
        // check if any process is doing I/O
        if(!io_buffer.empty()){
            for(unsigned int i = 0; i < io_buffer.size(); i++){
                for(unsigned int w = 0; w < io_q.size(); w++){
                    if(io_buffer[i].getid() == io_q[w].getid()) {io_q.erase(io_q.begin()+w);}
                }
            }
        }

        //--------------------
        //previous process just finished and next process can get into the queue
        if( doing_q.empty() && holding_q.empty() && !waiting_q.empty() ) {

            holding_q.push_back(waiting_q[0]);
            notempty = true;
            waiting_q.erase(waiting_q.begin());
            
            //set context switch time
            if(isend == true){
                t_cs = T_CS;
                isend = false;
            }
            else{t_cs = T_CS/2;}
            
            context_s++;
        }

        //--------------------
        //check if we have processes in the waiting_q which is ready queue 
        if(!waiting_q.empty()){
            for(unsigned int i = 0; i < waiting_q.size(); i++){
                waiting_q[i].add_to_last(1);
                waiting_q[i].addwait_time();
            }
        }

        //increase time
        t++;

    } //while(1) ends
        
    //final calculation and output
    int total_tar_t, total_burst, total_wait, total_task;
    float avg_tar_t, avg_burst, avg_wait;
    total_tar_t = total_burst = total_wait = avg_tar_t = avg_burst = total_task = avg_wait = 0;
    
    for(unsigned int i = 0; i < finished.size(); i++){
        total_task += finished[i].getnumburst();
        total_wait += finished[i].getwaittime();
        total_burst = total_burst + (finished[i].getnumburst() * finished[i].getbursttime());
        total_tar_t += finished[i].total_tar();
    }
    
    avg_burst = float(total_burst)/float(total_task);   
    avg_wait = float(total_wait)/float(total_task);
    avg_tar_t = float(total_tar_t)/float(total_task);
    fprintf(output_file, "Algorithm RR\n");
    fprintf(output_file, "-- average CPU burst time: %.2f ms\n", avg_burst);
    fprintf(output_file, "-- average wait time: %.2f ms\n", avg_wait);
    fprintf(output_file, "-- average turnaround time: %.2f ms\n", avg_tar_t );
    fprintf(output_file, "-- total number of context switches: %d\n", context_s);
    fprintf(output_file, "-- total number of preemptions: %d\n", preemption);
}


//==================================================================================
void FCFS(std::vector<process> order_q, FILE * output_file){

    //Initilize these vectors
    //waiting_q: ready queue
    //doing_q: CPU
    //io_q: for I/O operation, not really a queue
    //finished: processes that terminated
    //holding_q: for context switch
    //waiting_for_start: store processes from the input file
    std::vector<process> waiting_q, doing_q, io_q, finished, holding, waiting_for_start;
    
    int t = 0;
    waiting_for_start = order_q;
    printf("time %dms: Simulator started for FCFS ", t);
    print_queue(waiting_q);
    int t_cs =  T_CS/2;
    bool notempty = false;
    bool isend = false;
    int context_s = 0;
    int preemption = 0;

    while(1){

        bool this_end = false;

        //--------------------
        //check if a process would arrive
        for(unsigned int i = 0; i < waiting_for_start.size(); i++){
            if(waiting_for_start[i].getinitialtime() == t){
                waiting_for_start[i].add_newturn(1);
                waiting_q.push_back(waiting_for_start[i]);
                printf("time %dms: Process %c arrived ", t, waiting_for_start[i].getid());
                print_queue(waiting_q);
                waiting_for_start.erase(waiting_for_start.begin()+i);
                i--;
            }
        }

        //--------------------
        //check if it is last context switch
        if(waiting_q.empty() && doing_q.empty() && holding.empty()&&io_q.empty()){
            if(isend == true){
                t_cs = T_CS/2;
                t_cs--;
                isend = false;
            }
            else{
                t_cs--;
                if(t_cs == 0){
                    printf("time %dms: Simulator ended for FCFS\n", t);
                    break;
                }
            }
        }

        std::vector<process> io_buffer;

        //--------------------
        //check if CPU has process running
        if(!doing_q.empty()){
            
            doing_q[0].burst_one();
            doing_q[0].add_to_last(1);

            //check if the current process in the CPU is about to finish all bursts
            if(doing_q[0].gettmpbursttime()== 0 && doing_q[0].gettmptask()== 1){
                
                finished.push_back(doing_q[0]);
                printf("time %dms: Process %c terminated ", t, doing_q[0].getid());
                print_queue(waiting_q);
                
                doing_q.clear();
                isend = true;
                this_end = true;
            }
            //check if the current process in the CPU is about to finish one of bursts, not the last one
            else if(doing_q[0].gettmpbursttime()== 0 && doing_q[0].gettmptask()!= 1){
                
                doing_q[0].finishonce();
                printf("time %dms: Process %c completed a CPU burst; %d to go ", t, doing_q[0].getid(), doing_q[0].gettmptask());
                print_queue(waiting_q);
                printf("time %dms: Process %c blocked on I/O until time %dms ", t, doing_q[0].getid(), t+doing_q[0].getiot());
                print_queue(waiting_q);
                
                doing_q[0].burstchange();
                doing_q[0].changeio(1);
                io_q.push_back(doing_q[0]);
                doing_q.clear();
                isend = true;
                this_end = true;
            }
        }

        //--------------------
        // check if in the context switch
        if(!holding.empty()){
            
            //context switch
            t_cs--;

            if (t_cs == 0){
                doing_q.push_back(holding[0]);
                printf("time %dms: Process %c started using the CPU ", t, holding[0].getid());
                print_queue(waiting_q);
                holding.clear();
                notempty = false;
            }
            else{
                holding[0].add_to_last(1);
            }
        }

        //--------------------
        //check if have processes in the io queue
        if(!io_q.empty()){
            for (unsigned int i = 0; i< io_q.size(); i++){
                io_q[i].io_one();
                if(io_q[i].getiot() == 0){
                    io_q[i].iochange();

                    //check if doing_q is empty and notempty
                    if(doing_q.empty() && notempty == false){
                        notempty = true;
                        if(this_end == false){isend = false;}
                        bool nopush = false;
                        for(unsigned int w = 0; w < waiting_q.size(); w++){
                            if(waiting_q[w].getid() == io_q[i].getid()){
                                nopush = true;
                                waiting_q[w] = io_q[i];
                            }
                        }
                        if(nopush == false){
                            io_q[i].add_newturn(1);
                            waiting_q.push_back(io_q[i]);
                        }
                        printf("time %dms: Process %c completed I/O ", t, io_q[i].getid());
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
                            io_q[i].add_newturn(1);
                            waiting_q.push_back(io_q[i]);
                        }
                        printf("time %dms: Process %c completed I/O ", t, io_q[i].getid());
                        print_queue(waiting_q);
                    }
                    io_buffer.push_back(io_q[i]);
                }
            }
        }

        //--------------------
        // check if any process is doing I/O
        if(!io_buffer.empty()){
            for(unsigned int i = 0; i < io_buffer.size(); i++){
                for(unsigned int w = 0; w < io_q.size(); w++){
                    if(io_buffer[i].getid() == io_q[w].getid()){io_q.erase(io_q.begin()+w);}}
            }
        }

        //--------------------
        //previous process just finished and next process can get into the queue
        if(doing_q.empty()&&holding.empty()&&!waiting_q.empty()){
            holding.push_back(waiting_q[0]);
            notempty = true;
            waiting_q.erase(waiting_q.begin());

            //context switch
            if(isend == true){
                t_cs = T_CS;
                isend = false;
            }
            else{t_cs = T_CS/2;}
            context_s++;
        }

        //--------------------
        //check if we have processes in the waiting_q which is ready queue 
        if(!waiting_q.empty()){
            for(unsigned int i = 0; i < waiting_q.size(); i++){
                waiting_q[i].add_to_last(1);
                waiting_q[i].addwait_time();
            }
        }
        
        //increase time
        t++;

    } //while(1) ends

    //final calculation and output
    int total_tar_t, total_burst, total_wait, total_task;
    float avg_tar_t, avg_burst, avg_wait;
    total_tar_t = total_burst = total_wait = avg_tar_t = avg_burst = total_task = avg_wait = 0;

    for(unsigned int i = 0; i < finished.size(); i++){
        total_task += finished[i].getnumburst();
        total_wait += finished[i].getwaittime();
        total_burst = total_burst + (finished[i].getnumburst() * finished[i].getbursttime());
        total_tar_t += finished[i].total_tar();
    }

    avg_burst = float(total_burst)/float(total_task);   
    avg_wait = float(total_wait)/float(total_task);
    avg_tar_t = float(total_tar_t)/float(total_task);

    fprintf(output_file, "Algorithm FCFS\n");
    fprintf(output_file, "-- average CPU burst time: %.2f ms\n", avg_burst);
    fprintf(output_file, "-- average wait time: %.2f ms\n", avg_wait);
    fprintf(output_file, "-- average turnaround time: %.2f ms\n", avg_tar_t );
    fprintf(output_file, "-- total number of context switches: %d\n", context_s);
    fprintf(output_file, "-- total number of preemptions: %d\n", preemption);
}


//==================================================================================
void SJF(std::vector<process> order_q, FILE * output_file){

    //Initilize these vectors
    //waiting_q: ready queue
    //doing_q: CPU
    //io_q: for I/O operation, not really a queue
    //finished: processes that terminated
    //holding_q: for context switch
    //waiting_for_start: store processes from the input file
    std::vector<process> waiting_q, doing_q, io_q, finished, holding, waiting_for_start;
    
    int t = 0;
    waiting_for_start = order_q;
    
    printf("time %dms: Simulator started for SJF ", t);
    print_queue(waiting_q);
    
    int t_cs =  T_CS/2;
    bool notempty = false;
    bool isend = false;
    int context_s = 0;
    int preemption = 0; 
    
    while(1){

        bool this_end = false;
        
        //--------------------
        //check if a process would arrive
        for(unsigned int i = 0; i < waiting_for_start.size(); i++) {
            if(waiting_for_start[i].getinitialtime() == t) {
                waiting_for_start[i].add_newturn(1);
                waiting_q.push_back(waiting_for_start[i]);
                //sort
                std::sort(waiting_q.begin(), waiting_q.end(), SJF_Sort);
                printf("time %dms: Process %c arrived ", t, waiting_for_start[i].getid());
                print_queue(waiting_q);
                waiting_for_start.erase(waiting_for_start.begin()+i);
                i--;
            }
        }
        
        std::sort(waiting_q.begin(), waiting_q.end(), SJF_Sort);
        
        //--------------------
        //check if it is last context switch
        if( waiting_q.empty() && doing_q.empty() && holding.empty() && io_q.empty() ) {
            //context switch
            if(isend == true){
                t_cs = T_CS/2;
                t_cs--;
                isend = false;
            }
            else{
                t_cs--;
                if(t_cs == 0){
                    printf("time %dms: Simulator ended for SJF\n", t);
                    break;
                }
            }
        }

        std::vector<process> io_buffer;

        //--------------------
        //check if CPU has process running
        if(!doing_q.empty()) {

            doing_q[0].burst_one();
            doing_q[0].add_to_last(1);
            //check if the current process in the CPU is about to finish all bursts
            if(doing_q[0].gettmpbursttime()== 0 && doing_q[0].gettmptask()== 1) {
                
                finished.push_back(doing_q[0]);
                printf("time %dms: Process %c terminated ", t, doing_q[0].getid());
                print_queue(waiting_q);
                
                doing_q.clear();
                isend = true;
                this_end = true;
            }
            //check if the current process in the CPU is about to finish one of bursts, not the last one
            else if(doing_q[0].gettmpbursttime()== 0 && doing_q[0].gettmptask()!= 1) {
                doing_q[0].finishonce();
                printf("time %dms: Process %c completed a CPU burst; %d to go ", t, doing_q[0].getid(), doing_q[0].gettmptask());
                print_queue(waiting_q);
                printf("time %dms: Process %c blocked on I/O until time %dms ", t, doing_q[0].getid(), t+doing_q[0].getiot());
                print_queue(waiting_q);
                
                doing_q[0].burstchange();
                doing_q[0].changeio(1);
                io_q.push_back(doing_q[0]);
                doing_q.clear();
                isend = true;
                this_end = true;
            }
        }

        //--------------------
        // check if in the context switch
        if(!holding.empty()){
            
            t_cs--;

            if (t_cs == 0){
            
                //enter the CPU
                doing_q.push_back(holding[0]);
                
                printf("time %dms: Process %c started using the CPU ", t, holding[0].getid());
                print_queue(waiting_q);
                
                //finished holding
                holding.clear();
                notempty = false;
            
            }
            else{
                holding[0].add_to_last(1);
            }
        }

        //--------------------
        //check if have processes in the io queue
        if(!io_q.empty()){

            for (unsigned int i = 0; i< io_q.size(); i++){
                io_q[i].io_one();

                if(io_q[i].getiot() == 0){
                    io_q[i].iochange();

                    //check if doing_q is empty and notempty
                    if(doing_q.empty() && notempty == false){
                        notempty = true;
                        if(this_end == false){isend = false;}
                        bool nopush = false;
                        for(unsigned int w = 0; w < waiting_q.size(); w++){
                            if(waiting_q[w].getid() == io_q[i].getid()){
                                nopush = true;
                                waiting_q[w] = io_q[i];
                            }
                        }
                        if(nopush == false){
                            io_q[i].add_newturn(1);
                            waiting_q.push_back(io_q[i]);
                            std::sort(waiting_q.begin(), waiting_q.end(), SJF_Sort);
                        }
                        printf("time %dms: Process %c completed I/O ", t, io_q[i].getid());
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
                            io_q[i].add_newturn(1);
                            waiting_q.push_back(io_q[i]);
                            std::sort(waiting_q.begin(), waiting_q.end(), SJF_Sort);
                        }
                        printf("time %dms: Process %c completed I/O ", t, io_q[i].getid());
                        print_queue(waiting_q);
                    }

                    io_buffer.push_back(io_q[i]);
                }
            }
        }

        //--------------------
        // check if any process is doing I/O
        if(!io_buffer.empty()){
            for(unsigned int i = 0; i < io_buffer.size(); i++){
                for(unsigned int w = 0; w < io_q.size(); w++){
                    if(io_buffer[i].getid() == io_q[w].getid()){io_q.erase(io_q.begin()+w);}}
            }
        }

        //--------------------
        //previous process just finished and next process can get into the queue
        if(doing_q.empty()&&holding.empty()&&!waiting_q.empty()){
            holding.push_back(waiting_q[0]);
            notempty = true;
            waiting_q.erase(waiting_q.begin());
            //context switch
            if(isend == true){
                t_cs = T_CS;
                isend = false;
            }
            else{t_cs = T_CS/2;}
            context_s++;
        }

        //--------------------
        //check if we have processes in the waiting_q which is ready queue 
        if(!waiting_q.empty()){
            for(unsigned int i = 0; i < waiting_q.size(); i++){
                waiting_q[i].add_to_last(1);
                waiting_q[i].addwait_time();
            }
        }
        
        //increase time
        t++;

    }//while(1) ends

    //final calculation and output
    int total_tar_t, total_burst, total_wait, total_task;
    float avg_tar_t, avg_burst, avg_wait;
    total_tar_t = total_burst = total_wait = avg_tar_t = avg_burst = total_task = avg_wait = 0;
    
    for(unsigned int i = 0; i < finished.size(); i++){
        total_task += finished[i].getnumburst();
        total_wait += finished[i].getwaittime();
        total_burst = total_burst + (finished[i].getnumburst() * finished[i].getbursttime());
        total_tar_t += finished[i].total_tar();
    }

    avg_burst = float(total_burst)/float(total_task);   
    avg_wait = float(total_wait)/float(total_task);
    avg_tar_t = float(total_tar_t)/float(total_task);
    
    fprintf(output_file, "Algorithm SJF\n");
    fprintf(output_file, "-- average CPU burst time: %.2f ms\n", avg_burst);
    fprintf(output_file, "-- average wait time: %.2f ms\n", avg_wait);
    fprintf(output_file, "-- average turnaround time: %.2f ms\n", avg_tar_t );
    fprintf(output_file, "-- total number of context switches: %d\n", context_s);
    fprintf(output_file, "-- total number of preemptions: %d\n", preemption);
}