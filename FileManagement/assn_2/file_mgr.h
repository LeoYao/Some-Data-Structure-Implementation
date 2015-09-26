//
//  file_mgr.h
//  assn_2
//
//  Created by Leo on 9/26/15.
//  Copyright (c) 2015 Haiyu Yao. All rights reserved.
//

#ifndef assn_2_file_mgr_h
#define assn_2_file_mgr_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <sys/stat.h>
#include <ios>
#include "index.h"
#include "avail_list.h"

using namespace std;

namespace assn_2
{
    typedef int FIT_TYPE;
    class FileMgr
    {
    public:
        static const FIT_TYPE FIRST_FIT = 0;
        static const FIT_TYPE BEST_FIT = 1;
        static const FIT_TYPE WORST_FIT = 2;
        
        FileMgr(FIT_TYPE fit_type)
        {
            index = new Index();
            if (fit_type == FIRST_FIT)
            {
                avail_list = new FirstFitAvailList();
            }
            else if(fit_type == BEST_FIT)
            {
                avail_list = new BestFitAvailList();
            }
            else
            {
                avail_list = new WorstFitAvailList();
            }
        }
        
        ~FileMgr()
        {
            delete index;
            delete avail_list;
        }
        
        void open(const char *data_file_name)
        {
            struct stat state_buffer;
            if (stat (data_file_name, &state_buffer) != 0)
            {
                data_file.open(data_file_name, ios::in|ios::out|ios::binary|fstream::trunc);
            }
            else
            {
                data_file.open(data_file_name, ios::in|ios::out|ios::binary);
            }
            
            if (!(data_file.is_open()))
            {
                cerr << "Failed to open file " << data_file_name << "!" << endl;
                exit(1);
            }
            
            long file_name_len = strlen(data_file_name);
            
            index_file_name = new char[file_name_len + 5];
            avail_list_file_name = new char[file_name_len + 5];
            
            for(int i = 0; i < file_name_len; i++)
            {
                index_file_name[i] = data_file_name[i];
                avail_list_file_name[i] = data_file_name[i];
            }
            
            index_file_name[file_name_len] = '.';
            index_file_name[file_name_len + 1] = 'i';
            index_file_name[file_name_len + 2] = 'd';
            index_file_name[file_name_len + 3] = 'x';
            index_file_name[file_name_len + 4] = '\0';
            avail_list_file_name[file_name_len] = '.';
            avail_list_file_name[file_name_len + 1] = 'a';
            avail_list_file_name[file_name_len + 2] = 'v';
            avail_list_file_name[file_name_len + 3] = 'l';
            avail_list_file_name[file_name_len + 4] = '\0';
            
            index->load(index_file_name);
            avail_list->load(avail_list_file_name);
        }
        
        
        void close()
        {
            data_file.close();
            index->save(index_file_name);
            avail_list->save(avail_list_file_name);
            index->print();
            avail_list->print();
        }
        
        void add(int key, const char* rec)
        {
            IndexRecord* index_record = index->find(key);
            if (index_record != __null)
            {
                cout << "Record with SID=" << key << " exists" << endl;
                return;
            }
            
            int record_size = strlen(rec) * sizeof(char);
            int slot_size = record_size + sizeof(int);
            AvailSlot* slot = avail_list->get_fit(slot_size);
            long offset = 0;
            if (slot != __null)
            {
                offset = slot->offset;
                
                data_file.clear();
                data_file.seekg(offset, fstream::beg);
                data_file.write((char*)(&record_size), sizeof(int));
                data_file.write(rec,record_size);
                
                int rest_size = slot->size - slot_size;
                if (rest_size > 0)
                {
                    avail_list->add(rest_size, offset + slot_size);
                }
                delete slot;
            }
            else
            {
                data_file.clear();
                data_file.seekg(0, fstream::end);
                offset = data_file.tellg();
                data_file.write((char*)(&record_size), sizeof(int));
                data_file.write(rec,record_size);
            }
            
            index->add(key, offset);
        }
        
        void find(int key)
        {
            IndexRecord* index_record = index->find(key);
            if (index_record == __null)
            {
                cout << "No record with SID=" << key << " exists" << endl;
                return;
            }
            
            data_file.clear();
            data_file.seekg(index_record->offset, fstream::beg);
            int record_size = 0;
            data_file.read((char*)(&record_size), sizeof(int));
            int buffer_size = record_size / sizeof(char);
            char record_buffer[buffer_size];
            data_file.read(record_buffer, record_size);
            cout.write(record_buffer, record_size);
            cout << endl;
        }
        
        void del(int key)
        {
            IndexRecord* index_record = index->find(key);
            if (index_record == __null)
            {
                cout << "No record with SID=" << key << " exists" << endl;
                return;
            }
            
            data_file.clear();
            data_file.seekg(index_record->offset, fstream::beg);
            int record_size = 0;
            data_file.read((char*)(&record_size), sizeof(int));
            avail_list->add(record_size + sizeof(int), index_record->offset);
            
            index->del(key);
        }
        
    private:
        char* index_file_name;
        char* avail_list_file_name;
        fstream data_file;
        Index* index;
        AvailList* avail_list;
    };
    
}

#endif
