//
//  main.cpp
//  assn_1
//
//  Created by Leo on 8/22/15.
//  Copyright (c) 2015 Haiyu Yao. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <iomanip>
#include <cstdlib>
#include <cstring>

using namespace std;

struct List
{
    int *values;
    long length;
};

struct Option
{
    bool is_mem;
    bool is_lin;
};

static const char *MEMORY_LINEAR_OPTION = "--mem-lin";
static const char *MEMORY_BINARY_OPTION = "--mem-bin";
static const char *DISK_LINEAR_OPTION = "--disk-lin";
static const char *DISK_BINARY_OPTION = "--disk-bin";

static const int MEMORY_OPTION_LENGTH = strlen("--mem-lin");
static const int DISK_OPTION_LENGTH = strlen("--disk-lin");

long getFileSize(const char* file_name)
{
    ifstream infile;
    infile.open(file_name, ios::binary | ifstream::ate);
    if (!(infile.is_open()))
    {
        // Print an error and exit
        cerr << "Uh oh, " << file_name << " could not be opened for writing!" << endl;
        exit(1);
    }
    
    ifstream::pos_type pos = infile.tellg();
    long size = (long)pos;
    infile.close();
    return size;
}

//Read file into array
List * readFile(const char *file_name, long key_count)
{
    List *list = new List();
    
    list->length = key_count;
    list->values = new int[key_count];
    
    int buffer;
    ifstream infile;
    infile.open(file_name, ios::binary | ios::in);
    if (!(infile.is_open()))
    {
        // Print an error and exit
        cerr << "Uh oh, " << file_name << " could not be opened for writing!" << endl;
        exit(1);
    }
    
    int i = 0;
    while(infile.read((char*) &buffer, sizeof(int)))
    {
        list->values[i++] = buffer;
    }
    infile.close();
    
    return list;
}

//Print elapsed time
void print_time(timeval &tm_start, timeval &tm_end)
{
    signed long sec = tm_end.tv_sec - tm_start.tv_sec;
    signed long usec = tm_end.tv_usec - tm_start.tv_usec;
    if (usec <0)
    {
        usec += 1000000;
        sec--;
    }
    //cout << "Start Time: " << tm_start.tv_sec << "." << setw(6) << setfill('0') << tm_start.tv_usec <<endl;
    //cout << "End Time: " << tm_end.tv_sec << "." << setw(6) << setfill('0') << tm_end.tv_usec <<endl;
    cout << "Time: " << sec << "." << setw(6) << setfill('0') << usec <<endl;
}

//print searcing result
void print_result(timeval &tm_start, timeval &tm_end, List* seek_list, int *hit)
{
    for(int i = 0; i < seek_list->length; i++)
    {
        cout << setw(12) << seek_list->values[i] << ": ";
        if(hit[i] == 1)
        {
            cout << "Yes" << endl;
        }
        else
        {
            cout << "No" << endl;
        }
    }
    print_time(tm_start, tm_end);
}

//Sequential search in memory
void mem_seq_search(List* seek_list, const char* key_file_name, long key_count)
{
    int hit[seek_list->length];
    timeval tm_start;
    gettimeofday( &tm_start, NULL );
    
    List* key_list = readFile(key_file_name, key_count);
    
    for(int i = 0; i < seek_list->length; i++)
    {
        int seek = seek_list->values[i];
        bool found = false;
        for(int j = 0; j < key_list->length; j++)
        {
            if(seek == key_list->values[j])
            {
                found = true;
                break;
            }
        }
        
        if(found)
        {
            hit[i] = 1;
        }
        else
        {
            hit[i] = 0;
        }
    }
    timeval tm_end;
    gettimeofday( &tm_end, NULL );
    print_result(tm_start, tm_end, seek_list, hit);
    
    //release memory
    delete[] key_list->values;
    delete key_list;
}

//Binary search in memory
bool mem_bin_search_action(long start, long end, int seek, List* key_list)
{
    if (start > end)
    {
        return false;
    }
    else
    {
        long mid = (start + end)/2;
        int key = key_list->values[mid];
        if (key == seek)
        {
            return true;
        }
        else if (key < seek)
        {
            return mem_bin_search_action(mid + 1, end, seek, key_list);
        }
        else
        {
            return mem_bin_search_action(start, mid - 1, seek, key_list);
        }
    }
}

void mem_bin_search(List* seek_list, const char* key_file_name, long key_count)
{
    int hit[seek_list->length];
    
    timeval tm_start;
    gettimeofday( &tm_start, NULL );

    List* key_list = readFile(key_file_name, key_count);
    
    for(int i = 0; i < seek_list->length; i++)
    {
        int seek = seek_list->values[i];
        bool found = false;
        long start = 0;
        long end = key_list->length - 1;
        
        found = mem_bin_search_action(start, end, seek, key_list);
        
        if(found)
        {
            hit[i] = 1;
        }
        else
        {
            hit[i] = 0;
        }
    }
    timeval tm_end;
    gettimeofday( &tm_end, NULL );
    
    print_result(tm_start, tm_end, seek_list, hit);

    //release memory
    delete[] key_list->values;
    delete key_list;
}

//Sequential search on disk
void disk_seq_search(List* seek_list, const char* key_file_name)
{
    int hit[seek_list->length];
    
    ifstream key_file;
    key_file.open(key_file_name, ios::binary | ios::in);
    if (!(key_file.is_open()))
    {
        // Print an error and exit
        cerr << "Uh oh, " << key_file_name << " could not be opened for writing!" << endl;
        exit(1);
    }
    
    timeval tm_start;
    gettimeofday( &tm_start, NULL );
    for(int i = 0; i < seek_list->length; i++)
    {
        int seek = seek_list->values[i];
        bool found = false;
        if(key_file.eof())
        {
            key_file.clear();
        }
        key_file.seekg(0, ios::beg);
        int buffer;
        while(key_file.read((char*) &buffer, sizeof(int)))
        {
            if(seek == buffer)
            {
                found = true;
                break;
            }
        }
        
        if(found)
        {
            hit[i] = 1;
        }
        else
        {
            hit[i] = 0;
        }
    }
    timeval tm_end;
    gettimeofday( &tm_end, NULL );
    key_file.close();
    print_result(tm_start, tm_end, seek_list, hit);
}

//Binary search on disk
bool disk_bin_search_action(long start, long end, int seek, ifstream &key_file)
{
    if (start > end)
    {
        return false;
    }
    else
    {
        long mid = (start + end)/2;
        
        int key = 0;
        
        if(key_file.eof())
        {
            key_file.clear();
        }
        key_file.seekg(mid * sizeof(int), ios::beg);
        key_file.read((char*) &key, sizeof(int));
        
        if (key == seek)
        {
            return true;
        }
        else if (key < seek)
        {
            return disk_bin_search_action(mid + 1, end, seek, key_file);
        }
        else
        {
            return disk_bin_search_action(start, mid - 1, seek, key_file);
        }
    }
}

void disk_bin_search(List* seek_list, const char* key_file_name)
{
    int hit[seek_list->length];
    
    ifstream key_file;
    key_file.open(key_file_name, ios::binary | ios::in);
    if (!(key_file.is_open()))
    {
        // Print an error and exit
        cerr << "Uh oh, " << key_file_name << " could not be opened for writing!" << endl;
        exit(1);
    }
    
    //Get the number of keys
    long key_count = getFileSize(key_file_name)/sizeof(int);
    
    timeval tm_start;
    gettimeofday( &tm_start, NULL );
    for(int i = 0; i < seek_list->length; i++)
    {
        int seek = seek_list->values[i];
        bool found = false;
        long start = 0;
        long end = key_count - 1;
        
        found = disk_bin_search_action(start, end, seek, key_file);
        
        /* //iteration version of binary search
        while(start <= end)
        {
            int mid = (start + end)/2;
            if(key_file.eof())
            {
                key_file.clear();
            }
            key_file.seekg(mid * sizeof(int), ios::beg);
            key_file.read((char*) &buffer, sizeof(int));
            
            if (buffer == seek)
            {
                found = true;
                break;
            }
            else if (buffer < seek)
            {
                start = mid + 1;
            }
            else
            {
                end = mid - 1;
            }
        }*/
        
        if(found)
        {
            hit[i] = 1;
        }
        else
        {
            hit[i] = 0;
        }
    }
    timeval tm_end;
    gettimeofday( &tm_end, NULL );
    key_file.close();
    print_result(tm_start, tm_end, seek_list, hit);
}

//Print usage information
void usage()
{
    cerr << "Usage: assn_1 --[mem-lin|mem-bin|disk-lin|disk-bin] key_file seek_file" << endl;
    exit(1);
}

//Parse command-line arguments
Option * parse(int argc, const char * argv[])
{
    Option* option = new Option();
    
    if (argc < 4)
    {
        usage();
    }
    
    if (strlen(argv[1]) == MEMORY_OPTION_LENGTH)
    {
        option->is_mem = true;
        if (strncmp(argv[1], MEMORY_BINARY_OPTION, MEMORY_OPTION_LENGTH) == 0)
        {
            option->is_lin = false;
        }
        else if (strncmp(argv[1], MEMORY_LINEAR_OPTION, MEMORY_OPTION_LENGTH) == 0)
        {
            option->is_lin = true;
        }
        else
        {
            usage();
        }
    }
    else if (strlen(argv[1]) == DISK_OPTION_LENGTH)
    {
        option->is_mem = false;
        if (strncmp(argv[1], DISK_BINARY_OPTION, DISK_OPTION_LENGTH) == 0)
        {
            option->is_lin = false;
        }
        else if (strncmp(argv[1], DISK_LINEAR_OPTION, DISK_OPTION_LENGTH) == 0)
        {
            option->is_lin = true;
        }
        else
        {
            usage();
        }
    }
    else
    {
        usage();
    }
    
    return option;
}


//Entry function
int main(int argc, const char * argv[]) {
    
    Option* option = parse(argc, argv);
    
    const char* seek_file_name = argv[3];
    const char* key_file_name = argv[2];
    long seek_count = getFileSize(seek_file_name)/sizeof(int);
    List* seek_list = readFile(seek_file_name, seek_count);
    long key_count = getFileSize(key_file_name)/sizeof(int);
    
    if(option->is_mem)
    {
        if(option->is_lin)
        {
            mem_seq_search(seek_list, key_file_name, key_count);
        }
        else
        {
            mem_bin_search(seek_list, key_file_name, key_count);
        }
    }
    else
    {
        if(option->is_lin)
        {
            disk_seq_search(seek_list, key_file_name);
        }
        else
        {
            disk_bin_search(seek_list, key_file_name);
        }
    }
    
    delete option;
    delete[] seek_list->values;
    delete seek_list;
    
    return 0;
    
}

