#ifndef _MAP_H_
#define _MAP_H_
#include "stdint.h"
#include "semaphore.h"
#include "process.h"

#define MEMDEBUG 0

/** 
 * Atomic map
 */

template <class T> class MapNode {
  public:
    MapNode<T>* next;
    uint32_t key;
    T* value;
  
    MapNode(MapNode<T>* next, uint32_t key, T* value) : next(next), key(key), value(value){
      if (MEMDEBUG) Debug::printf("^node\n"); 
    };
    ~MapNode() {
      if (MEMDEBUG) Debug::printf("~node\n");
    }
};

template <class T> class Map {
  private:
    Mutex mutex;
    uint32_t size;

  public: 
    MapNode<T>* head;
    Map(){
      size = 0;
      head = nullptr;
      if (MEMDEBUG) Debug::printf("^map\n");
    };
    ~Map() {
      mutex.lock();
      if (MEMDEBUG) Debug::printf("~map size: %d\n", size);
      MapNode<T>* curr = head;
      while (curr != nullptr) {
        if (MEMDEBUG) Debug::printf("NONEMPTY MAP\n");
        MapNode<T>* old = curr;
        curr = curr->next;
        delete old;
      }
      mutex.unlock();
    }

    void add(uint32_t key, T* value) {
      mutex.lock();
      head = new MapNode<T>(head, key, value);
      size++;
      mutex.unlock();
    }

    T* get(uint32_t key) {
      mutex.lock();
      MapNode<T>* curr = head;
      while (curr != nullptr) {
        if (curr->key == key) {
          mutex.unlock();
          return curr->value;
        }
        curr = curr->next;
      }
      mutex.unlock();
      return nullptr;
    }

    bool contains(uint32_t key) {
      mutex.lock();
      MapNode<T>* curr = head;
      while (curr != nullptr) {
        if (curr->key == key) {
          mutex.unlock();
          return true;
        }
        curr = curr->next;
      }
      mutex.unlock();
      return false;
    }

    bool remove(uint32_t key) {
      mutex.lock();
      if (head == nullptr) return false;
      MapNode<T>* curr = head;
      if (head->key == key) {
        MapNode<T>* old = head;
        head = head->next;
        size--;
        delete old;
        mutex.unlock();
        return true;
      }
      while (curr->next != nullptr) {
        if (curr->next->key == key) {
          MapNode<T>* old = curr->next;
          curr->next = curr->next->next;
          size--;
          delete old;
          mutex.unlock();
          return true;
        }
        curr = curr->next;
      }
      mutex.unlock();
      return false;
    }

    uint32_t getSize() {
      return size;
    }

    void lock() {
      mutex.lock();
    }
    void unlock() {
      mutex.unlock();
    }
};
#endif
