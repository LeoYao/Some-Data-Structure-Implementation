//
//  main.cpp
//  assn_2
//
//  Created by Leo on 9/10/15.
//  Copyright (c) 2015 Haiyu Yao. All rights reserved.
//

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
#include "file_mgr.h"
#include "command.h"

using namespace std;

using namespace assn_2;

struct Option
{
    bool first_fit;
    bool best_fit;
    bool worst_fit;
};


static const char *FIRST_FIT_OPTION = "--first-fit";
static const char *BEST_FIT_OPTION = "--best-fit";
static const char *WORST_FIT_OPTION = "--worst-fit";

static const int FIRST_FIT_OPTION_LENGTH = strlen("--first-fit");
static const int BEST_FIT_OPTION_LENGTH = strlen("--best-fit");
static const int WORST_FIT_OPTION_LENGTH = strlen("--worst-fit");


//Print usage information
void usage()
{
    cerr << "Usage: assn_2 --[first-fit|best-fit|worst-fit] studentfile-name" << endl;
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
    
    option->first_fit = false;
    option->best_fit = false;
    option->worst_fit = false;
    
    if (strlen(argv[1]) == FIRST_FIT_OPTION_LENGTH)
    {
        if (strncmp(argv[1], FIRST_FIT_OPTION, FIRST_FIT_OPTION_LENGTH) == 0)
        {
            option->first_fit = true;
            return option;
        }
    }
    
    if (strlen(argv[1]) == BEST_FIT_OPTION_LENGTH)
    {
        if (strncmp(argv[1], BEST_FIT_OPTION, BEST_FIT_OPTION_LENGTH) == 0)
        {
            option->best_fit = true;
            return option;
        }
    }
    
    if (strlen(argv[1]) == WORST_FIT_OPTION_LENGTH)
    {
        if (strncmp(argv[1], WORST_FIT_OPTION, WORST_FIT_OPTION_LENGTH) == 0)
        {
            option->worst_fit = true;
            return option;
        }
    }
    
    usage();
    
    return option;
}

int main(int argc, const char * argv[]) {
    Option* option = parse(argc, argv);
    const char* datafile = argv[2];
    FileMgr* file_mgr = __null;
    
    if (option->first_fit)
    {
        file_mgr = new FileMgr(FileMgr::FIRST_FIT);
    }
    else if (option->best_fit)
    {
        file_mgr = new FileMgr(FileMgr::BEST_FIT);
    }
    else
    {
        file_mgr = new FileMgr(FileMgr::WORST_FIT);
    }
    
    file_mgr->open(datafile);
    
    while(Command* cmd = Command::readCommand())
    {
        if (cmd->is_end())
        {
            file_mgr->close();
            break;
        }
        else if (cmd->is_add())
        {
            file_mgr->add(cmd->key, cmd->rec);
        }
        else if (cmd->is_del())
        {
            file_mgr->del(cmd->key);
        }
        else if (cmd->is_find())
        {
            file_mgr->find(cmd->key);
        }
        
        delete cmd;
    }
        
    delete file_mgr;
    
    
    return 0;
}

