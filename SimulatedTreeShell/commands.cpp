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
   cout << "in path_exists..\n";
   // loop through all strings in path
   for (int i=0; i<int(path.size()); ++i) {
      if (path[i] == ".") {// if "." is in path, ignore
         cout << "'.' found...\n";
         continue;
      }
      string path_to_find = path[i] + '/';
      //TODO: Make special case for .. backdirectory
      if (path[i] == "..") {// if "." is in path, go back one directory
         cout << "'..' found...\n";
         path_to_find = path[i];
      }
      
      inode_ptr next_node = NULL;
      if( state.get_cwd()->get_contents()->get_dirents().find(path_to_find) != state.get_cwd()->get_contents()->get_dirents().end() )
         next_node = state.get_cwd()->get_contents()->get_dirents()[path_to_find];
      if(next_node!=NULL && next_node->get_contents()->is_directory()) // if the node exists and is a directory
         state.set_cwd(next_node);
      else {                                             // if the specified directory DNE
         state.set_cwd(start);
         return false;
      }
   }

   cout << "\n\n";
   state.set_cwd(start); // set the cwd back to beginning
   return true;
}


/** get_subdir_at
 *  @brief: returns an inode_ptr to the subdirectory at a specified path
 *          functions behavior assumes that the subdirectory exists
 *          NOTE: Call path_exists before calling get_subdir_at
 *  @param: inode_ptr start - a pointer to the node to start looking for subdirectories
 *          wordvec& pathname - a vector of pathnames to traverse.
 **/
inode_ptr get_subdir_at(inode_ptr start, wordvec& pathname) {
  // Assert start not null
  if(!start) {
    cout << "Error: get_subdir_at: start is null\n";
    return start;
  } 
  // Start search with a pointer to start node
  inode_ptr curr = start;
  // Find the subdirectory that corresponds to element of pathname
  for (int i=0; i<int(pathname.size()); ++i) {
    // Special case: "."
    if(pathname[i] == ".") {
       cout << "'.' found\n"; //@DELETE
       continue;
    }
    string path_to_find = pathname[i] + '/';
    // Special case: ".."
    if(pathname[i] == "..") {
       cout << "'..' found\n"; //@DELETE
       path_to_find = pathname[i];
    }
    // Assert that curr is a directory
    if (!curr->get_contents()->is_directory()) {
      cout << "Error in get_subdir_at: pathname: " << path_to_find << " DNE.\n";
      return nullptr;
    }
    cout << "Line: " << __LINE__ << endl;
    // Assert that the entry at the next directory exists
    if (curr->get_contents()->get_dirents().find(path_to_find)==curr->get_contents()->get_dirents().end()) {
      cout << "Error in get_subdir_at: path_to_find: " << path_to_find << " DNE.\n";
      return curr;
    }
    // Move current node to the next directory entry
    cout << "Line: " << __LINE__ << endl;
    curr = curr->get_contents()->get_dirents().find(path_to_find)->second;
    cout << "curr: " << curr << " typeid(curr): " << typeid(curr).name() << endl;

  }
  return curr;
}


void fn_comment (inode_state& state, const wordvec& words) {
   // blank function
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   return;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Base case: No pathname specified
   if (words.size() < 2) {
      cout << "cat: error: No pathname specified\n";
      return;
   }

   // Loop through arguments
   for (int i=1; i<int(words.size()); i++) {
      cout << "state: " <<  state << " line: " << __LINE__ << endl;
      //    parse pathname
      wordvec pathname = split(words[i], "/");
      cout << "pathname: " << pathname << endl;//@DELETE
      string filename = pathname[pathname.size()-1];
      // Error condition: file is a directory
      if (path_exists(state, pathname)) {
         cout << "cat: " << words[i] << ": Is a directory.\n";
         continue;
      }

      cout << "state: " <<  state << " line: " << __LINE__ << endl;
      // Error condition: file does not exist
      wordvec dir_name = pathname;
      dir_name.pop_back();                      // remove the filename from directoryname
      cout << "dir_name: " << dir_name << endl;//@DELETE
      if (!path_exists(state,dir_name)) {       // check parent directory
         cout << "cat: " << words[i] << ": No such file or directory\n";
         continue;
      }
      cout << "state: " <<  state << " line: " << __LINE__ << endl;
      // Check parent directory for the file
      inode_ptr parent_dir = get_subdir_at(state.get_cwd(), dir_name);
      cout << "parent_dir: " << parent_dir << endl;
      if (parent_dir->get_contents()->get_dirents().find(filename) == parent_dir->get_contents()->get_dirents().end()) {
         cout << "cat: " << words[i] << ": No such file or directory\n";
         cout << "state: " <<  state << " line: " << __LINE__ << endl;
         continue;
      }
      cout << "state: " <<  state << " line: " << __LINE__ << endl;
      // Print contents of file
      inode_ptr file = parent_dir->get_contents()->get_dirents().find(filename)->second;
      cout << file->get_contents()->readfile() << endl;
   }

}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Assert that there are less than 2 arguments
   if (words.size() > 2) {
      cout << "cd: too many arguments\n";
      return;
   }

   // Case: no arguments - cd into root
   if(words.size() < 2) {
      state.set_cwd(state.get_root());
      return;
   }

   // Split argument into path
   wordvec path = split(words[1], "/");

   // Assert that path exists
   if (!path_exists(state, path)) {
      // error
      cout << "cd: " << words[1] << ": No such file or directory\n";
      return;
   }
   // go into each directory specified by path
   inode_ptr new_dir = state.get_cwd();
   new_dir = get_subdir_at(new_dir, path);

   state.set_cwd(new_dir);
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
   //    print the entries of cwd - if ".." or "." print a "/" too
   if (words.size() < 2) {
      string this_dir = state.get_cwd()->get_contents()->get_path();
      cout << ((this_dir == "") ? ("/") : (this_dir)) << ":" << endl; // print the path header
      for (auto &entry : state.get_cwd()->get_contents()->get_dirents()) {
         cout << setw(6) << setprecision(6) << entry.second->get_inode_nr() << "  "          // print the inode number
              << setw(6) << setprecision(6) << entry.second->get_contents()->size() << "  "  // print the size
              << entry.first << ((entry.first=="." || entry.first=="..") ? ("/") : (""))     // print the entry
              << endl;
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

   // Precondition: words is at least size 2
   if (words.size() < 2) {
      cout << words[0] << ": missing operand\n";
      return;
   }
   // Create file in pathname words[1]
   //    parse pathname
   wordvec pathname = split(words[1], "/");
   cout << "pathname: " << pathname << endl;//@DELETE
   //    separate filename from pathname
   string filename = pathname[pathname.size()-1];
   cout << "filename: " << filename << endl;//@DELETE
   pathname.pop_back(); 
   cout << "pathname: " << pathname << endl;//@DELETE
   //    assert pathname exists
   bool path_good = path_exists(state, pathname);
   cout << "path_good: " << path_good << endl;//@DELETE
   if(path_good) {
      //    mkfile with name filename in directory pathname
      inode_ptr dir = get_subdir_at(state.get_cwd(),pathname);   // get a pointer to the directory specified
      cout << "dir: " << dir << endl; //@DELETE
      inode_ptr new_node = dir->get_contents()->mkfile(filename);
      // Assert that it is not a directory
      if (new_node->get_contents()->is_directory()) {
         cout << words[0] << ": " << filename << "is a directory\n";
         return;
      }
      new_node->get_contents()->writefile(wordvec(words.begin()+2, words.end()));
      cout << "wordvec(words.begin()+2, words.end()): " << wordvec(words.begin()+2, words.end()) << endl;
   }
   return;
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
   // combine words into a string
   string new_prompt = "";
   for (int i=1; i<int(words.size()); i++) {
      new_prompt += words[i];
   }
   cout << "new_prompt: " << new_prompt << endl;
   state.set_prompt(new_prompt);
   return;
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << state.get_cwd()->get_contents()->get_path() << endl;
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

