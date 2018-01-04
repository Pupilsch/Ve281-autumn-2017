#include <iostream>
#include <getopt.h>
#include <string>
#include <time.h>
#include "binary_heap.h"
#include "fib_heap.h"
#include "unsorted_heap.h"

struct point{
    unsigned int x;
    unsigned int y;
};

struct cell{
    point crd;
    bool reached;
    unsigned int weight;
    unsigned int pathcost;
    cell *predecessor;
};

struct compare_t
{
    bool operator()(cell *a, cell *b) const
    {
        if(a->pathcost!=b->pathcost)return a->pathcost > b->pathcost;
        else{
            if(a->crd.x!=b->crd.x) return a->crd.x>b->crd.x;
            else return a->crd.y>b->crd.y;
        }
    }
};

void Searchpath(cell *C,cell *N){
    N->pathcost=C->pathcost+N->weight;
    N->reached=true;
    N->predecessor=C;
}

void Trace_back(cell *N,point start_p){
    std::vector<cell*> pr;
    std::cout<<"Path:"<<std::endl;
    while(!(N->crd.x==start_p.x && N->crd.y==start_p.y)){
        pr.push_back(N);
        N=N->predecessor;
    }
    pr.push_back(N);
    while(!pr.empty()){
        cell *tmp=pr.back();
        std::cout<<"("<<tmp->crd.x<<", "<<tmp->crd.y<<")"<<std::endl;
        pr.pop_back();
    }
}

void Middle_print(cell *N){
    std::cout<<"Cell ("<<N->crd.x<<", "<<N->crd.y<<") with accumulated length "<<N->pathcost<<" is added into the queue."<<std::endl;
}

void End_print(cell *N){
    std::cout<<"Cell ("<<N->crd.x<<", "<<N->crd.y<<") with accumulated length "<<N->pathcost<<" is the ending point."<<std::endl;
}

int main(int argc, char* argv[]) {
    bool verbose= false;
    int type=0;
    struct option long_options[]={
            {"verbose",0,NULL,'v'},
            {"implementation",1,NULL,'i'},
            {NULL,0,NULL,0}
    };
    int c;
    std::string s;
    while((c=getopt_long(argc,argv,"vi:",long_options,NULL))!=-1){
        switch (c)
        {
            case 'v':
                verbose=true;
                break;
            case 'i':
                s=optarg;
                if(s=="BINARY"){
                    type=0;
                }
                if(s=="UNSORTED"){
                    type=1;
                }
                if(s=="FIBONACCI"){
                    type=2;
                }
                break;
        }
    }
    priority_queue<cell*, compare_t> *BPQ;
    if(type==0) BPQ=new binary_heap<cell*, compare_t>;
    if(type==1) BPQ=new unsorted_heap<cell*, compare_t>;
    if(type==2) BPQ=new fib_heap<cell*, compare_t>;
    unsigned int width;
    unsigned int height;
    std::cin>>width;
    std::cin>>height;
    point start_p;
    point end_p;
    std::cin>>start_p.x>>start_p.y>>end_p.x>>end_p.y;
    cell** grid=new cell*[height];
    for(int m=0;m<height;m++){
        grid[m]=new cell[width];
    }
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            grid[i][j].crd.x=j;
            grid[i][j].crd.y=i;
            grid[i][j].reached= false;
            grid[i][j].pathcost=0;
            std::cin>>grid[i][j].weight;
        }
    }

    //next part is the min path algorithm.
    int t;
    t=clock();
    grid[start_p.y][start_p.x].pathcost=grid[start_p.y][start_p.x].weight;
    grid[start_p.y][start_p.x].reached= true;
    BPQ->enqueue(&grid[start_p.y][start_p.x]);
    int step=0;
    while(!BPQ->empty()){
        cell *C=BPQ->dequeue_min();
        if(verbose){
            std::cout<<"Step "<<step<<std::endl;
            std::cout<<"Choose cell ("<<C->crd.x<<", "<<C->crd.y<<") with accumulated length "<<C->pathcost<<"."<<std::endl;
        }
        cell *N;
        N = &grid[C->crd.y][C->crd.x];
        if (N->crd.x == end_p.x && N->crd.y == end_p.y) {
            if (verbose)End_print(N);
            std::cout << "The shortest path from (" << start_p.x << ", " << start_p.y << ") to " << "(" << end_p.x
                      << ", " << end_p.y << ") " << "is " << N->pathcost
                      << "." << std::endl;
            Trace_back(N, start_p);
            t=clock()-t;
            std::cout<<t<<std::endl;
            return 0;
        }
        int limit;
        limit=C->crd.x+1;
        if(limit<width) {
            if(!grid[C->crd.y][C->crd.x+1].reached) {
                N = &grid[C->crd.y][C->crd.x + 1];
                Searchpath(C, N);
                if (N->crd.x == end_p.x && N->crd.y == end_p.y) {
                    if(verbose)End_print(N);
                    std::cout << "The shortest path from (" << start_p.x << ", " << start_p.y << ") to "<<"("<<end_p.x<<", "<<end_p.y<<") "<<"is " << N->pathcost
                              << "." << std::endl;
                    Trace_back(N, start_p);
                    delete BPQ;
                    t=clock()-t;
                    std::cout<<t<<std::endl;
                    return 0;
                } else {
                    BPQ->enqueue(N);
                    if(verbose)Middle_print(N);
                }
            }
        }
        limit=C->crd.y+1;
        if(limit<height) {
            if(!grid[C->crd.y+1][C->crd.x].reached) {
                N = &grid[C->crd.y + 1][C->crd.x];
                Searchpath(C, N);
                if (N->crd.x == end_p.x && N->crd.y == end_p.y) {
                    if(verbose)End_print(N);
                    std::cout << "The shortest path from (" << start_p.x << ", " << start_p.y << ") to "<<"("<<end_p.x<<", "<<end_p.y<<") "<<"is " << N->pathcost
                              << "." << std::endl;
                    Trace_back(N, start_p);
                    delete BPQ;
                    t=clock()-t;
                    std::cout<<t<<std::endl;
                    return 0;
                } else {
                    BPQ->enqueue(N);
                    if(verbose)Middle_print(N);
                }
            }
        }
        limit=C->crd.x-1;
        if(limit>=0) {
            if(!grid[C->crd.y][C->crd.x-1].reached) {
                N = &grid[C->crd.y][C->crd.x - 1];
                Searchpath(C, N);
                if (N->crd.x == end_p.x && N->crd.y == end_p.y) {
                    if(verbose)End_print(N);
                    std::cout << "The shortest path from (" << start_p.x << ", " << start_p.y << ") to "<<"("<<end_p.x<<", "<<end_p.y<<") "<<"is " << N->pathcost
                              << "." << std::endl;
                    Trace_back(N, start_p);
                    delete BPQ;
                    t=clock()-t;
                    std::cout<<t<<std::endl;
                    return 0;
                } else {
                    BPQ->enqueue(N);
                    if(verbose)Middle_print(N);
                }
            }
        }
        limit=C->crd.y-1;
        if(limit>=0) {
            if (!grid[C->crd.y - 1][C->crd.x].reached) {
                N = &grid[C->crd.y - 1][C->crd.x];
                Searchpath(C, N);
                if (N->crd.x == end_p.x && N->crd.y == end_p.y) {
                    if(verbose)End_print(N);
                    std::cout << "The shortest path from (" << start_p.x << ", " << start_p.y << ") to "<<"("<<end_p.x<<", "<<end_p.y<<") "<<"is " << N->pathcost
                              << "." << std::endl;
                    Trace_back(N, start_p);
                    delete BPQ;
                    t=clock()-t;
                    std::cout<<t<<std::endl;
                    return 0;
                } else {
                    BPQ->enqueue(N);
                    if(verbose)Middle_print(N);
                }
            }
        }
        step++;
    }
    for(int n=0;n<height;n++){
        delete[] grid[n];
    }
    delete[] grid;
    return 0;
}