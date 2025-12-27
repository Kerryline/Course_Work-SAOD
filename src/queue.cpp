#include "queue.h"
#include <iostream>

void initQueue(Queue& q) {
    q.front = q.rear = nullptr;
    q.size = 0;
}

void enqueue(Queue& q, Record* rec) {
    QueueNode* newNode = new QueueNode;
    newNode->data = rec;
    newNode->next = nullptr;

    if (q.rear == nullptr) {
        q.front = q.rear = newNode;
    } else {
        q.rear->next = newNode;
        q.rear = newNode;
    }
    q.size++;
}

Record* dequeue(Queue& q) {
    if (isEmpty(q)) return nullptr;

    QueueNode* temp = q.front;
    Record* rec = temp->data;

    q.front = q.front->next;
    if (q.front == nullptr) q.rear = nullptr;

    delete temp;
    q.size--;
    return rec;
}

bool isEmpty(const Queue& q) {
    return q.front == nullptr;
}

void clearQueue(Queue& q) {
    while (!isEmpty(q)) {
        dequeue(q);
    }
}