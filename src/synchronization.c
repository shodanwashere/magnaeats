#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "synchronization.h"

sem_t * semaphore_create(char* name, int value){
    int uid = getuid();
    char newname[50];
    sprintf(newname, "%s%d", name, uid);
    sem_t * sem = sem_open(newname, O_RDWR, S_IRWXU, value);
    return sem;
}

void semaphore_destroy(char *name, sem_t *semaphore){
    int uid = getuid();
    char todestroy[50];
    sprintf(todestroy, "%s%d", name, uid);
    
    int e;
    if((e = sem_close(semaphore)) == -1){
        perror("what");
        exit(-1);
    }
    if((e = sem_unlink(todestroy)) == -1){
        perror("how");
        exit(-1);
    }
    return;
}

void produce_begin(struct prodcons* pc){
    //let's wait till its empty
    sem_wait(&(pc->empty));
    //let's also wait till we have mutual exclusivity
    sem_wait(&(pc->mutex));
    //now, we pass the reins to the producer
    return;
}

void produce_end(struct prodcons* pc){
    //the producer has finished producing
    //hand mutual exclusivity over
    sem_post(&(pc->mutex));
    //announce that the buffer is full
    sem_post(&(pc->full));
    //producer has finished
    return;
}

void consume_begin(struct prodcons* pc){ //TIME TO CONSOOOM
    //let's wait till its full
    sem_wait(&(pc->full));
    //wait till you've been handed mutual exclusivity
    sem_wait(&(pc->mutex));
    //ready to consoom. hand the reins to the consoomer
    return;
}

void consume_end(struct prodcons* pc){
    //producer has consoomed. hand over mutual exclusivity
    sem_post(&(pc->mutex));
    //announce that the buffer is empty
    sem_post(&(pc->empty));
    //consumer has finished (let's stop the consoomer joke)
    return;
}

void semaphore_mutex_lock(sem_t* sem){
    sem_wait(&sem);
}

void semaphore_mutex_unlock(sem_t* sem){
    sem_post(&sem);
}