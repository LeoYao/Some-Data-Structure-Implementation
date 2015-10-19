//
//  composite_heap.h
//  assn_3
//
//  Created by Leo on 10/18/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef composite_heap_h
#define composite_heap_h


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
    
    class Element
    {
    public:
        int value;
        int run_no;
        
        Element& operator= (const Element &rhs)
        {
            value = rhs.value;
            run_no = rhs.run_no;
            return *this;
        }
        
        bool operator <(const Element& rhs)
        {
            return (value < rhs.value);
        }

        bool operator >(const Element& rhs)
        {
            return (value > rhs.value);
        }
        
        bool operator <=(const Element& rhs)
        {
            return (value <= rhs.value);
        }
        
        bool operator >=(const Element& rhs)
        {
            return (value >= rhs.value);
        }
        
        bool operator ==(const Element& rhs)
        {
            return (value == rhs.value);
        }
    };
    
    template <class T>
    class CompositeHeap
    {
    public:
        CompositeHeap<T>(T* buffer, int data_size)
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
        
        void add(T key)
        {
            setValue(size_, key);
            size_++;
            
            int keyIndex = size_ - 1;
            bool needSift = true;
            
            while(needSift && keyIndex > 0)
            {
                needSift = false;
                int pIndex = getParentIndex(keyIndex);
                
                T pKey = getValue(pIndex);
                T key = getValue(keyIndex);
                
                if (key < pKey)
                {
                    swap(keyIndex, pIndex);
                    keyIndex = pIndex;
                    needSift = true;
                }
            }
        }
        
        void addWithoutHeapify(T key)
        {
            setValue(size_, key);
            size_++;
        }
        
        T pop()
        {
            T top = getValue(0);
            
            T lastValue = getValue(--size_);
            setValue(0, lastValue);
            heapify(0);
            return top;
        }
        
        T peek()
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
            
            T key = getValue(index);
            int minIndex = index;
            
            if (lIndex < size_)
            {
                T lKey = getValue(lIndex);
                if (lKey < key)
                {
                    minIndex = lIndex;
                }
            }
            
            if (rIndex < size_)
            {
                T rKey = getValue(rIndex);
                T minKey = getValue(minIndex);
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
        
        T getValue(int index)
        {
            return *(heap_ + index);
        }
        
        void setValue(int index, T value)
        {
            *(heap_ + index) = value;
        }
        
    protected:
        T* heap_;
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
            
            T tmp = getValue(index1);
            setValue(index1, getValue(index2));
            setValue(index2, tmp);
        }
        
    };
}

#endif /* composite_heap_h */
