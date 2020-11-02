// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};
// NOTE: Redeclaration of a static member if the class

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
   root = make_shared<inode> (file_type::DIRECTORY_TYPE);
   //root->get_contents()->set_path("/");
   cwd = root;
   root->contents->get_dirents().insert(pair<string, inode_ptr>(".", root));
   root->contents->get_dirents().insert(pair<string, inode_ptr>("..", root));
}

const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

/** get_subdir_at
 *  @brief: returns an inode_ptr to the subdirectory at a specified path
 *  @param: wordvec& pathname - a vector of pathnames to traverse.
 **/
inode_ptr inode::get_subdir_at(wordvec& pathname) {
  // Base case: pathname is empty. Return this
  inode_ptr curr = make_shared<inode> (*this);

  // Find the subdirectory that corresponds to element of pathname
  for (int i=0; i<int(pathname.size()); ++i) {
    // Assert that curr is a directory
    if (!curr->get_contents()->is_directory()) {
      cout << "Error in get_subdir_at: pathname["<< i << "]: " << pathname[i] << " DNE.\n";
      return nullptr;
    }

    // Assert that the entry at the next directory exists
    if (curr->get_contents()->get_dirents().find(pathname[i])==curr->get_contents()->get_dirents().end()) {
      cout << "Error in get_subdir_at: pathname["<< i << "]: " << pathname[i] << " DNE.\n";
      return curr;
    }

    // Move current node to the next directory entry
    curr = curr->get_contents()->get_dirents().find(pathname[i])->second;

    cout << "curr: " << curr << " typeid(curr): " << typeid(curr).name() << endl;

  }
  return curr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}


size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);

   cout << "THIS FUNCTION IS BEING ENTERED..\n";

   // create a inode pointer like in root
   inode_ptr new_node = make_shared<inode> (file_type::DIRECTORY_TYPE);
   // get parent path (this->getpath)
   cout << "path: " << path << endl;
   // concatenate parentpath slash directory
   string new_path = path + dirname + '/';
   cout << "new_path: " << new_path << endl;
   // set it to the new inodes path
   new_node->contents->set_path(new_path);
   // insert to the new inode "." , itself
   new_node->contents->get_dirents().insert(direntry(".", new_node));
   // insert to the new inode "..", this->getdirents.at("."))
   new_node->contents->get_dirents().insert(direntry("..", this->get_dirents().at(".")));
   // Insert to the root
   get_dirents().insert(direntry(new_path, new_node));

   cout << "new_node: " << new_node << "\t&new_node: " << &new_node << "\tnew_node->contents: " << new_node->contents << endl
        << "new_node->contents->get_dirents(): " << new_node->contents->get_dirents() << endl;
   return new_node;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   return nullptr;
}

/** operator<<
 *  allows printing the contents of a directory to filestream
 **/
// ostream& operator<< (ostream& out, const directory& dir) { 
//   DEBUGF ('i', "<<directory");
//   return out << dir.get_dirents();
// }

/** operator<<
 *  allows printing the contents of dirents to on ostream
 **/
ostream& operator<< (ostream& out, const map<string,inode_ptr>& dirents) { 
  DEBUGF ('i', "<<dirents");
  for (auto it=dirents.begin(); it!=dirents.end(); it++) {
    out << "[" << it->first  << "]: ["
        << it->second << "] typeid(it->second): " << typeid(it->second).name() << " typeid(inode_ptr): " << typeid(inode_ptr).name() << "\t"; 
  }
  out << endl;
  return out;
}
