//
//  command.h
//  assn_2
//
//  Created by Leo on 9/26/15.
//  Copyright (c) 2015 Haiyu Yao. All rights reserved.
//

#ifndef assn_2_command_h
#define assn_2_command_h


#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <sys/stat.h>
#include <ios>

using namespace std;

namespace assn_2 {
    
    class Command
    {
    public:
        int key;
        char* rec;
        
        Command()
        {
            add = false;
            del = false;
            find = false;
            end = false;
            key = __null;
            rec = __null;
        }
        
        ~Command()
        {
            if (rec != __null)
            {
                delete rec;
            }
        }
        
        void set_add()
        {
            reset_flags();
            add = true;
        }
        
        bool is_add()
        {
            return add;
        }
        
        void set_del()
        {
            reset_flags();
            del = true;
        }
        
        bool is_del()
        {
            return del;
        }
        
        void set_end()
        {
            reset_flags();
            end = true;
        }
        
        bool is_end()
        {
            return end;
        }
        
        void set_find()
        {
            reset_flags();
            find = true;
        }
        
        bool is_find()
        {
            return find;
        }
        
        void set_key(string key)
        {
            this->key = atoi(key.c_str());
        }
        
        void set_rec(string rec)
        {
            this->rec = new char[rec.length() + 1];
            for (int i = 0; i < rec.length(); i++)
            {
                this->rec[i] = rec[i];
            }
            this->rec[rec.length()] = '\0';
        }
        
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
            while (std::getline(cmd_ss, buf, ' ')) {
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
                            else if(strncmp(char_buf, "del", 3) == 0)
                            {
                                cmd->set_del();
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
                        else
                        {
                            cerr << "Invalid command! Only add, del, find and end are valid." << endl;
                            exit(1);
                        }
                    }
                    else if (cmd_pos == 1)
                    {
                        if (cmd->is_add() || cmd->is_del() || cmd->is_find())
                        {
                            cmd->set_key(buf);
                        }
                        else
                        {
                            cerr << "Invalid command! Only add, del, find can have the 2nd parameter." << endl;
                            exit(1);
                        }
                    }
                    else if (cmd_pos == 2)
                    {
                        if (cmd->is_add())
                        {
                            cmd->set_rec(buf);
                        }
                        else
                        {
                            cerr << "Invalid command! Only add can have the 3rd parameter." << endl;
                            exit(1);
                        }
                    }
                    
                    cmd_pos++;
                }
            }
            
            if (cmd->is_add())
            {
                if (cmd_pos < 3)
                {
                    cerr << "Invalid command! Usage: add key record" << endl;
                    exit(1);
                }
            }
            else if (cmd->is_del() || cmd->is_find())
            {
                if (cmd_pos < 2)
                {
                    cerr << "Invalid command! Usage: del|find key" << endl;
                    exit(1);
                }
            }
            return cmd;
        }
        
    private:
        bool add;
        bool del;
        bool find;
        bool end;
        
        void reset_flags()
        {
            add = false;
            del = false;
            find = false;
            end = false;
            
        }
    };
}


#endif
