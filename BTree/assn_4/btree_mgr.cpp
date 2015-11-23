//
//  btree_mgr.cpp
//  assn_4
//
//  Created by Leo on 11/6/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#include "btree_mgr.hpp"
#include "queue.hpp"
#include "btree_search_path.hpp"

using namespace std;

namespace assn_4 {
    
    /*******************
     * Public Methods *
     *******************/
    BTreeManager::BTreeManager(int order)
    {
        order_ = order;
        root_ = __null;
        root_offset_ = -1;
    }
    
    BTreeManager::~BTreeManager()
    {
        delete root_;
    }
    
    
    void BTreeManager::open(const char *file_name)
    {
        bool new_file = false;
        struct stat state_buffer;
        if (stat (file_name, &state_buffer) != 0)
        {
            index_file_.open(file_name, ios::in|ios::out|ios::binary|fstream::trunc);
            new_file = true;
        }
        else
        {
            index_file_.open(file_name, ios::in|ios::out|ios::binary);
        }
        
        if (!(index_file_.is_open()))
        {
            cerr << "Failed to open file " << file_name << "!" << endl;
            exit(1);
        }
        
        if (!new_file)
        {
            read_root();
        }
        else
        {
            //placeholder
            index_file_.write((char*)(&root_offset_), sizeof(long));
            index_file_.flush();
        }
    }
    
    void BTreeManager::close()
    {
        if (index_file_.is_open())
        {
            index_file_.clear();
            index_file_.seekg(0, fstream::beg);
            index_file_.write((char*)(&root_offset_), sizeof(long));
            index_file_.close();
        }
    }
    
    bool BTreeManager::find(int key)
    {
        return search(root_, root_offset_, key, __null);
    }
    
    bool BTreeManager::add(int key)
    {
        if (root_ == __null)
        {
            create_root(key, -1, -1);
            write_root();
            return true;
        }
        
        BTreeSearchPath search_path;
        
        bool exist = search(root_, root_offset_, key, &search_path);
        
        if (exist)
        {
            return false;
        }
        
        add_key(key, &search_path);
        
        return true;
    }
    
    void BTreeManager::print()
    {
        if (root_offset_ < 0)
        {
            //cerr << "Cannot print, because this is an empty index file." << endl;
            //exit(1);
            return;
        }
        Queue queue;
        queue.add(root_offset_);
        
        int level_no = 1;
        while(!queue.is_empty())
        {
            int size = queue.get_size();
            cout << setw(2) << setfill(' ') << level_no++ << ": ";
            for (int i = 0; i < size; i++)
            {
                long offset = queue.get();
                BTreeNode* node = read_btree_node(offset);
                node->print();
                if (!node->is_leaf())
                {
                    int child_count = node->get_key_count() + 1;
                    long* children = node->get_children();
                    for (int j = 0; j < child_count; j++)
                    {
                        queue.add(children[j]);
                    }
                }
                delete node;
                
            }
            cout << endl;
        }
        //cout << "Time: " << "." << setw(6) << setfill('0');
    }
    
    /*******************
     * Private Methods *
     *******************/
    
    void BTreeManager::add_key(int key, BTreeSearchPath* search_path)
    {
        int latest_key_to_add = key;
        int key_to_promote = -1;
        long left_child_offset = -1;
        long right_child_offset = -1;
        BTreeSearchPathNode* latest_path_node = search_path->get_lastest_path_node();
        
        bool add_complted = false;
        while(!add_complted)
        {
            if (latest_path_node == __null)
            {
                create_root(latest_key_to_add, left_child_offset, right_child_offset);
                write_root();
                add_complted = true;
                break;
            }
            
            BTreeNode* new_splited_node = new BTreeNode(order_);
            BTreeNode* latest_node = latest_path_node->get_node();
            
            bool need_to_promote = add_key_to_node(latest_key_to_add, latest_node, right_child_offset, &key_to_promote, new_splited_node);
            add_complted = !need_to_promote;
            
            left_child_offset = write(latest_path_node->get_offset(), latest_node);

            if (need_to_promote)
            {
                right_child_offset = write(-1, new_splited_node);
            }
            
            delete latest_path_node;
            delete new_splited_node;
            if (latest_node != root_ || need_to_promote)
            {
                delete latest_node;
            }
            
            if (need_to_promote)
            {
                latest_path_node = search_path->get_lastest_path_node();
                latest_key_to_add = key_to_promote;
            }
        }
    }
    
    
    void BTreeManager::create_root(int key, long left_child_offset, long right_child_offset)
    {
        int keys[1];
        long children[2];
        keys[0] = key;
        children[0] = left_child_offset;
        children[1] = right_child_offset;
        root_ = new BTreeNode(order_);
        root_->set_keys(keys, 1);
        root_->set_children(children, 2);
        root_offset_ = -1;
    }
    
    /*
    bool BTreeManager::pub_add_key_to_node(int key, BTreeNode* node, long right_child_offset, int* key_to_promote, BTreeNode* splited_node)
    {
        return add_key_to_node(key, node, right_child_offset, key_to_promote, splited_node);
    }*/
    
    bool BTreeManager::add_key_to_node(int key, BTreeNode* node, long right_child_offset, int* key_to_promote, BTreeNode* splited_node)
    {
        int curr_key_count = node->get_key_count();
        
        int aux_key_count = curr_key_count + 1;
        int aux_child_count = aux_key_count + 1;
        
        int aux_key_arr[aux_key_count];
        long aux_children_arr[aux_child_count];
        
        build_aux_arr(key, node, right_child_offset, aux_key_arr, aux_children_arr);
        
        if (aux_key_count < order_)
        {
            node->set_keys(aux_key_arr, aux_key_count);
            node->set_children(aux_children_arr, aux_child_count);
            return false;
        }
        
        //Need to split and promote
        int promoted_key_pos = aux_key_count / 2;
        *key_to_promote = aux_key_arr[promoted_key_pos];
        
        //Assign left node
        int left_key_count = promoted_key_pos;
        int left_keys[left_key_count];
        long left_children[left_key_count+1];
        
        for (int i = 0; i < left_key_count; i++)
        {
            left_keys[i] = aux_key_arr[i];
            left_children[i] = aux_children_arr[i];
        }
        left_children[left_key_count] = aux_children_arr[left_key_count];
        
        node->set_keys(left_keys, left_key_count);
        node->set_children(left_children, left_key_count + 1);
        
        //Assign right node
        int right_key_count = aux_key_count - promoted_key_pos - 1;
        int right_keys[right_key_count];
        long right_children[right_key_count+1];
        
        for (int i = 0; i + promoted_key_pos + 1 < aux_key_count; i++)
        {
            right_keys[i] = aux_key_arr[i + promoted_key_pos + 1];
            right_children[i] = aux_children_arr[i + promoted_key_pos + 1];
        }
        right_children[right_key_count] = aux_children_arr[aux_child_count - 1];
        
        //splited_node = new BTreeNode(order_);
        splited_node->set_keys(right_keys, right_key_count);
        splited_node->set_children(right_children, right_key_count + 1);
        
        return true;
    }
    
    void BTreeManager::build_aux_arr(int key, BTreeNode* node, long right_child_offset, int* aux_key_arr, long* aux_children_arr)
    {
        int key_count = node->get_key_count();
        int* curr_keys = node->get_keys();
        long* curr_children = node->get_children();
        
        int aux_key_arr_ind = 0;
        bool is_key_added = false;
        
        for (int i = 0; i < key_count; i++)
        {
            int tmp_key = curr_keys[i];
            long tmp_child = curr_children[i];
            if (tmp_key > key && !is_key_added)
            {
                aux_children_arr[aux_key_arr_ind] = tmp_child;
                aux_key_arr[aux_key_arr_ind] = key;
                aux_key_arr_ind++;
                tmp_child = right_child_offset;
                is_key_added = true;
            }
            
            aux_key_arr[aux_key_arr_ind] = tmp_key;
            aux_children_arr[aux_key_arr_ind] = tmp_child;
            aux_key_arr_ind++;
        }
        
        aux_children_arr[aux_key_arr_ind] = curr_children[key_count];
        if (!is_key_added)
        {
            aux_key_arr[aux_key_arr_ind] = key;
            aux_key_arr_ind++;
            aux_children_arr[aux_key_arr_ind] = right_child_offset;
        }
    }
    
    
    bool BTreeManager::search(BTreeNode* btree_node, long btree_node_offset, int key, BTreeSearchPath* search_path)
    {
        if (btree_node == __null)
        {
            return false;
        }
        
        if (search_path != __null)
        {
            search_path->add(btree_node_offset, btree_node);
        }
        
        int* keys = btree_node->get_keys();
        int key_count = btree_node->get_key_count();
        
        int key_offset = 0;
        while(key_offset < key_count && keys[key_offset] < key)
        {
            key_offset++;
        }
        
        bool result = false;
        if (key_offset < key_count && keys[key_offset] == key)
        {
            result = true;
        }
        else if (btree_node->is_leaf())
        {
            result = false;
        }
        else
        {
        
            long* children = btree_node->get_children();
            long child_offset_to_search = children[key_offset];
            
            BTreeNode* child = read_btree_node(child_offset_to_search);
            
            result = search(child, child_offset_to_search, key, search_path);
        }
        
        if (search_path == __null && btree_node != root_)
        {
            delete btree_node;
        }
        
        return result;
        
    }
    
    
    BTreeNode* BTreeManager::read_btree_node(long offset)
    {
        if (offset < 0)
        {
            cerr << "BTreeManager.read_btree_node: Invalid offset" << endl;
            exit(1);
        }
        index_file_.clear();
        index_file_.seekg(offset, fstream::beg);
        int key_count = 0;
        index_file_.read((char*)(&key_count), sizeof(int));
        int keys[order_-1];
        index_file_.read((char*)keys, sizeof(int)*(order_ - 1));
        long children[order_];
        index_file_.read((char*)children, sizeof(long)*order_);
        
        BTreeNode* btree_node = new BTreeNode(order_);
        btree_node->set_keys(keys, key_count);
        btree_node->set_children(children, key_count + 1);
        return btree_node;
    }
    
    long BTreeManager::write(long offset, BTreeNode* btree_node)
    {
        index_file_.clear();
        if (offset < 0)
        {
            index_file_.seekg(0, fstream::end);
            offset = index_file_.tellg();
        }
        else{
            index_file_.seekg(offset, fstream::beg);
        }
        
        int key_count = btree_node->get_key_count();
        index_file_.write((char*)(&key_count), sizeof(int));
        int* keys = btree_node->get_keys();
        index_file_.write((char*)keys, sizeof(int) * (order_ - 1));
        long* children = btree_node->get_children();
        index_file_.write((char*)children, sizeof(long) * order_);
        
        return offset;
    }
    
    long BTreeManager::write_root()
    {
        root_offset_ = write(root_offset_, root_);
        
        //index_file_.clear();
        //index_file_.seekg(0, fstream::beg);
        //index_file_.write((char*)(&root_offset_), sizeof(long));
        
        return root_offset_;
    }
    
    void BTreeManager::read_root()
    {
        index_file_.clear();
        index_file_.seekg(0, fstream::beg);
        index_file_.read((char*)(&root_offset_), sizeof(long));
        if (root_offset_ >= 0)
        {
            root_ = read_btree_node(root_offset_);
        }
    }
}






