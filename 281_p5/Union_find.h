//
// Created by Administrator on 2017/12/8.
//

#ifndef INC_281_P5_UNION_FIND_H
#define INC_281_P5_UNION_FIND_H

#include <iostream>
#include <stdlib.h>

int* Init(int vertex_num);
void Getset(int *a, int vertex_num);
int Find(int *a,int vertex_id);
void Union(int *a,int x,int y);

int* Init(int vertex_num){
    int *a=new int[vertex_num+1];
    return a;
}

void Getset(int *a, int vertex_num){
    for(int i=1;i<=vertex_num;i++){
        a[i]=0;
    }
}

int Find(int *a,int vertex_id){
    if(a[vertex_id]<=0) return vertex_id;
    else return a[vertex_id]=Find(a,a[vertex_id]);
}

void Union(int *a,int x,int y){
    int root_of_x=Find(a,x);
    int root_of_y=Find(a,y);
    if(a[root_of_x]==a[root_of_y]){
        a[root_of_x]=root_of_y;
        a[root_of_y]--;
        return;
    }
    if(abs(a[root_of_x])>abs(a[root_of_y])) a[root_of_y]=root_of_x;
    else a[root_of_x]=root_of_y;
}
#endif //INC_281_P5_UNION_FIND_H
