//
//  btree_mgr.hpp
//  assn_4
//
//  Created by Leo on 11/6/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef btree_mgr_hpp
#define btree_mgr_hpp

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
#include "btree_search_path.hpp"

using namespace std;

namespace assn_4 {
    
    class BTreeManager
    {
    public:
        BTreeManager(int order);
        ~BTreeManager();
        
        void open(const char* file_name);
        void close();
        bool find(int key);
        bool add(int key);
        void print();
        
        /*bool pub_add_key_to_node(int key, BTreeNode* node, long right_child_offset, int* key_to_promote, BTreeNode* splitd_node);
        */
        
    private:
        BTreeNode* root_;
        long root_offset_;
        fstream index_file_;
        int order_;
        
        void add_key(int key, BTreeSearchPath* search_path);
        void create_root(int key, long left_child_offset, long right_child_offset);
        
        //return true if promotion is necessary; false otherwise
        bool add_key_to_node(int key, BTreeNode* node, long right_child_offset, int* key_to_promote, BTreeNode* splitd_node);
        
        void build_aux_arr(int key, BTreeNode* node, long right_child_offset, int* aux_key_arr, long* aux_children_arr);
        
        bool search(BTreeNode* btree_node, long btree_node_offset, int key, BTreeSearchPath* search_path);
        long write_root();
        long write(long offset, BTreeNode* btree_node);
        BTreeNode* read_btree_node(long offset);
        void read_root();
    };
}


#endif /* btree_mgr_hpp */
