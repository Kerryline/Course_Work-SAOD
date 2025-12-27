#ifndef QUEUE_H
#define QUEUE_H

#include "database.h"

struct QueueNode {
    Record* data;
    QueueNode* next;
};

struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int size;
};

void initQueue(Queue& q);
void enqueue(Queue& q, Record* rec);
Record* dequeue(Queue& q);
bool isEmpty(const Queue& q);
void clearQueue(Queue& q);

#endif