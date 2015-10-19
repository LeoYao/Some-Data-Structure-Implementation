//
//  repl_sel_merge_sort.h
//  assn_3
//
//  Created by Leo on 10/18/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef repl_sel_merge_sort_h
#define repl_sel_merge_sort_h

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
    
    class ReplacementSelectionMergeSort
    {
    public:
        ReplacementSelectionMergeSort(const char* input_file_name, const char* output_file_name, int input_buffer_size, int output_buffer_size)
        {
            output_buffer_manager_ = new OutputBufferManager(output_buffer_size);
            input_file_name_ = input_file_name;
            output_file_name_ = output_file_name;
            
            long len = strlen(input_file_name);
            run_file_name_prefix_ = new char[len+2];
            memcpy(run_file_name_prefix_, input_file_name, len);
            run_file_name_prefix_[len] = '.';
            run_file_name_prefix_[len+1] = '\0';
            
            run_file_mgr_ = new RunFileManager(input_buffer_size, output_buffer_manager_);
        }
        
        ReplacementSelectionMergeSort()
        {
            delete run_file_mgr_;
            delete output_buffer_manager_;
            delete run_file_name_prefix_;
        }
        
        void generateRuns(int heap_buffer_size)
        {
            run_count_ = run_file_mgr_->generateRunsUsingHeap(input_file_name_, run_file_name_prefix_, heap_buffer_size);
#ifdef MONITOR
            cout << "IO Read Count: " << run_file_mgr_->getReadCount() << endl;
            cout << "IO Write Count: " << output_buffer_manager_->getWriteCount() << endl;
            output_buffer_manager_->resetWriteCount();
#endif
        }
        
        void merge()
        {
            run_file_mgr_->open(run_file_name_prefix_, 0, run_count_ - 1);

#ifdef HEAP
            merge_sort_using_heap();
#else
            merge_sort();
#endif
            
            run_file_mgr_->close();
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
        long run_count_;
        RunFileManager *run_file_mgr_;
        OutputBufferManager* output_buffer_manager_;
        
        void merge_sort()
        {
            output_buffer_manager_->open(output_file_name_);
            
            while(!run_file_mgr_->is_run_all_completed())
            {
                bool first = true;
                int min_key = -1;
                int min_key_run_no = -1;
                
                for (int i = 0; i < run_count_; i++)
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
            }
            
            output_buffer_manager_->flush();
            output_buffer_manager_->close();
        }
        
        void merge_sort_using_heap()
        {
            output_buffer_manager_->open(output_file_name_);
            
            Element* e_arr = new Element[run_count_];
            for (int i = 0; i < run_count_; i++)
            {
                if (!run_file_mgr_->is_run_completed(i))
                {
                    int tmp = run_file_mgr_->read_run(i);
                    e_arr[i].value = tmp;
                    e_arr[i].run_no = i;
                }
            }
            
            CompositeHeap<Element> heap(e_arr, run_count_);
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
        }
    };
}

#endif /* repl_sel_merge_sort_h */
