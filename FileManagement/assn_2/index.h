//
//  Index.h
//  assn_2
//
//  Created by Leo on 9/26/15.
//  Copyright (c) 2015 Haiyu Yao. All rights reserved.
//

#ifndef assn_2_Index_h
#define assn_2_Index_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <sys/stat.h>
#include <ios>

using namespace std;

namespace assn_2 {
    
    class IndexRecord
    {
    public:
        int key;
        long offset;
        
        IndexRecord()
        {
            
        }
        
        IndexRecord(int key, long offset): key(key), offset(offset)
        {
            
        }
        
    };
    
    int compar(const void* i1, const void* i2)
    {
        if (i1 == __null && i2 == __null)
        {
            return 0;
        }
        else if(i1 == __null)
        {
            return 1;
        }
        else if (i2 == __null)
        {
            return -1;
        }
        
        IndexRecord** r1 = (IndexRecord**)i1;
        IndexRecord** r2 = (IndexRecord**)i2;
        
        if ((*r1)->key == (*r2)->key)
        {
            return 0;
        }
        else if ((*r1)->key < (*r2)->key)
        {
            return -1;
        }
        else
        {
            return 1;
        }
        
    }
    
    class Index
    {
    public:
        Index()
        {
            records = new IndexRecord*[maxSize];
            records_size = 0;
        }
        ~Index()
        {
            for (int i = 0; i<records_size; i++)
            {
                delete records[i];
            }
            delete records;
        }
        
        void sort()
        {
            qsort(records, records_size, sizeof(IndexRecord**), compar);
        }
        
        void add(int key, long offset)
        {
            records[records_size++] = new IndexRecord(key, offset);
            sort();
        }
        
        bool del(int key)
        {
            long pos = find_pos(key);
            if (pos != -1)
            {
                delete records[pos];
                records[pos] = records[records_size - 1];
                records[records_size - 1] = __null;
                records_size--;
                sort();
            }
            
            return pos != -1;
        }
        
        long find_pos(int key)
        {
            long start = 0;
            long end = records_size -1;
            
            while (start <= end)
            {
                long mid = (start + end) / 2;
                IndexRecord* rec = records[mid];
                if (rec->key == key)
                {
                    return mid;
                }
                else if (rec->key < key)
                {
                    start = mid + 1;
                }
                else
                {
                    end = mid -1;
                }
            }
            
            return -1;
        }
        
        IndexRecord* find(int key)
        {
            long pos = find_pos(key);
            if (pos == -1)
            {
                return __null;
            }
            else
            {
                return records[pos];
            }
        }
        
        void load(const char* index_file_name)
        {
            ifstream index_file;
            struct stat state_buffer;
            if (stat (index_file_name, &state_buffer) == 0)
            {
                index_file.open(index_file_name, ios::in|ios::binary);
            }
            else
            {
                return;
            }
            
            if (!(index_file.is_open()))
            {
                cerr << "Failed to open file " << index_file_name << "!" << endl;
                exit(1);
            }
            
            while(true)
            {
                int key_buffer;
                if (!(index_file.read((char*) &key_buffer, sizeof(int))))
                {
                    break;
                }
                
                long offset_buffer;
                if (!(index_file.read((char*) &offset_buffer, sizeof(long))))
                {
                    break;
                }
                
                records[records_size++] = new IndexRecord(key_buffer, offset_buffer);
                
            }
            index_file.close();
            
        }
        
        void save(const char* index_file_name)
        {
            ofstream index_file;
            struct stat state_buffer;
            if (stat (index_file_name, &state_buffer) != 0)
            {
                index_file.open(index_file_name, ios::out|ios::binary|fstream::trunc);
            }
            else
            {
                index_file.open(index_file_name, ios::out|ios::binary);
            }
            
            if (!(index_file.is_open()))
            {
                cerr << "Failed to open file " << index_file_name << "!" << endl;
                exit(1);
            }
            
            for(int i = 0; i < records_size; i++)
            {
                IndexRecord* record = records[i];
                index_file.write((char*)(&(record->key)), sizeof(int));
                index_file.write((char*)(&(record->offset)), sizeof(long));
            }
            index_file.close();
        }
        
        void print()
        {
            cout << "Index:" << endl;
            for(int i = 0; i < records_size; i++)
            {
                IndexRecord* record = records[i];
                cout << "key=" << record->key << ": offset=" << record->offset << endl;
            }
        }
        
    private:
        const static int maxSize = 10010;
        IndexRecord** records;
        long records_size;
    };
}

#endif
