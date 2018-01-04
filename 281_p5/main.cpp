#include <iostream>
#include <vector>
#include <map>
#include <limits.h>
#include "binary_heap.h"
#include "fib_heap.h"
#include "Union_find.h"
#include <fstream>
using namespace std;

struct edge{
    int s;
    int t;
    int weight;
    edge(int s,int t,int weight){
        this->s=s;
        this->t=t;
        this->weight=weight;
    }
};

struct node{
    node* prev;
    int dist;
    int d;
    map<int,int> a_list;
    node(int N){
        this->prev=NULL;
        this->dist=INT_MAX;
        this->d=0;
    }
};

struct compare_edge
{
    bool operator()(edge *a, edge *b) const
    {
        return a->weight > b->weight;
    }
};

struct compare_dist
{
    bool operator()(node *a, node *b) const
    {
        return a->dist > b->dist;
    }
};

struct compare_degree
{
    bool operator()(node *a, node *b) const
    {
        return a->d > b->d;
    }
};

struct graph{
    vector<node*> nd;
};

int Find_node_degree(node* target,binary_heap<node*,compare_degree> &TS){
    for(int i=0;i<TS.data.size();i++){
        if(TS.data[i]==target){
            return i+1;
        }
    }
}

int Find_node_dist(node* target,binary_heap<node*,compare_dist> &TS){
    for(int i=0;i<TS.data.size();i++){
        if(TS.data[i]==target){
            return i+1;
        }
    }
}

void Find_short(graph &G,int s,int t){
    int length;
    node* v;
    if(s==t) {
        length=0;
    }
    else{
        binary_heap<node*, compare_dist> BPQ;
        G.nd[s]->dist=0;
        for(int i=0;i<G.nd.size();i++){
            BPQ.enqueue(G.nd[i]);
        }
        v=G.nd[s];
        do{
//            if(v->a_list.empty()){
//                cout<<"No path exists!"<<endl;
//                return;
//            }
            map<int,int>::iterator iter;
            for(iter=v->a_list.begin();iter!=v->a_list.end();iter++){
                int u=iter->first;
                int temp=v->dist+iter->second;
                if(temp<G.nd[u]->dist){
                    G.nd[u]->dist=temp;
                    int id=Find_node_dist(G.nd[u],BPQ);
                    BPQ.Percolate(id);
                    G.nd[u]->prev=v;
                }
            }
            node* trash=BPQ.dequeue_min();
            if(BPQ.empty()){
                break;
            }
            v=BPQ.get_min();
        }while(v!=G.nd[t]);
        if(G.nd[t]->dist==INT_MAX){
            cout<<"No path exists!"<<endl;
            return;
        }
        length=G.nd[t]->dist;
    }
    cout<<"Shortest path length is "<<length<<endl;
}

void DAG(graph &G){
    binary_heap<node*,compare_degree> TS;
    for(int i=0;i<G.nd.size();i++){
        TS.enqueue(G.nd[i]);
    }
    node *tmp=TS.get_min();
    while(tmp->d==0){
        map<int,int>::iterator iter;
        for(iter=tmp->a_list.begin();iter!=tmp->a_list.end();iter++){
            int u=iter->first;
            G.nd[u]->d-=1;
            int id=Find_node_degree(G.nd[u],TS);
            TS.Percolate(id);
        }
        tmp=TS.dequeue_min();
        if(TS.empty())break;
        tmp=TS.get_min();
    }
    if(TS.empty()){
        cout<<"The graph is a DAG"<<endl;
    } else{
        cout<<"The graph is not a DAG"<<endl;
    }
}


void Kruskal(graph &G,int N,binary_heap<edge*,compare_edge> &MST){
    int total_weight=0;
    int *a=Init(N);
    Getset(a,N);
    map<int,int>visit;
    for(int i=0;i<G.nd.size();i++){
        visit.insert(map<int,int>::value_type(i,0));
    }
    while(!MST.empty()) {
        edge* tmp=MST.dequeue_min();
        if(Find(a,tmp->s+1)!=Find(a,tmp->t+1)){
            Union(a,tmp->s+1,tmp->t+1);
            total_weight+=tmp->weight;
            map<int,int>::iterator itr;
            itr=visit.find(tmp->s);
            if(itr!=visit.end()){
                visit.erase(itr);
            }
            itr=visit.find(tmp->t);
            if(itr!=visit.end()){
                visit.erase(itr);
            }
        }
    }
    int exam=Find(a,1);
    for(int j=2;j<=N;j++){
        if(Find(a,j)!=exam){
            cout<<"No MST exists!"<<endl;
            return;
        }
    }
    cout<<"The total weight of MST is "<<total_weight<<endl;
}

int main() {
    fstream iFile;
    iFile.open("8.in");
    graph G;
    binary_heap<edge*,compare_edge> MST;
    int N;
    int s;
    int t;
    int w1=-1;
    int w2;
    int weight;
    iFile>>N>>s>>t;
    for(int i=0;i<N;i++){
        node *tmp=new node(N);
        G.nd.insert(G.nd.begin(),tmp);
    }
    while(!iFile.eof()){
        w1=-1;
        iFile>>w1>>w2>>weight;
        if(w1==-1)break; //check whether the input is empty
        G.nd[w1]->a_list.insert(map<int,int>::value_type(w2,weight));
        edge* n_edge=new edge(w1,w2,weight);
        MST.enqueue(n_edge);
        G.nd[w2]->d+=1;
    }
    Find_short(G,s,t);
    DAG(G);
    Kruskal(G,N,MST);
    return 0;
}