//
//  dynamical_array.cpp
//  assn_4
//
//  Created by Leo on 11/8/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#include "queue.hpp"

using namespace std;

namespace assn_4 {
    Queue::Queue()
    {
        head_ = __null;
        tail_ = __null;
        size_ = 0;
    }
    
    Queue::~Queue()
    {
        delete[] head_;
    }
    
    void Queue::add(long data)
    {
        QueueElement* e = new QueueElement(data, __null);
        
        if (tail_ == __null)
        {
            head_ = e;
            tail_ = e;
        }
        else
        {
            tail_->next = e;
            tail_ = e;
        }
        size_++;
    }
    
    long Queue::get()
    {
        if (is_empty())
        {
            cerr << "DynamicalArray.get: Already empty!" << endl;
            exit(1);
        }
        
        QueueElement* e = head_;
        head_ = head_->next;
        if (head_ == __null)
        {
            tail_ = __null;
        }
        long data = e->data;
        delete e;
        size_--;
        return data;
    }
    
    int Queue::get_size()
    {
        return size_;
    }
    
    bool Queue::is_empty()
    {
        return size_ == 0;
    }
}