//
//  command.cpp
//  assn_4
//
//  Created by Leo on 11/8/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#include "command.hpp"

namespace assn_4 {
    Command::Command()
    {
        add_ = false;
        find_ = false;
        end_ = false;
        key_ = 0;
    }
    
    void Command::set_add()
    {
        reset_flags();
        add_ = true;
    }
    
    bool Command::is_add()
    {
        return add_;
    }
    
    void Command::set_end()
    {
        reset_flags();
        end_ = true;
    }
    
    bool Command::is_end()
    {
        return end_;
    }
    
    void Command::set_find()
    {
        reset_flags();
        find_ = true;
    }
    
    bool Command::is_find()
    {
        return find_;
    }
    
    void Command::set_print()
    {
        reset_flags();
        print_ = true;
    }
    bool Command::is_print()
    {
        return print_;
    }
    
    void Command::set_key(string key)
    {
        this->key_ = atoi(key.c_str());
    }
    
    int Command::get_key()
    {
        return key_;
    }
    
    void Command::reset_flags()
    {
        key_ = 0;
        add_ = false;
        find_ = false;
        end_ = false;
        print_ = false;
        
    }
}