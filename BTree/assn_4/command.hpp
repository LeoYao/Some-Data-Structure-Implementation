//
//  command.hpp
//  assn_4
//
//  Created by Leo on 11/8/15.
//  Copyright © 2015 Haiyu Yao. All rights reserved.
//

#ifndef command_hpp
#define command_hpp

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

using namespace std;

namespace assn_4 {
    
    class Command
    {
    public:
        Command();
        void set_add();
        bool is_add();
        void set_end();
        bool is_end();
        void set_find();
        bool is_find();
        void set_print();
        bool is_print();
        void set_key(string key);
        int get_key();
        
        //Parse commands
        static Command * readCommand()
        {
            Command* cmd = new Command();
            
            string cmd_str;
            getline(cin, cmd_str);
            stringstream cmd_ss(cmd_str);
            string buf;
            bool parseOrder = true;
            int cmd_pos = 0;
            while (std::getline(cmd_ss, buf, ' '))
            {
                if (buf.length() > 0)
                {
                    const char* char_buf = buf.c_str();
                    if (cmd_pos == 0)
                    {
                        parseOrder = false;
                        if (strlen(char_buf) == 3)
                        {
                            if (strncmp(char_buf, "add", 3) == 0)
                            {
                                cmd->set_add();
                            }
                            else if(strncmp(char_buf, "end", 3) == 0)
                            {
                                cmd->set_end();
                            }
                        }
                        else if (strlen(char_buf) == 4)
                        {
                            if(strncmp(char_buf, "find", 4) == 0)
                            {
                                cmd->set_find();
                            }
                        }
                        else if (strlen(char_buf) == 5)
                        {
                            if(strncmp(char_buf, "print", 5) == 0)
                            {
                                cmd->set_print();
                            }
                        }
                        else
                        {
                            cerr << "Invalid command! Only add, del, find and end are valid." << endl;
                            exit(1);
                        }
                    }
                    else if (cmd_pos == 1)
                    {
                        if (cmd->is_add() || cmd->is_find())
                        {
                            cmd->set_key(buf);
                        }
                        else
                        {
                            cerr << "Invalid command! Only add, del, find can have the 2nd parameter." << endl;
                            exit(1);
                        }
                    }
                    else if (cmd_pos > 1)
                    {
                        cerr << "Invalid command! No 3rd parameter." << endl;
                        exit(1);
                        
                    }
                    
                    cmd_pos++;
                }
            }
            
            if (cmd_pos < 2 && (cmd->is_add() || cmd->is_find()))
            {
                cerr << "Invalid command! Usage: [add|find] key" << endl;
                exit(1);
            }
            return cmd;
        }
        
    private:
        bool add_;
        bool find_;
        bool end_;
        bool print_;
        int key_;
        
        void reset_flags();
    };
}

#endif /* command_hpp */
