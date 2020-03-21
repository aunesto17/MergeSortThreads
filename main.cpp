//
//  main.cpp
//  mergeSortThreads
//
//  Created by Alexander Arturo Baylon Ibanez on 16/09/15.
//  Copyright (c) 2015 Alexander Arturo Baylon Ibanez. All rights reserved.
//

#include <stdio.h>
#include <cstdlib>
#include <pthread.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <random>
#include <algorithm>
#include <iterator>
#include <functional>
#include <chrono>
using namespace std;

//Merge Sort
void mergeHelp(int *,int,int,int);
void mergeSort_mt(int *,int,int,int);
void* mergeSort_thread (void*);
void mergeSort_pub (int *,int,int);

struct parametros{
    int * vec;
    int left;
    int right;
    int prof;
};

//incializamos un mutex global
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

//Generador
vector<int> crear_vector_random(int n) {
    auto sem  = random_device()();
    auto gen  = mt19937(sem);
    auto dist = uniform_int_distribution<int>();
    auto v    = vector<int>(n);
    generate(begin(v), end(v), bind(dist, gen));
    return v;
}

////
////

void mergeHelp(int * vec, int left, int mid, int right)
{
    int tempArray[right-left+1];
    int pos=0,lpos = left,rpos = mid + 1;
    while(lpos <= mid && rpos <= right)
    {
        if(vec[lpos] < vec[rpos])
            tempArray[pos++] = vec[lpos++];
        else
            tempArray[pos++] = vec[rpos++];
    }
    while(lpos <= mid)  tempArray[pos++] = vec[lpos++];
    while(rpos <= right)tempArray[pos++] = vec[rpos++];
    for(int iter = 0;iter < pos; iter++)
        vec[iter+left] = tempArray[iter];
    return;
}

void mergeSort_mt(int * vec, int left, int right,int prof)
{
    if(left >= right) return;
    int mid = (left+right)/2;
    pthread_mutex_lock(&mtx);
    pthread_mutex_unlock(&mtx);
    //Cuando se crean todos los Threads llamamos a la funcion recursivamente.
    if((right-left)<4 || prof<=0){
        mergeSort_mt(vec,left,mid,0);
        mergeSort_mt(vec,mid+1,right,0);
    }
    else{
        //Creamos los parametros del Thread
        struct parametros parm = {vec,left,mid,prof/2};
        pthread_t thread;
        
        pthread_mutex_lock(&mtx);
        cout << "Creando thread en profundidad..." << prof << endl;
        pthread_mutex_unlock(&mtx);
        
        pthread_create(&thread,NULL,mergeSort_thread, &parm);
        mergeSort_mt(vec,mid+1,right,prof/2);
        
        pthread_join(thread, NULL);
        
        pthread_mutex_lock(&mtx);
        cout << "Terminando thread..." << endl;
        pthread_mutex_unlock(&mtx);
    }
    // Juntamos las particiones
    mergeHelp(vec,left,mid,right);
}

void * mergeSort_thread (void* params){
    struct parametros * p = (struct parametros*) params;
    mergeSort_mt(p->vec,p->left, p->right, p->prof);
    return params;
}

void mergeSort_pub (int * vec, int left, int right){
    mergeSort_mt(vec,left,right,4);
}

int main(int argc, const char * argv[]) {
    auto random_data = crear_vector_random(2048);
    
    /*
     for (int i=0; i<100; i++) {
     cout << "(" << i << ")" << " " << random_data[i] << endl;
     }
     */
    int *data = (int*)malloc (2048 * sizeof(*data));
    for (int j=0; j<2048; j++) {
        data[j] = random_data[j];
    }
    /*
    for (int i=0; i<100; i++) {
        cout << "(" << i << ")" << " " << data[i] << endl;
    }
    */
    
    mergeSort_pub(data,0,2047);
    
    for (int i=0; i<2048; i++) {
        cout << "(" << i << ")" << " " << data[i] << endl;
    }
    
    
    free(data);
    return 0;
}
