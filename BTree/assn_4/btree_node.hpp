//
//  btree_node.hpp
//  assn_4
//
//  Created by Leo on 11/6/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef btree_node_hpp
#define btree_node_hpp

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


namespace assn_4 {
    
    class BTreeNode
    {
    public:
        BTreeNode(int order);
        ~BTreeNode();
        
        bool is_leaf();
        int* get_keys();
        void set_keys(int* keys, int key_count);
        void set_children(long* children, int child_count);
        long* get_children();
        int get_key_count();
        void print();
        
    private:
        int order_;
        int* keys_;
        long* children_;
        int key_count_;
        int child_count_;
        
    };
}
#endif /* btree_node_hpp */
