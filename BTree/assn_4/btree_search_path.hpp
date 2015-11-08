//
//  btree_search_path.hpp
//  assn_4
//
//  Created by Leo on 11/6/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef btree_search_path_hpp
#define btree_search_path_hpp

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
#include "btree_node.hpp"

namespace assn_4 {
    class BTreeSearchPathNode
    {
    public:
        BTreeSearchPathNode(long offset, BTreeNode* btree_node, BTreeSearchPathNode* next);
        ~BTreeSearchPathNode();
        BTreeSearchPathNode* get_next();
        BTreeNode* get_node();
        long get_offset();
    private:
        long offset_;
        BTreeNode* btree_node_;
        BTreeSearchPathNode* next_;
    };
    
    class BTreeSearchPath
    {
    public:
        BTreeSearchPath();
        ~BTreeSearchPath();
        void add(long offset, BTreeNode* btree_node_);
        BTreeSearchPathNode* get_lastest_path_node();
    private:
        BTreeSearchPathNode* head_;
    };
    
    
}

#endif /* btree_search_path_hpp */
