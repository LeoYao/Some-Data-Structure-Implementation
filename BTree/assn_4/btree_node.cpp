//
//  btree_node.cpp
//  assn_4
//
//  Created by Leo on 11/6/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#include "btree_node.hpp"

using namespace std;

namespace assn_4 {
    BTreeNode::BTreeNode(int order)
    {
        order_ = order;
        keys_ = new int[order_ - 1];
        children_ = new long[order_];
        key_count_ = 0;
        
        for (int i = 0; i < order_ - 1; i++)
        {
            keys_[i] = -1;
        }
        
        for (int i = 0; i < order_; i++)
        {
            children_[i] = -1;
        }
    }
    
    BTreeNode::~BTreeNode()
    {
        delete[] keys_;
        delete[] children_;
    }
    
    bool BTreeNode::is_leaf()
    {
        return children_[0] == -1;
    }
    
    
    void BTreeNode::set_keys(int* keys, int key_count)
    {
        key_count_ = key_count;
        for (int i = 0; i < key_count; i++)
        {
            keys_[i] = keys[i];
        }
        
        for (int i = 0; i < order_ ;i++)
        {
            children_[i] = -1;
        }
    }
    
    void BTreeNode::set_children(long* children, int child_count)
    {
        for (int i = 0; i < child_count; i++)
        {
            children_[i] = children[i];
        }
    }
    
    long* BTreeNode::get_children()
    {
        return children_;
    }
    
    int* BTreeNode::get_keys()
    {
        return keys_;
    }
    
    int BTreeNode::get_key_count()
    {
        return key_count_;
    }
    
    void BTreeNode::print()
    {
        for (int i = 0; i < key_count_; i++)
        {
            cout << keys_[i];
            if (i < key_count_ - 1)
            {
                cout << ",";
            }
        }
        cout << " ";
    }
    
}