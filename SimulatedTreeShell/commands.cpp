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

   // If first character is a hash '#' ignore this commmand
   if (cmd[0] == '#') {
      return fn_comment;
   }
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
      inode_ptr next_node = NULL;
      if( state.get_cwd()->get_contents()->get_dirents().find(path[i]+'/') != state.get_cwd()->get_contents()->get_dirents().end() )
         next_node = state.get_cwd()->get_contents()->get_dirents()[path[i]+'/'];
      if(next_node!=NULL && next_node->get_contents()->is_directory()) // if the node exists and is a directory
         state.set_cwd(next_node);
      else {                                             // if the specified directory DNE
         return false;
      }
   }

   cout << "\n\n";
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

void fn_comment (inode_state& state, const wordvec& words) {
   // blank function
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   return;
}

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

   // Base Case: No pathname argument provided
   //    print the entries of cwd
   if (words.size() < 2) {
      for (auto &entry : state.get_cwd()->get_contents()->get_dirents()) {
         cout << setw(6) << setprecision(6) << left << entry.second->get_inode_nr() << "  "
              << setw(6) << setprecision(6) << left << entry.second->get_contents()->size() << "  " << entry.first << endl;
      }
   }

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
         auto loc = state.get_cwd()->get_contents()->get_dirents().find(path[j]);
         if (loc == state.get_cwd()->get_contents()->get_dirents().end()) {
            cout << path[j] << " not found in dirents." << endl; // @DELETE
            continue;
         }
         inode_ptr next_node = loc->second;
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

