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
   {"rmr"   , fn_rmr   },
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
   // save cwd so that it can be returned
   inode_ptr start = state.get_cwd();       
   // loop through all strings in path
   for (int i=0; i<int(path.size()); ++i) {
      // if "." is in path, ignore
      if (path[i] == ".") { continue; }     

      string path_to_find = path[i] + '/';
      // if "." is in path, go back one directory
      if (path[i] == "..") { path_to_find = path[i]; }
      
      inode_ptr next_node = NULL;
      auto entrs = state.get_cwd()->get_contents()->get_dirents();
      if (entrs.find(path_to_find) 
         != entrs.end())
         next_node = entrs[path_to_find];
      // if the node exists and is a directory
      if(next_node!=NULL && next_node->get_contents()->is_directory())
         state.set_cwd(next_node);
      else { // if the specified directory DNE 
         state.set_cwd(start);
         return false;
      }
   }
   // set the cwd back to beginning
   state.set_cwd(start);   
   return true;
}


/** get_subdir_at
 *  @brief: returns an inode_ptr to the subdirectory at a specifd path
 *          functions behavior assumes that the subdirectory exists
 *          NOTE: Call path_exists before calling get_subdir_at
 *  @param: inode_ptr start - a pointer to the node to start looking 
 *          wordvec& pathname - a vector of pathnames to traverse.
 **/
inode_ptr get_subdir_at(inode_ptr start, wordvec& pathname) {
  // Assert start not null
  if(!start) {
    return start;
  } 
  
  // Start search with a pointer to start node
  inode_ptr curr = start;
  
  // Find the subdirectory that corresponds to element of pathname
  for (int i=0; i<int(pathname.size()); ++i) {
    // Special case: "."
    if(pathname[i] == ".") {continue;}
    string path_to_find = pathname[i] + '/';
    
    // Special case: ".."
    if(pathname[i] == "..") {path_to_find = pathname[i];}
    
    // Save curr->get_contents() to save character space
    auto conts = curr->get_contents();

    // Assert that curr is a directory
    if (!conts->is_directory()) {return nullptr;}
    
    // Assert that the entry at the next directory exists
    if (conts->get_dirents().find(path_to_find)
      == conts->get_dirents().end()) {return curr;}
    
    // Move current node to the next directory entry
    curr = conts->get_dirents().find(path_to_find)->second;
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
      cerr << "cat: error: No pathname specified\n";
      return;
   }

   // Loop through arguments
   for (int i=1; i<int(words.size()); i++) {
      //    parse pathname
      wordvec pathname = split(words[i], "/");
      string filename = pathname[pathname.size()-1];
      // Error condition: file is a directory
      if (path_exists(state, pathname)) {
         cerr << "cat: " << words[i] << ": Is a directory.\n";
         continue;
      }
      // Error condition: file does not exist
      wordvec dir_name = pathname;
      dir_name.pop_back();               
      if (!path_exists(state,dir_name)) {
         cerr << "cat: " << words[i] 
              << ": No such file or directory\n";
         continue;
      }
      // Check parent directory for the file
      inode_ptr parent_dir = get_subdir_at(state.get_cwd(), dir_name);
      if (parent_dir->get_contents()->get_dirents().find(filename) 
         == parent_dir->get_contents()->get_dirents().end()) {
         cerr << "cat: " << words[i] 
              << ": No such file or directory\n";
         continue;
      }
      // Print contents of file
      auto conts = parent_dir->get_contents();
      inode_ptr file = conts->get_dirents().find(filename)->second;
      cout << file->get_contents()->readfile() << endl;
   }

}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Assert that there are less than 2 arguments
   if (words.size() > 2) {
      cerr << "cd: too many arguments\n";
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
      cerr << "cd: " << words[1] << ": No such file or directory\n";
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


void fn_exit (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

/** print_subdir
 *  @brief: helper fcn for ls
 *          print the contents of a specified directory
 *          assumes directory path is valid
 **/
void print_subdir (inode_state& state, const wordvec& pathname) {
   // Copy pathname
   wordvec dir_name = pathname;

   // Get the pointer to the subdirectory specified
   inode_ptr dir_ptr = get_subdir_at(state.get_cwd(), dir_name);

   // Get the path name
   string this_dir = dir_ptr->get_contents()->get_path();

   // print the path header
   cout << ((this_dir == "") ? ("/") : (this_dir)) << ":" << endl;
   for (auto &entry : dir_ptr->get_contents()->get_dirents()) {

      cout << setw(6) << setprecision(6) 
           << entry.second->get_inode_nr() << "  "        
           << setw(6) << setprecision(6) 
           << entry.second->get_contents()->size() << "  "
           << entry.first 
           << ((entry.first=="." || entry.first=="..") ? ("/") : (""))
           << endl;
   }
   return;
}

/** print_cwd
 *  @brief: helper fcn for ls and fn_lsr
 *          print the contents of state cwd
 **/
void print_cwd(inode_state& state) {
   string this_dir = state.get_cwd()->get_contents()->get_path();
   cout << ((this_dir == "") ? ("/") : (this_dir)) << ":" << endl;  
   for (auto &entry : state.get_cwd()->get_contents()->get_dirents()){
      cout << setw(6) << setprecision(6) 
           << entry.second->get_inode_nr() << "  "          
           << setw(6) << setprecision(6) 
           << entry.second->get_contents()->size() << "  "
           << entry.first 
           << ((entry.first=="." || entry.first=="..") ? ("/") : (""))  
           << endl;
   }
   return;
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

// print the entries of cwd - if ".." or "." print a "/" too
if (words.size() < 2) {
   string this_dir = state.get_cwd()->get_contents()->get_path();
   cout << ((this_dir == "") ? ("/") : (this_dir)) << ":" << endl;   
   for (auto &entry : state.get_cwd()->get_contents()->get_dirents()){
      cout << setw(6) << setprecision(6) 
           << entry.second->get_inode_nr() << "  "       
           << setw(6) << setprecision(6) 
           << entry.second->get_contents()->size() << "  " 
           << entry.first 
           << ((entry.first=="." || entry.first=="..") ? ("/") : (""))
           << endl;
   }
}
   // Loop through arguments
   for (int i=1; i<int(words.size()); i++) {
      wordvec pathname = split(words[i], "/");           
      print_subdir (state, pathname);   
   }                 
}

/** preorder_traversal
 *  @brief: helper function for lsr
 *          prints contents of state via preorder traversal
 *          recursively changes state
 **/
void preorder_traversal (inode_state& state) {
   // Print contents of current state cwd
   print_cwd(state);
   // Save cwd as start
   inode_ptr start = state.get_cwd();
   // Loop through cwd dirents
   for (auto &entry : state.get_cwd()->get_contents()->get_dirents()){
      // if entry isn't "." or ".." /and/ if entry is a directory
      if ( (entry.first != ".") and (entry.first != "..")  
         and (entry.second->get_contents()->is_directory()) ) {
         state.set_cwd(entry.second);       // set cwd to this entry
         preorder_traversal (state);        // call preorder traversal
      }
   }
   state.set_cwd(start); // Set state back to start
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // Base case: no arguments
   if ( words.size() < 2) {
      preorder_traversal(state);
      return;
   }
   for (int i=1; i<int(words.size()); ++i) {  
      wordvec pathname = split(words[i],"/");    // Get pathname
      if (!path_exists(state, pathname)) {
         cerr << "lsr: " << words[i] << ": no such directory\n";
         continue;
      }
      inode_ptr start = state.get_cwd();              
      inode_ptr dir = get_subdir_at(start, pathname);  
      state.set_cwd(dir);                             
      preorder_traversal(state);                      
      state.set_cwd(start);                           
   }
   return;
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Precondition: words is at least size 2
   if (words.size() < 2) {
      cerr << words[0] << ": missing operand\n";
      return;
   }
   // Create file in pathname words[1]
   //    parse pathname
   wordvec pathname = split(words[1], "/");
   //    separate filename from pathname
   string filename = pathname[pathname.size()-1];
   pathname.pop_back(); 
   //    assert pathname exists
   bool path_good = path_exists(state, pathname);
   if(path_good) {
      //    mkfile with name filename in directory pathname
      inode_ptr dir = get_subdir_at(state.get_cwd(),pathname);   
      inode_ptr new_node = dir->get_contents()->mkfile(filename);
      // Assert that it is not a directory
      if (new_node->get_contents()->is_directory()) {
         cerr << words[0] << ": " << filename << "is a directory\n";
         return;
      }
      new_node->get_contents()->writefile(
         wordvec(words.begin()+2, words.end()));
   }
   return;
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // If less than 2 words, complain
   if (words.size() < 2) {
      cerr << words[0] << ": missing operand" << endl;
      return;
   }
   // For all directory arguments specified in words
   for (int i=1; i< int(words.size()); ++i) {
      // Second argument should be directory. Split by delimeter '/'
      wordvec path = split(words[i], "/");
      // Assert path already exists
      if (path_exists(state, path)) {
         // error
         cerr << "mkdir: cannot create directory '" << words[i] 
              << "': File exists\n";
         continue;
      }
      // Assert parent path exists
      wordvec parent_path (path.begin(), path.end()-1);
      if (!path_exists(state, parent_path)) {
         cerr << "mkdir: cannot create directory '" << words[i] 
              << "': No such file or directory\n";
         continue;
      }
      // Make new directory in parent directory
      inode_ptr start = state.get_cwd();
      inode_ptr parent_dir = get_subdir_at(start, parent_path);
      parent_dir->get_contents()->mkdir(path[path.size()-1]); 
   }
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
   state.set_prompt(new_prompt);
   return;
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (state.get_cwd() == state.get_root() ) {
      cout << "/";
   }
   cout << state.get_cwd()->get_contents()->get_path() << endl;
   return;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Assert that words has at least one argument
   if (words.size() < 2) {
      // Print error
      cerr << "rm: missing operand\n";
      return;
   }

   // Loop through arguments
   for (int i=1; i< int(words.size()); i++) {      
      // Split argument by '/' to get pathname
      wordvec dir_name = split(words[i], "/");

      // Get parent directory name and filename
      string filename = dir_name.back();
      dir_name.pop_back();             

      // Assert that parent directory exists
      if (!path_exists(state, dir_name)) {
         cerr << "rm: cannot remove directory '" << words[i] 
              << "': No such file or directory\n";
         continue;
      }

      // Assert that we are not deleting the root directory
      if (get_subdir_at(state.get_cwd(), dir_name) 
         == state.get_root()) { 
         if (filename == "." or filename == "..") {
            cerr << "rm: refusing to remove '.' '..' "
                 << "directory: skipping '"
                 << words[i] << "'\n";
            continue;
         }
      }

      // Get pointer to parent directory
      inode_ptr parent_dir = get_subdir_at(state.get_cwd(), dir_name);

      // Get a boolean for if the file is a plain type or directory_
      auto entrs = parent_dir->get_contents()->get_dirents();
      bool pf_found = !(entrs.find(filename) 
                        == entrs.end());
      bool df_found = !(entrs.find(filename+'/') 
                        == entrs.end());

      // Assert that file exists
      if (!(pf_found or df_found)) {
         cerr << "rm: cannot remove directory '" << words[i] 
              << "': No such file or directory\n";
         continue;
      }

      // if it is a plainfile, delete it
      if (pf_found) {
         parent_dir->get_contents()->get_dirents().erase(filename);
         continue;
      }
      else { // file is a directory
         auto conts = parent_dir->get_contents();
         inode_ptr file = conts->get_dirents()[filename+'/'];

         // Assert directory is not empty
         if (file->get_contents()->size() > 2) {
            cerr << "rm: cannot delete non-empty directory\n";
            continue;
         }

         // Remove "." and ".." from directory
         file->get_contents()->get_dirents().erase(".");
         file->get_contents()->get_dirents().erase("..");
         
         // Remove file from parent
        parent_dir->get_contents()->get_dirents().erase(filename+'/');
      }
   }
}

/** recursive_delete
 *  @brief: helper fcn for fn_rmr
 *          recursively deletes a specified inode_ptr
 *          assumes the pointer exists and is a directory
 **/
void recursive_delete(inode_ptr dir) {
   // Erase "." and ".." from directory first
   if (dir->get_contents()->get_dirents().empty()) return; 
   // Loop through dirents and delete each entry
   for (auto & entry : dir->get_contents()->get_dirents()) {
      if (entry.first == "." || entry.first== ".." ) continue; // skip
      // if entry is a directory, recursively delete it
      if (entry.second->get_contents()->is_directory()) 
         {recursive_delete(entry.second);}
   }
   dir->get_contents()->get_dirents().clear();
   return;
}


void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // Assert that a path is provided
   if (words.size() < 2) {
      // Print error
      cerr << "rmr: missing operand\n";
      return;
   }
   // Loop through arguments
   for (int i=1; i<int(words.size()); ++i) {
      // Get pathname
      wordvec pathname = split(words[i], "/");
      wordvec parent_path = pathname;                                
      parent_path.pop_back();                 // Get the parent path
      string filename = pathname.back();      // Get the name
      // Assert that we are not deleting the root directory
      if (get_subdir_at(state.get_cwd(), parent_path) 
         == state.get_root()) {
         if (filename == "." or filename == "..") { // check subname
            cerr << "rmr: refusing to remove '.' '..' "
                 << "directory: skipping '" 
                 << words[i] << "'\n";
            continue;
         }
      }

      // Assert that path exists
      if (!path_exists(state, pathname)) {
         cerr << "rmr: cannot remove directory '" << words[i] 
              << "': No such file or directory\n";
         continue;
      }   
      // Get inode_ptr at path
      inode_ptr dir = get_subdir_at(state.get_cwd(), pathname);
      // Get parent
      inode_ptr pd = get_subdir_at(state.get_cwd(), parent_path);

      recursive_delete(dir);             // Recursively delete path
      pd->get_contents()->get_dirents().erase(filename+'/');
   }
   return;
}

