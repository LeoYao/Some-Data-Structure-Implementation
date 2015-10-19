//
//  run_file_mgr.h
//  assn_3
//
//  Created by Leo on 10/17/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef run_file_mgr_h
#define run_file_mgr_h

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
#include "heap.h"
#include "output_buffer_manager.h"
#include "small_input_buffer.h"

using namespace std;

namespace assn_3 {
    
    class RunFileManager
    {
    public:
        RunFileManager(int buffer_size, OutputBufferManager* output_buffer_manager)
        {
            output_buffer_manger_ = output_buffer_manager;
            input_buffer_size_ = buffer_size;
            run_count_ = 0;
            start_run_no_ = 0;
            min_run_size_ = 0;
            
            input_buffer_ = new int[input_buffer_size_];
            run_files_ = __null;
            min_run_buffer_indices_ = __null;
            run_read_count_ = __null;
            run_file_key_count_ = __null;
            ioread_count_ = 0;
        }
        
        ~RunFileManager()
        {
            delete[] run_files_;
            delete[] input_buffer_;
            delete[] min_run_buffer_indices_;
            delete[] run_read_count_;
            delete[] run_file_key_count_;
        }
        
        long generateRuns(const char* input_file_name, char* run_name_prefix)
        {
            long key_count = getFileSize(input_file_name)/sizeof(int);
            
            long run_count = key_count / input_buffer_size_;
            if (key_count%input_buffer_size_ > 0)
            {
                run_count += 1;
            }
            
            ifstream input_file;
            input_file.open(input_file_name, ios::binary | ios::in);
            if (!(input_file.is_open()))
            {
                cerr << "RunFileManager.generateRuns: Uh oh, " << input_file_name << " could not be opened for reading!" << endl;
                exit(1);
            }
            
            int readed_count = 0;
            int run_no = 0;
            while (readed_count < key_count)
            {
                long unread_key_count = key_count - readed_count;
                long to_read_key_count = unread_key_count < input_buffer_size_ ? unread_key_count : input_buffer_size_;
                
                input_file.read((char*)input_buffer_, sizeof(int) * to_read_key_count);
                ioread_count_++;
                readed_count += to_read_key_count;
                
                qsort(input_buffer_, to_read_key_count, sizeof(int), compar_asc);
                
                stringstream run_file_name_generator;
                run_file_name_generator << run_name_prefix << setfill('0') << setw(3) << run_no;
                const char* run_file_name = duplicate_chars(run_file_name_generator.str().c_str());
                
                output_buffer_manger_->open(run_file_name);
                for (int i = 0; i < to_read_key_count; i++)
                {
                    output_buffer_manger_->write(input_buffer_[i]);
                }
                output_buffer_manger_->close();
                
                run_no++;
                
                delete[] run_file_name;
            }
            input_file.close();
            input_file.clear();
            
            return run_count;
        }
        
        long generateRunsUsingHeap(const char* input_file_name, char* run_name_prefix, int heap_buffer_size)
        {
            long key_count = getFileSize(input_file_name)/sizeof(int);
            int real_input_buffer_size = input_buffer_size_ - heap_buffer_size;
            int* heap_buffer = input_buffer_;
            
            int run_count = 0;
            int processed_count = 0;
            
            ifstream input_file;
            input_file.open(input_file_name, ios::binary | ios::in);
            if (!(input_file.is_open()))
            {
                cerr << "RunFileManager.generateRuns: Uh oh, " << input_file_name << " could not be opened for reading!" << endl;
                exit(1);
            }
            
            //Init heap buffer
            long to_read_key_count = key_count < heap_buffer_size ? key_count : heap_buffer_size;
            input_file.read((char*)heap_buffer, sizeof(int) * to_read_key_count);
            HeapBufferManager heap_buffer_manager(heap_buffer, heap_buffer_size, to_read_key_count);

            //Init input buffer
            SmallInputBuffer real_input_buffer(input_buffer_ + heap_buffer_size, real_input_buffer_size, &input_file, to_read_key_count, key_count);

            bool first = true;
            while (processed_count < key_count)
            {
                stringstream run_file_name_generator;
                run_file_name_generator << run_name_prefix << setfill('0') << setw(3) << run_count;
                const char* run_file_name = duplicate_chars(run_file_name_generator.str().c_str());

                output_buffer_manger_->open(run_file_name);
                
                while (!heap_buffer_manager.isPrimaryHeapEmpty())
                {
                    processed_count++;
                    int min = heap_buffer_manager.get();
                    output_buffer_manger_->write(min);
                    
                    if (!real_input_buffer.is_complete())
                    {
                        int new_value = real_input_buffer.read();
                        heap_buffer_manager.add(new_value);
                    }
                    else
                    {
                        heap_buffer_manager.pop();
                    }
                }
                
                output_buffer_manger_->close();
                run_count++;
                
                delete[] run_file_name;
                
                heap_buffer_manager.switchHeap();
            }
            
            input_file.close();
            
#ifdef MONITOR
            ioread_count_ += real_input_buffer.getReadCount() + 1;
            real_input_buffer.resetReadCount();
#endif
            
            return run_count;
        }
        
        void open(const char* prefix, int start, int end)
        {
            run_count_ = end - start + 1;
            run_files_ = new ifstream[run_count_];
            run_read_count_ = new long[run_count_];
            run_file_key_count_ = new long[run_count_];
            min_run_size_ = input_buffer_size_ /run_count_;
            min_run_buffer_indices_ = new int[run_count_];
            start_run_no_ = start;
            
            char *local_prefix = duplicate_chars(prefix);
            
            for (int i = 0; i < run_count_; i++)
            {
                int file_index = start + i;
                stringstream run_file_name_generator;
                run_file_name_generator << local_prefix << setfill('0') << setw(3) << file_index;
                const char* run_file_name = duplicate_chars(run_file_name_generator.str().c_str());
                
                //get totol number of keys in this run
                run_file_key_count_[i] = getFileSize(run_file_name)/sizeof(int);
                
                //open the run file and cache it
                run_files_[i].open(run_file_name, ios::binary | ios::in);
                if (!(run_files_[i].is_open()))
                {
                    cerr << "RunFileManager.open_all: Uh oh, " << run_file_name << " could not be opened for reading!" << endl;
                    exit(1);
                }
                
                run_read_count_[i] = 0;
                
                delete[] run_file_name;
            }
            
            delete [] local_prefix;
            
            init_buffer();
        }
        
        void close()
        {
            delete[] min_run_buffer_indices_;
            min_run_buffer_indices_ = __null;
            min_run_size_ = -1;
            for(int i = 0; i < run_count_; i++)
            {
                if (run_files_[i].is_open())
                {
                    run_files_[i].close();
                }
            }
        }
        
        bool is_run_completed(int run_no)
        {
            int buffer_index = run_no - start_run_no_;
            
            if (buffer_index >= run_count_)
            {
                cerr << "RunFileManager.is_run_completed: run_no [" << run_no << "] should be in range of "<< start_run_no_ << " - " << start_run_no_ + run_count_ - 1 << endl;
                exit(1);
            }
                
            return run_read_count_[buffer_index] >= run_file_key_count_[buffer_index];
            
        }
        
        bool is_run_all_completed()
        {
            for (int i = 0; i < run_count_; i++)
            {
                if (!is_run_completed(start_run_no_ + i))
                {
                    return false;
                }
            }
            
            return true;
        }
        
        
        int read_run(int run_no)
        {
            int buffer_index = run_no - start_run_no_;
            
            if (buffer_index >= run_count_)
            {
                cerr << "RunFileManager.read_min_run: run_no [" << run_no << "] should be in range of "<< start_run_no_ << " - " << start_run_no_ + run_count_ - 1 << endl;
                exit(1);
            }
            
            if (run_read_count_[buffer_index] >= run_file_key_count_[buffer_index])
            {
                cerr << "RunFileManager.read_min_run: The run file [" << run_no << "] has completed." << endl;
                exit(1);
            }
            
            int min_run_buffer_index = min_run_buffer_indices_[buffer_index];
            
            if (min_run_buffer_index >= (buffer_index + 1) * min_run_size_)
            {
                init_run(buffer_index);
                min_run_buffer_index = min_run_buffer_indices_[buffer_index];
            }
            
            int tmp = input_buffer_[min_run_buffer_index];
            
            min_run_buffer_indices_[buffer_index] += 1;
            run_read_count_[buffer_index] += 1;
            
            return tmp;
        }
        
        int peek_run(int run_no)
        {
            int buffer_index = run_no - start_run_no_;
            
            if (buffer_index >= run_count_)
            {
                cerr << "RunFileManager.peek_min_run: run_no [" << run_no << "] should be in range of "<< start_run_no_ << " - " << start_run_no_ + run_count_ - 1 << endl;
                exit(1);
            }
            
            if (run_read_count_[buffer_index] >= run_file_key_count_[buffer_index])
            {
                cerr << "RunFileManager.peek_min_run: The run file [" << run_no << "] has completed." << endl;
                exit(1);
            }
            
            int min_run_buffer_index = min_run_buffer_indices_[buffer_index];
            if (min_run_buffer_index >= (buffer_index + 1) * min_run_size_)
            {
                init_run(buffer_index);
                min_run_buffer_index = min_run_buffer_indices_[buffer_index];
            }
            return input_buffer_[min_run_buffer_index];
        }
        
        void resetReadCount()
        {
            ioread_count_ = 0;
        }
        
        long getReadCount()
        {
            return ioread_count_;
        }
        
    private:
        ifstream* run_files_;
        long run_count_;
        int start_run_no_;
        int min_run_size_;
        long ioread_count_;
        
        int* input_buffer_;
        int input_buffer_size_;
        int* min_run_buffer_indices_;
        
        long* run_read_count_;
        long* run_file_key_count_;
        OutputBufferManager* output_buffer_manger_;
        
        void init_buffer()
        {
            for (int i = 0; i < run_count_; i++)
            {
                init_run(i);
            }
        }
        
        void init_run(int buffer_index)
        {
            int min_run_buffer_index = buffer_index * min_run_size_;
            min_run_buffer_indices_[buffer_index] = min_run_buffer_index;
            
            long unread_key_count = 0;
            long to_read_key_count = 0;

            unread_key_count = run_file_key_count_[buffer_index] - run_read_count_[buffer_index];

            to_read_key_count = min_run_size_ < unread_key_count ? min_run_size_ : unread_key_count;
            
            run_files_[buffer_index].read((char*)(input_buffer_ + min_run_buffer_index), sizeof(int) * to_read_key_count);
            ioread_count_++;
        }
    };
}


#endif /* run_file_mgr_h */
