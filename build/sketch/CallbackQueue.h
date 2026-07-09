#line 1 "C:\\Users\\GSV\\Pictures\\IVR_Vijay_V2\\GSV_VARMA_IVR\\GSV_VARMA_IVR\\CallbackQueue.h"
#ifndef CALLBACK_QUEUE_H
#define CALLBACK_QUEUE_H

#include <Arduino.h>

class CallbackQueue {
private:
  static const int MAX_CAPACITY = 20;
  String queue[MAX_CAPACITY];
  int head;
  int tail;
  int count;

public:
  CallbackQueue() {
    head = 0;
    tail = 0;
    count = 0;
  }

  bool enqueue(String number) {
    if (number.length() == 0) return false;
    if (isFull()) return false;
    if (contains(number)) return false; // Avoid duplicate callbacks

    queue[tail] = number;
    tail = (tail + 1) % MAX_CAPACITY;
    count++;
    return true;
  }

  String dequeue() {
    if (isEmpty()) return "";
    String number = queue[head];
    head = (head + 1) % MAX_CAPACITY;
    count--;
    return number;
  }

  bool isEmpty() const {
    return count == 0;
  }

  bool isFull() const {
    return count == MAX_CAPACITY;
  }

  int size() const {
    return count;
  }

  bool contains(String number) const {
    for (int i = 0; i < count; i++) {
      int idx = (head + i) % MAX_CAPACITY;
      if (queue[idx] == number) {
        return true;
      }
    }
    return false;
  }

  void clear() {
    head = 0;
    tail = 0;
    count = 0;
  }
  
  String peek() const {
    if (isEmpty()) return "";
    return queue[head];
  }
};

#endif
