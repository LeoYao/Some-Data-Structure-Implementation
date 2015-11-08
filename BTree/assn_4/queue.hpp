//
//  dynamical_array.hpp
//  assn_4
//
//  Created by Leo on 11/8/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef dynamical_array_hpp
#define dynamical_array_hpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <sys/stat.h>
#include <ios>
#include <exception>
#include <iomanip>

namespace assn_4 {
    class QueueElement
    {
    public:
        long data;
        QueueElement* next;
        QueueElement(long _data, QueueElement* _next):data(_data), next(_next){}
        
    };
    
    class Queue {
    public:
        Queue();
        ~Queue();
        int get_size();
        void add(long data);
        long get();
        bool is_empty();
    private:
        int size_;
        QueueElement* head_;
        QueueElement* tail_;
    };
}
#endif /* dynamical_array_hpp */
