//
//  btree_search_path.cpp
//  assn_4
//
//  Created by Leo on 11/6/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#include "btree_search_path.hpp"

namespace assn_4 {

    /***********************
     * BTreeSearchPathNode *
     ***********************/
    BTreeSearchPathNode::BTreeSearchPathNode(long offset, BTreeNode* btree_node, BTreeSearchPathNode* next)
    {
        offset_ = offset;
        btree_node_ = btree_node;
        next_ = next;
    }
    
    BTreeSearchPathNode::~BTreeSearchPathNode()
    {
        //delete btree_node_;
    }
    
    BTreeSearchPathNode* BTreeSearchPathNode::get_next()
    {
        return next_;
    }
    
    BTreeNode* BTreeSearchPathNode::get_node()
    {
        return btree_node_;
    }
    
    long BTreeSearchPathNode::get_offset()
    {
        return offset_;
    }
    
    /*******************
     * BTreeSearchPath *
     *******************/
    BTreeSearchPath::BTreeSearchPath()
    {
        head_ = __null;
    }
    
    BTreeSearchPath::~BTreeSearchPath()
    {
        while(head_ != __null)
        {
            BTreeSearchPathNode* next = head_->get_next();
            BTreeNode* btree_node = head_->get_node();
            if (next != __null) //Keep root node
            {
                delete btree_node;
            }
            delete head_;
            head_ = next;
        }
    }
    
    void BTreeSearchPath::add(long offset, BTreeNode* btree_node)
    {
        BTreeSearchPathNode* new_path_node = new BTreeSearchPathNode(offset, btree_node, head_);
        head_ = new_path_node;
    }
    
    BTreeSearchPathNode* BTreeSearchPath::get_lastest_path_node()
    {
        if (head_ == __null)
        {
            return __null;
        }
        BTreeSearchPathNode* latest_path_node = head_;
        head_ = head_->get_next();
        return latest_path_node;
    }
}