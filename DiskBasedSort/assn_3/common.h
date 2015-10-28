//
//  common.h
//  assn_3
//
//  Created by Leo on 10/17/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

//#define MONITOR

#ifndef common_h
#define common_h

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

using namespace std;

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
    
    int* o1 = (int*) i1;
    int* o2 = (int*) i2;
    
    if (*o1 > *o2)
    {
        return 1;
    }
    else if (*o1 < *o2)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

char* duplicate_chars(const char* chars)
{
    long len = strlen(chars);
    char* result = new char[len+1];
    result[len] = '\0';
    memcpy(result, chars, len);
    return result;
}

long getFileSize(const char* file_name)
{
    ifstream infile;
    infile.open(file_name, ios::binary | ifstream::ate);
    if (!(infile.is_open()))
    {
        // Print an error and exit
        cerr << "RunFileManager.getFileSize: Uh oh, " << file_name << " could not be opened for reading!" << endl;
        exit(1);
    }
    
    ifstream::pos_type pos = infile.tellg();
    long size = (long)pos;
    infile.close();
    return size;
}


#endif /* common_h */
