//
//  main.cpp
//  assn_4
//
//  Created by Leo on 11/6/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <sys/stat.h>
#include <ios>

#include "btree_node.hpp"
#include "btree_mgr.hpp"
#include "btree_search_path.hpp"
#include "queue.hpp"
#include "command.hpp"

using namespace assn_4;
using namespace std;

int main(int argc, const char * argv[]) {

    const char* file_name = argv[1];
    int order = atoi(argv[2]);
    
    BTreeManager btree_mgr(order);
    btree_mgr.open(file_name);
    
    while(Command* cmd = Command::readCommand())
    {
        if (cmd->is_end())
        {
            btree_mgr.close();
            break;
        }
        else if (cmd->is_add())
        {
            bool result = btree_mgr.add(cmd->get_key());
            if (!result)
            {
                cout << "Entry with key=" << cmd->get_key() << " already exists" << endl;
            }
        }
        else if (cmd->is_find())
        {
            bool result = btree_mgr.find(cmd->get_key());
            if (result)
            {
                cout << "Entry with key=" << cmd->get_key() << " exists" << endl;
            }
            else
            {
                cout << "Entry with key=" << cmd->get_key() << " does not exist" << endl;
            }
        }
        else if (cmd->is_print())
        {
            btree_mgr.print();
        }
        delete cmd;
    }

    return 0;
}
