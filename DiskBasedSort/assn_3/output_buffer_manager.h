//
//  output_buffer_manager.h
//  assn_3
//
//  Created by Leo on 10/17/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef output_buffer_manager_h
#define output_buffer_manager_h


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
#include "common.h"

using namespace std;

namespace assn_3 {
    class OutputBufferManager
    {
    public:
        OutputBufferManager(int buffer_size)
        {
            written_size_ = 0;
            buffer_size_ = buffer_size;
            output_buffer_ = new int[buffer_size_];
        }
        
        ~OutputBufferManager()
        {
            delete[] output_buffer_;
        }
        
        void open(const char* file_name)
        {
            written_size_ = 0;

            output_file_ = new ofstream(file_name, ios::out|ios::binary|fstream::trunc);
            if (!(output_file_->is_open()))
            {
                cerr << "OutputBufferManager.open: Failed to open file " << file_name << "!" << endl;
                exit(1);
            }
        }
        
        void close()
        {
            flush();
            output_file_->close();
            delete output_file_;
        }
        
        void write(int key)
        {
            if (written_size_ >= buffer_size_)
            {
                writeFile();
            }
            output_buffer_[written_size_++] = key;
        }
        
        void flush()
        {
            if (written_size_ > 0)
            {
                writeFile();
            }
        }
        
        long getWriteCount()
        {
            return iowrite_count_;
        }
        
        void resetWriteCount()
        {
            iowrite_count_ = 0;
        }
        
        
    private:
        int buffer_size_;
        int* output_buffer_;
        int written_size_;
        ofstream* output_file_;
        long iowrite_count_;
        
        void writeFile()
        {
            output_file_->write((char*)(output_buffer_), sizeof(int) * written_size_);
            written_size_ = 0;
            iowrite_count_++;
        }
    };
}


#endif /* output_buffer_manager_h */
