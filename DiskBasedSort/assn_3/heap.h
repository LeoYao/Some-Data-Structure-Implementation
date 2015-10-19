//
//  heap.h
//  assn_3
//
//  Created by Leo on 10/16/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef heap_h
#define heap_h


#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <sys/stat.h>
#include <ios>


using namespace std;

namespace assn_3 {
    class Heap
    {
    public:
        Heap(int* buffer, int data_size)
        {
            heap_ = buffer;
            size_ = data_size;
        }
        
        void buildHeap()
        {
            int startIndex = getParentIndex(size_ - 1);
            
            for (int i = startIndex; i >= 0; i--)
            {
                heapify(i);
            }
        }
        
        void add(int key)
        {
            setValue(size_, key);
            size_++;
            
            int keyIndex = size_ - 1;
            bool needSift = true;
            
            while(needSift && keyIndex > 0)
            {
                needSift = false;
                int pIndex = getParentIndex(keyIndex);
                
                int pKey = getValue(pIndex);
                int key = getValue(keyIndex);
                
                if (key < pKey)
                {
                    swap(keyIndex, pIndex);
                    keyIndex = pIndex;
                    needSift = true;
                }
            }
        }
        
        void addWithoutHeapify(int key)
        {
            setValue(size_, key);
            size_++;
        }
        
        int pop()
        {
            int top = getValue(0);
            
            int lastValue = getValue(--size_);
            setValue(0, lastValue);
            heapify(0);
            return top;
        }
        
        int peek()
        {
            return getValue(0);
        }
        
        bool isEmpty()
        {
            return size_ <= 0;
        }
        
        void heapify(int index)
        {
            int lIndex = getLeftChildIndex(index);
            int rIndex = getRightChildIndex(index);
            
            int key = getValue(index);
            int minIndex = index;
            
            if (lIndex < size_)
            {
                int lKey = getValue(lIndex);
                if (lKey < key)
                {
                    minIndex = lIndex;
                }
            }
            
            if (rIndex < size_)
            {
                int rKey = getValue(rIndex);
                int minKey = getValue(minIndex);
                if (rKey < minKey)
                {
                    minIndex = rIndex;
                }
            }
            
            if (minIndex != index)
            {
                swap(minIndex, index);
                heapify(minIndex);
            }
        }
        
        virtual int getValue(int index) = 0;
        
        virtual void setValue(int index, int value) = 0;
        
    protected:
        int* heap_;
        int size_;
        
        int getParentIndex(int index)
        {
            if (index <= 0)
            {
                return -1;
            }
            
            return (index + 1)/2 - 1;
        }
        
        int getLeftChildIndex(int index)
        {
            if (index < 0)
            {
                return -1;
            }
            
            int left = 2 * (index + 1) - 1;
            
            return left;
        }
        
        int getRightChildIndex(int index)
        {
            if (index < 0)
            {
                return -1;
            }
            
            int right = 2 * (index + 1) + 1 - 1;
            
            return right;
        }
        
        void swap(int index1, int index2)
        {
            if (index1 >= size_ || index2 >= size_)
            {
                cerr << "Heap.swap: Cannot swap data out of range." << endl;
                exit(1);
            }
            
            int tmp = getValue(index1);
            setValue(index1, getValue(index2));
            setValue(index2, tmp);
        }
        
    };
    
    class LeftHeap : public Heap
    {
    public:
        LeftHeap(int* buffer, int data_size) : Heap(buffer, data_size)
        {
            
        }
        
        int getValue(int index)
        {
            return *(heap_ + index);
        }
        
        void setValue(int index, int value)
        {
            *(heap_ + index) = value;
        }
    };
    
    class RightHeap : public Heap
    {
    public:
        RightHeap(int* buffer, int data_size) : Heap(buffer, data_size)
        {
            
        }
        
        int getValue(int index)
        {
            return *(heap_ - index);
        }
        
        void setValue(int index, int value)
        {
            *(heap_ - index) = value;
        }
    };
    
    
    class HeapBufferManager
    {
    public:
        HeapBufferManager(int* buffer, int buffer_size, int data_size)
        {
            heap1_ = new LeftHeap(buffer, data_size);
            heap2_ = new RightHeap(buffer + buffer_size - 1, 0);
            heap1_->buildHeap();
            prev_value_ = -1;
        }
        
        void switchHeap()
        {
            Heap* t = heap1_;
            heap1_ = heap2_;
            heap2_ = t;
            heap1_->buildHeap();
        }
        
        bool isPrimaryHeapEmpty()
        {
            return heap1_->isEmpty();
        }
        
        int get()
        {
            prev_value_ = heap1_->peek();
            return prev_value_;
        }
        
        void add(int value)
        {
            if (value >= prev_value_)
            {
                heap1_->setValue(0, value);
                heap1_->heapify(0);
            }
            else
            {
                heap1_->pop();
                heap2_->addWithoutHeapify(value);
            }
        }
        
        void pop()
        {
            heap1_->pop();
        }
        
        
    private:
        Heap* heap1_;
        Heap* heap2_;
        int prev_value_;
    };
}


#endif /* heap_h */
