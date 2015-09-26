//
//  availList.h
//  assn_2
//
//  Created by Leo on 9/26/15.
//  Copyright (c) 2015 Haiyu Yao. All rights reserved.
//

#ifndef assn_2_availList_h
#define assn_2_availList_h


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
    
    class AvailSlot
    {
    public:
        int size;
        long offset;
        
        AvailSlot(int size, long offset): size(size), offset(offset)
        {
            
        }
        
        
    };
    
    
    class FirstFitAvailSlot : public AvailSlot
    {
    public:
        FirstFitAvailSlot* next;
        
        FirstFitAvailSlot(int size, long offset): AvailSlot(size, offset), next(__null)
        {
            
        }
    };
    
    class AvailList
    {
    public:
        const static int maxSize = 10010; // extra 10 in case of some boundary issue
        virtual ~AvailList() {};
        virtual void add(int size, long offset) = 0;
        virtual AvailSlot* get_fit(int size) = 0;
        
        void load(const char* avail_list_file_name)
        {
            ifstream avail_list_file;
            struct stat state_buffer;
            if (stat(avail_list_file_name, &state_buffer) == 0)
            {
                avail_list_file.open(avail_list_file_name, ios::in|fstream::binary);
            }
            else
            {
                return;
            }
            
            if (!(avail_list_file.is_open()))
            {
                cerr << "Failed to open file " << avail_list_file_name << "!" << endl;
                exit(1);
            }
            
            while(true)
            {
                int size_buffer;
                if (!(avail_list_file.read((char*) &size_buffer, sizeof(int))))
                {
                    break;
                }
                
                long offset_buffer;
                if (!(avail_list_file.read((char*) &offset_buffer, sizeof(long))))
                {
                    break;
                }
                
                addWithoutSort(size_buffer, offset_buffer);
            }
            avail_list_file.close();
        }
        
        virtual void save(const char* avail_list_file_name) = 0;
        virtual void print() = 0;
        
    protected:
        virtual void addWithoutSort(int size, long offset) = 0;
    };
    
    class FirstFitAvailList : public AvailList
    {
    public:
        
        FirstFitAvailList()
        {
            header = __null;
        }
        
        ~FirstFitAvailList()
        {
            FirstFitAvailSlot* tmp = header;
            while (tmp != __null)
            {
                FirstFitAvailSlot* tmp2 = tmp->next;
                delete tmp;
                tmp = tmp2;
            }
        }
        
        void add(int size, long offset)
        {
            addWithoutSort(size, offset);
        }
        
        AvailSlot* get_fit(int size)
        {
            FirstFitAvailSlot* tmp = header;
            FirstFitAvailSlot* parent = __null;
            
            while(tmp != __null)
            {
                if(tmp->size >= size)
                {
                    if (parent != __null)
                    {
                        parent->next = tmp->next;
                    }
                    else
                    {
                        header = tmp->next;
                    }
                    
                    if (tmp->next == __null)
                    {
                        tail = parent;
                    }
                    
                    return tmp;
                }
                else
                {
                    parent = tmp;
                    tmp = tmp->next;
                }
            }
            return __null;
        }
        
        
        virtual void save(const char* avail_list_file_name)
        {
            ofstream avail_list_file;
            struct stat state_buffer;
            if (stat (avail_list_file_name, &state_buffer) != 0)
            {
                avail_list_file.open(avail_list_file_name, ios::out|ios::binary|fstream::trunc);
            }
            else
            {
                avail_list_file.open(avail_list_file_name, ios::out|ios::binary);
            }
            
            if (!(avail_list_file.is_open()))
            {
                cerr << "Failed to open file " << avail_list_file_name << "!" << endl;
                exit(1);
            }
            
            FirstFitAvailSlot* record = header;
            
            while (record != __null) {
                avail_list_file.write((char*)(&(record->size)), sizeof(int));
                avail_list_file.write((char*)(&(record->offset)), sizeof(long));
                record = record->next;
            }
            avail_list_file.close();
        }
        
        void print()
        {
            cout << "Availability:" << endl;
            
            FirstFitAvailSlot* record = header;
            int hole_n = 0;
            long hole_size = 0;
            while (record != __null) {
                hole_n++;
                hole_size += record->size;
                cout << "size=" << record->size << ": offset=" << record->offset << endl;
                record = record->next;
            }
            
            cout << "Number of holes: " << hole_n << endl;
            cout << "Hole space: " << hole_size << endl;
        }
        
        virtual void addWithoutSort(int size, long offset)
        {
            FirstFitAvailSlot* new_slot = new FirstFitAvailSlot(size, offset);
            if (header == __null)
            {
                header = new_slot;
            }
            else
            {
                tail->next = new_slot;
            }
            tail = new_slot;
        }
        
        
    private:
        FirstFitAvailSlot* header;
        FirstFitAvailSlot* tail;
        
    };
    
    class SortableAvailList : public AvailList
    {
    public:
        
        SortableAvailList()
        {
            records = new AvailSlot*[maxSize];
            records_size = 0;
        }
        
        ~SortableAvailList()
        {
            for (int i = 0; i< records_size; i++)
            {
                delete records[i];
            }
            delete records;
        }
        
        
        void add(int size, long offset)
        {
            addWithoutSort(size, offset);
            sort();
        }
        
        AvailSlot* get_fit(int size)
        {
            for (int i = 0; i < records_size; i++)
            {
                if (records[i]->size >= size)
                {
                    AvailSlot* tmp = records[i];
                    for (int j = i; j < records_size - 1; j++)
                    {
                        records[j] = records[j+1];
                    }
                    records[records_size - 1] = __null;
                    records_size--;
                    return tmp;
                }
            }
            return __null;
        }
        
        virtual void save(const char* avail_list_file_name)
        {
            ofstream avail_list_file;
            struct stat state_buffer;
            if (stat (avail_list_file_name, &state_buffer) != 0)
            {
                avail_list_file.open(avail_list_file_name, ios::out|ios::binary|fstream::trunc);
            }
            else
            {
                avail_list_file.open(avail_list_file_name, ios::out|ios::binary);
            }
            
            if (!(avail_list_file.is_open()))
            {
                cerr << "Failed to open file " << avail_list_file_name << "!" << endl;
                exit(1);
            }
            
            
            
            for(int i = 0; i < records_size; i++) {
                AvailSlot* record = records[i];
                avail_list_file.write((char*)(&(record->size)), sizeof(int));
                avail_list_file.write((char*)(&(record->offset)), sizeof(long));
            }
            avail_list_file.close();
        }
        
        void print()
        {
            cout << "Availability:" << endl;
            
            int hole_n = 0;
            long hole_size = 0;
            
            for(int i = 0; i < records_size; i++) {
                AvailSlot* record = records[i];
                hole_n++;
                hole_size += record->size;
                cout << "size=" << record->size << ": offset=" << record->offset << endl;
            }
            
            cout << "Number of holes: " << hole_n << endl;
            cout << "Hole space: " << hole_size << endl;
        }
        
        virtual void addWithoutSort(int size, long offset)
        {
            AvailSlot* new_slot = new AvailSlot(size, offset);
            records[records_size] = new_slot;
            records_size++;
        }
        
    protected:
        AvailSlot** records;
        int records_size;
        
        virtual void sort() = 0;
    };
    
    int compar_asc(const void* i1, const void* i2)
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
        
        AvailSlot** r1 = (AvailSlot**)i1;
        AvailSlot** r2 = (AvailSlot**)i2;
        
        if ((*r1)->size == (*r2)->size)
        {
            if ((*r1)->offset < (*r2)->offset)
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        else if ((*r1)->size < (*r2)->size)
        {
            return -1;
        }
        else
        {
            return 1;
        }
        
    }
    
    class BestFitAvailList : public SortableAvailList
    {
    protected:
        void sort()
        {
            qsort(records, records_size, sizeof(AvailSlot**), compar_asc);
        }
    };
    
    int compar_desc(const void* i1, const void* i2)
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
        
        AvailSlot** r1 = (AvailSlot**)i1;
        AvailSlot** r2 = (AvailSlot**)i2;
        
        if ((*r1)->size == (*r2)->size)
        {
            if ((*r1)->offset < (*r2)->offset)
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        else if ((*r1)->size < (*r2)->size)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    
    class WorstFitAvailList : public SortableAvailList
    {
    protected:
        void sort()
        {
            qsort(records, records_size, sizeof(AvailSlot**), compar_desc);
        }
    };
}

#endif
