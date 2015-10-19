//
//  small_input_buffer.h
//  assn_3
//
//  Created by Leo on 10/18/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef small_input_buffer_h
#define small_input_buffer_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <sys/stat.h>
#include <ios>
#include <iomanip>
#include "common.h"

using namespace std;

namespace assn_3 {
    class SmallInputBuffer
    {
    public:
        SmallInputBuffer(int* buffer, int buffer_size, ifstream* input_file, int skip_key_count, int total_key_count)
        {
            input_file_ = input_file;
            read_count_ = skip_key_count;
            input_file_key_count = total_key_count;
            ioread_count_ = 0;
            buffer_ = buffer;
            buffer_size_ = buffer_size;
            buffer_offset_ = 0;
            buffer_to_read_count = 0;
        }
        
        
        int read()
        {
            if (is_complete())
            {
                cerr << "SmallInputBuffer.read: The input file has completed." << endl;
                exit(1);
            }
            
            if (buffer_offset_ >= buffer_to_read_count)
            {
                int unread_count = input_file_key_count - read_count_;
                buffer_to_read_count = unread_count < buffer_size_ ? unread_count : buffer_size_;
                
                input_file_->read((char*)buffer_, sizeof(int) * buffer_to_read_count);
                
                buffer_offset_ = 0;
                ioread_count_++;
            }
            
            read_count_++;
            return buffer_[buffer_offset_++];
        }
        
        bool is_complete()
        {
            return read_count_ >= input_file_key_count;
        }
        
        long getReadCount()
        {
            return ioread_count_;
        }
        
        void resetReadCount()
        {
            ioread_count_ = 0;
        }
        
    private:
        int* buffer_;
        int buffer_size_;
        int buffer_offset_;
        int buffer_to_read_count;
        
        ifstream* input_file_;
        int input_file_key_count;
        int read_count_;
        long ioread_count_;
    };
}

#endif /* small_input_buffer_h */
