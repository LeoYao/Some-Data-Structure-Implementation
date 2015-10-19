//
//  main.cpp
//  assn_3
//
//  Created by Leo on 10/16/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <sys/stat.h>
#include <ios>
#include <sys/time.h>

#include "common.h"
#include "heap.h"
#include "basic_merge_sort.h"
#include "output_buffer_manager.h"
#include "run_file_mgr.h"
#include "multistep_merge_sort.h"
#include "repl_sel_merge_sort.h"
#include "composite_heap.h"

using namespace assn_3;

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
    cout << "Time: " << sec << "." << setw(6) << setfill('0') << usec <<endl;
}

struct Option
{
    bool basic;
    bool multistep;
    bool replacement;
};


static const char *BASIC_OPTION = "--basic";
static const char *MULTISTEP_OPTION = "--multistep";
static const char *REPLACEMENT_OPTION = "--replacement";

static const int BASIC_OPTION_LENGTH = strlen("--basic");
static const int MULTISTEP_OPTION_LENGTH = strlen("--multistep");
static const int REPLACEMENT_OPTION_LENGTH = strlen("--replacement");


//Print usage information
void usage()
{
    cerr << "Usage: assn_3 --[basic|multistep|replacement] input.bin sort.bin" << endl;
    exit(1);
}

//Parse command-line arguments
Option * parse(int argc, const char * argv[])
{
    Option* option = new Option();
    
    if (argc < 3)
    {
        usage();
    }
    
    option->basic = false;
    option->multistep = false;
    option->replacement = false;
    
    if (strlen(argv[1]) == BASIC_OPTION_LENGTH)
    {
        if (strncmp(argv[1], BASIC_OPTION, BASIC_OPTION_LENGTH) == 0)
        {
            option->basic = true;
            return option;
        }
    }
    
    if (strlen(argv[1]) == MULTISTEP_OPTION_LENGTH)
    {
        if (strncmp(argv[1], MULTISTEP_OPTION, MULTISTEP_OPTION_LENGTH) == 0)
        {
            option->multistep = true;
            return option;
        }
    }
    
    if (strlen(argv[1]) == REPLACEMENT_OPTION_LENGTH)
    {
        if (strncmp(argv[1], REPLACEMENT_OPTION, REPLACEMENT_OPTION_LENGTH) == 0)
        {
            option->replacement = true;
            return option;
        }
    }
    
    usage();
    
    return option;
}

int main(int argc, const char * argv[]) {
   
    Option* option = parse(argc, argv);
    const char* input = argv[2];;
    const char* output = argv[3];;
    
    timeval tm_start;
    gettimeofday( &tm_start, NULL );
    

    if (option->basic)
    {
        BasicMergeSort basic_merge_sort(input, output, 1000, 1000);
        
#ifdef MONITOR
        timeval tm_mid1;
        gettimeofday( &tm_mid1, NULL );
        print_time(tm_start, tm_mid1);
#endif
        
        basic_merge_sort.generateRuns();
        
#ifdef MONITOR
        timeval tm_mid2;
        gettimeofday( &tm_mid2, NULL );
        print_time(tm_mid1, tm_mid2);
#endif
        
        basic_merge_sort.merge();
#ifdef MONITOR
        timeval tm_mid3;
        gettimeofday( &tm_mid3, NULL );
        print_time(tm_mid2, tm_mid3);
#endif
        
    }
    else if (option->multistep)
    {
        MultistepMergeSort multistep_merge_sort(input, output, 1000, 1000);
#ifdef MONITOR
        timeval tm_mid1;
        gettimeofday( &tm_mid1, NULL );
        print_time(tm_start, tm_mid1);
#endif
        
        multistep_merge_sort.generateRuns();
#ifdef MONITOR
        timeval tm_mid2;
        gettimeofday( &tm_mid2, NULL );
        print_time(tm_mid1, tm_mid2);
#endif
        
        multistep_merge_sort.generateSuperRuns(15);
#ifdef MONITOR
        timeval tm_mid3;
        gettimeofday( &tm_mid3, NULL );
        print_time(tm_mid2, tm_mid3);
#endif
        
        multistep_merge_sort.merge();
#ifdef MONITOR
        timeval tm_mid4;
        gettimeofday( &tm_mid4, NULL );
        print_time(tm_mid3, tm_mid4);
#endif
        
    }
    else
    {
        ReplacementSelectionMergeSort repl_sel_merge_sort(input, output, 1000, 1000);
#ifdef MONITOR
        timeval tm_mid1;
        gettimeofday( &tm_mid1, NULL );
        print_time(tm_start, tm_mid1);
#endif
        
        repl_sel_merge_sort.generateRuns(750);
#ifdef MONITOR
        timeval tm_mid2;
        gettimeofday( &tm_mid2, NULL );
        print_time(tm_mid1, tm_mid2);
#endif
        
        repl_sel_merge_sort.merge();
#ifdef MONITOR
        timeval tm_mid3;
        gettimeofday( &tm_mid3, NULL );
        print_time(tm_mid2, tm_mid3);
#endif
    }
    
    timeval tm_end;
    gettimeofday( &tm_end, NULL );
    print_time(tm_start, tm_end);

    return 0;
}

