//
//  multistep_merge_sort.h
//  assn_3
//
//  Created by Leo on 10/18/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef multistep_merge_sort_h
#define multistep_merge_sort_h



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
#include "run_file_mgr.h"
#include "output_buffer_manager.h"
#include "composite_heap.h"

using namespace std;

namespace assn_3 {
    
    class MultistepMergeSort
    {
    public:
        MultistepMergeSort(const char* input_file_name, const char* output_file_name, int input_buffer_size, int output_buffer_size)
        {
            output_buffer_manager_ = new OutputBufferManager(output_buffer_size);
            input_file_name_ = input_file_name;
            output_file_name_ = output_file_name;
            long len = strlen(input_file_name);
            
            run_file_name_prefix_ = new char[len+2];
            memcpy(run_file_name_prefix_, input_file_name, len);
            run_file_name_prefix_[len] = '.';
            run_file_name_prefix_[len+1] = '\0';
            
            super_run_file_name_prefix_ = new char[len + 8];
            memcpy(super_run_file_name_prefix_, input_file_name, len);
            super_run_file_name_prefix_[len] = '.';
            super_run_file_name_prefix_[len+1] = 's';
            super_run_file_name_prefix_[len+2] = 'u';
            super_run_file_name_prefix_[len+3] = 'p';
            super_run_file_name_prefix_[len+4] = 'e';
            super_run_file_name_prefix_[len+5] = 'r';
            super_run_file_name_prefix_[len+6] = '.';
            super_run_file_name_prefix_[len+7] = '\0';
            
            run_file_mgr_ = new RunFileManager(input_buffer_size, output_buffer_manager_);
        }
        
        ~MultistepMergeSort()
        {
            delete run_file_mgr_;
            delete output_buffer_manager_;
            delete run_file_name_prefix_;
        }
        
        void generateRuns()
        {
            run_count_ = run_file_mgr_->generateRuns(input_file_name_, run_file_name_prefix_);
#ifdef MONITOR
            cout << "IO Read Count: " << run_file_mgr_->getReadCount() << endl;
            cout << "IO Write Count: " << output_buffer_manager_->getWriteCount() << endl;
            run_file_mgr_->resetReadCount();
            output_buffer_manager_->resetWriteCount();
#endif
        }
        
        void generateSuperRuns(int super_run_batch_size)
        {
            super_run_count_ = run_count_ / super_run_batch_size;
            if (run_count_ % super_run_batch_size != 0)
            {
                super_run_count_ += 1;
            }
            
            for (int i = 0; i < super_run_count_; i++)
            {
                stringstream super_run_file_name_generator;
                super_run_file_name_generator << super_run_file_name_prefix_ << setfill('0') << setw(3) << i;
                const char* super_run_file_name = duplicate_chars(super_run_file_name_generator.str().c_str());
                
                int start = i * super_run_batch_size;
                int end = start + super_run_batch_size - 1 < run_count_ ? start + super_run_batch_size - 1 : run_count_ - 1;

#ifdef HEAP
                merge_sort_using_heap(run_file_name_prefix_, super_run_file_name, start, end);
#else
                merge_sort(run_file_name_prefix_, super_run_file_name, start, end);
#endif

                delete[] super_run_file_name;
            }
            
#ifdef MONITOR
            cout << "IO Read Count: " << run_file_mgr_->getReadCount() << endl;
            cout << "IO Write Count: " << output_buffer_manager_->getWriteCount() << endl;
            run_file_mgr_->resetReadCount();
            output_buffer_manager_->resetWriteCount();
#endif
        }
        
        void merge()
        {
#ifdef HEAP
            merge_sort_using_heap(super_run_file_name_prefix_, output_file_name_, 0, super_run_count_-1);
#else
            merge_sort(super_run_file_name_prefix_, output_file_name_, 0, super_run_count_-1);
#endif
            
#ifdef MONITOR
            cout << "IO Read Count: " << run_file_mgr_->getReadCount() << endl;
            cout << "IO Write Count: " << output_buffer_manager_->getWriteCount() << endl;
            run_file_mgr_->resetReadCount();
            output_buffer_manager_->resetWriteCount();
#endif
        }
        
    private:
        const char* input_file_name_;
        const char* output_file_name_;
        char* run_file_name_prefix_;
        char* super_run_file_name_prefix_;
        long run_count_;
        long super_run_count_;
        
        RunFileManager *run_file_mgr_;
        OutputBufferManager* output_buffer_manager_;
        
        void merge_sort(const char* input_file_name_prefix, const char* output_file_name, int start, int end)
        {
            run_file_mgr_->open(input_file_name_prefix, start, end);
            
            output_buffer_manager_->open(output_file_name);
            
            long written_count = 0;
            while(!run_file_mgr_->is_run_all_completed())
            {
                bool first = true;
                int min_key = -1;
                int min_key_run_no = -1;
                
                for (int i = start; i <= end; i++)
                {
                    if (!run_file_mgr_->is_run_completed(i))
                    {
                        int tmp = run_file_mgr_->peek_run(i);
                        if (first)
                        {
                            min_key = tmp;
                            min_key_run_no = i;
                            first = false;
                        }
                        else if (tmp < min_key)
                        {
                            min_key = tmp;
                            min_key_run_no = i;
                        }
                    }
                }
                
                min_key = run_file_mgr_->read_run(min_key_run_no);
                output_buffer_manager_->write(min_key);
                written_count++;
            }
            
            output_buffer_manager_->flush();
            output_buffer_manager_->close();
            
            run_file_mgr_->close();
        }
        
        void merge_sort_using_heap(const char* input_file_name_prefix, const char* output_file_name, int start, int end)
        {
            run_file_mgr_->open(input_file_name_prefix, start, end);
            int count = end - start + 1;
            
            output_buffer_manager_->open(output_file_name);
            
            Element* e_arr = new Element[count];
            for (int i = start; i <= end; i++)
            {
                int index = i - start;
                if (!run_file_mgr_->is_run_completed(i))
                {
                    int tmp = run_file_mgr_->read_run(i);
                    e_arr[index].value = tmp;
                    e_arr[index].run_no = i;
                }
            }
            
            CompositeHeap<Element> heap(e_arr, count);
            heap.buildHeap();
            
            while(!heap.isEmpty())
            {
                Element min = heap.pop();
                output_buffer_manager_->write(min.value);
                
                if (!run_file_mgr_->is_run_completed(min.run_no))
                {
                    Element e;
                    e.value = run_file_mgr_->read_run(min.run_no);
                    e.run_no = min.run_no;
                    heap.add(e);
                }
            }
            
            delete[] e_arr;
            output_buffer_manager_->flush();
            output_buffer_manager_->close();
            
            run_file_mgr_->close();
        }
    };
}

#endif /* multistep_merge_sort_h */
