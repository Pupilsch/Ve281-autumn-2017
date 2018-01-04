#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <algorithm>
#include "priority_queue.h"

// OVERVIEW: A specialized version of the 'heap' ADT implemented as a binary
//           heap.
template<typename TYPE, typename COMP = std::less<TYPE> >
class binary_heap: public priority_queue<TYPE, COMP> {
public:
    typedef unsigned size_type;

    // EFFECTS: Construct an empty heap with an optional comparison functor.
    //          See test_heap.cpp for more details on functor.
    // MODIFIES: this
    // RUNTIME: O(1)
    binary_heap(COMP comp = COMP());

    // EFFECTS: Add a new element to the heap.
    // MODIFIES: this
    // RUNTIME: O(log(n))
    virtual void enqueue(const TYPE &val);

    // EFFECTS: Remove and return the smallest element from the heap.
    // REQUIRES: The heap is not empty.
    // MODIFIES: this
    // RUNTIME: O(log(n))
    virtual TYPE dequeue_min();

    // EFFECTS: Return the smallest element of the heap.
    // REQUIRES: The heap is not empty.
    // RUNTIME: O(1)
    virtual const TYPE &get_min() const;

    // EFFECTS: Get the number of elements in the heap.
    // RUNTIME: O(1)
    virtual size_type size() const;

    // EFFECTS: Return true if the heap is empty.
    // RUNTIME: O(1)
    virtual bool empty() const;

private:
    // Note: This vector *must* be used in your heap implementation.
    std::vector<TYPE> data;
    // Note: compare is a functor object
    COMP compare;

private:
    // Add any additional member functions or data you require here.
};

template<typename TYPE, typename COMP>
binary_heap<TYPE, COMP> :: binary_heap(COMP comp) {
    compare = comp;
    // Fill in the remaining lines if you need.
}

template<typename TYPE, typename COMP>
void binary_heap<TYPE, COMP> :: enqueue(const TYPE &val) {
    data.push_back(val);
    unsigned int id=data.size();
    while (id>1 && compare(data[id/2-1],data[id-1])){
        TYPE tmp=data[id-1];
        data[id-1]=data[id/2-1];
        data[id/2-1]=tmp;
        id=id/2;
    }
    // Fill in the body.
}

template<typename TYPE, typename COMP>
TYPE binary_heap<TYPE, COMP> :: dequeue_min() {
    TYPE tmp=data[0];
    data[0]=data[this->size()-1];
    data.pop_back();
    unsigned int id=1;
    for(unsigned int j=2*id;j<=data.size();j=2*id){
        if(j<data.size()&&compare(data[j-1],data[j]))j++;
        if(!compare(data[id-1],data[j-1]))break;
        TYPE tmp2=data[id-1];
        data[id-1]=data[j-1];
        data[j-1]=tmp2;
        id=j;
    }
    return tmp;
    // Fill in the body.
}

template<typename TYPE, typename COMP>
const TYPE &binary_heap<TYPE, COMP> :: get_min() const {
    return data[0];
    // Fill in the body.
}

template<typename TYPE, typename COMP>
bool binary_heap<TYPE, COMP> :: empty() const {
    return data.size()==0;
    // Fill in the body.
}

template<typename TYPE, typename COMP>
unsigned binary_heap<TYPE, COMP> :: size() const {
    return data.size();
    // Fill in the body.
}

#endif //BINARY_HEAP_H
