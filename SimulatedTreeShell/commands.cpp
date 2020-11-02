// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $

#include "commands.h"
#include "debug.h"
#include "typeinfo"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

/** path_exists
 *  @brief: returns true if directory path exists within filesystem.
 *  @param: state - the state object
 *          path - the list of directories
 **/
bool path_exists(inode_state& state, const wordvec& path) {
   inode_ptr start = state.get_cwd();                    // save cwd so that it can be returned

   // loop through all strings in path
   for (int i=0; i<int(path.size()); ++i) {
      //cout << "checking for subdirectory " << path[i] << "...\n";
      inode_ptr next_node = state.get_cwd()->get_contents()->get_dirents()[path[i]];
      //cout << "typeid(next_node->get_contents()->is_directory()): "  << (typeid(next_node->get_contents()->is_directory()).name()) << ", typeid(file_type::DIRECTORY_TYPE): " << (typeid(file_type::DIRECTORY_TYPE).name()) << endl;
      if(next_node!=NULL && next_node->get_contents()->is_directory()) // if the node exists and is a directory
         state.set_cwd(next_node);
      else {                                             // if the specified directory DNE
         //cout << path[i] << " DNE.\n";
         return false;
      }
   }

   state.set_cwd(start); // set the cwd back to beginning
   return true;
}

// /** path_exists
//  *  @brief: returns true if directory path exists as child of root.
//  *  @param: root - the root node
//  *          path - the list of directories
//  **/
// bool path_exists(inode_ptr& root, const wordvec& path) {
//    inode_ptr start = root;                    // save cwd so that it can be returned

//    // loop through all strings in path
//    for (int i=0; i<int(path.size()); ++i) {

//    }
//    return true;
// }

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   cout << "state: " << state << endl;

}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // If less than 2 words, complain
   if (words.size() < 2) {
      cout << words[0] << ": missing operand" << endl;
      return;
   }

   // For all directory arguments specified in words
   for (int i=1; i< int(words.size()); i++) {
      // TODO: check if path is absolute (starts with '/') or relative:
      //       if words[i] is an absolute pathname:
      //          error if path already exists
      //          error if parent doesn't exist
      //       // Assume relative
      //       
      //       if words[i] is a relative pathname:
      //          error if parent doesn't exist

      // TODO: if path does not start with '/':
      //       append current working directory

      // Second argument should be directory. Split by delimeter '/'
      wordvec path = split(words[i], "/");

      //cout << "fn_mkdir: path: " << path << endl;
      //cout << "path_exists(state, path): " << path_exists(state, path) << endl;

      // Check if path already exists
      if (path_exists(state, path)) {
         // error
         cout << "mkdir: cannot create directory '" << words[i] << "': File exists\n";
         continue;
      }

      // Check if parent path exists
      wordvec parent_path (path.begin(), path.end()-1);
      cout << "parent_path: " << parent_path << endl;
      if (!path_exists(state, parent_path)) {
         cout << "mkdir: cannot create directory '" << words[i] << "': No such file or directory\n";
         continue;
      }

      // Make new directory in cwd
      inode_ptr new_node;
      inode_ptr start = state.get_cwd();                    // save cwd so that it can be returned
      // loop through all strings in path
      for (int j=0; j<int(path.size()); ++j) {
         inode_ptr next_node = state.get_cwd()->get_contents()->get_dirents()[path[j]];
      }
      new_node = state.get_cwd()->get_contents()->mkdir(path[path.size()-1]); // create the new node
      state.set_cwd(start); // set the cwd back to beginning
      DEBUGF ('c', new_node);

      cout << "state.get_cwd()->get_contents()->get_dirents(): " << state.get_cwd()->get_contents()->get_dirents() << endl;
   }

   //map<string,inode_ptr> temp_map = new_node->get_contents()->get_dirents();
   //cout << "temp_map: " << temp_map << endl ;
   //cout << "temp_map.size(): " << temp_map.size() << endl;
   //
   return;
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << state.get_cwd()->get_contents()->get_path() <<endl;
   return;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

