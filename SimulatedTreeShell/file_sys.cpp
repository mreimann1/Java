// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <numeric>              // for accumulate

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
   root->contents->get_dirents().insert(direntry(".", root));
   root->contents->get_dirents().insert(direntry("..", root));
}

inode_state::~inode_state() {
  cwd = nullptr;
  root = nullptr;
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
   size_t size = data.size();
   DEBUGF ('i', "size = " << size);
  // Accumulate the contents of data into one string
  string result = "";
  for (int i=0;i<int(size);++i){
    result += data[i];
  }
  return result.size()+size-1;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data = words;
   return;
}

size_t directory::size() const {
  size_t size {0};
  DEBUGF ('i', "size = " << size);
  size = dirents.size();
  return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
  DEBUGF ('i', dirname);
  // create a inode pointer like in root
  inode_ptr new_node = make_shared<inode> (file_type::DIRECTORY_TYPE);
  // concatenate parentpath slash directory
  string new_path = path + "/" + dirname ;
  // set it to the new inodes path
  new_node->contents->set_path(new_path);
  // insert to the new inode "." , itself
  new_node->contents->get_dirents().insert(
    direntry(".", new_node));
  // insert to the new inode "..", this->getdirents.at("."))
  new_node->contents->get_dirents().insert(
    direntry("..", get_dirents().find(".")->second));
  // Insert to the root
  get_dirents().insert(direntry(dirname+'/', new_node));
  return new_node;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   // Base Case: file already exists
   if(get_dirents().find(filename)!=get_dirents().end()) {
    return get_dirents().find(filename)->second; //return the file
   }

   // create new node
   inode_ptr new_node = make_shared<inode> (file_type::PLAIN_TYPE);
   get_dirents().insert(direntry(filename, new_node));
   return new_node;
}

/** operator<<
 *  allows printing the contents of dirents to on ostream
 **/
using m_s_i = map<string,inode_ptr>; //for shortening column widths
ostream& operator<< (ostream& out, const m_s_i& dirents) { 
  DEBUGF ('i', "<<dirents");
  for (auto it=dirents.begin(); it!=dirents.end(); it++) {
    out << "[" << it->first  << "]: ["
        << it->second << "] typeid(it->second): " 
        << typeid(it->second).name()
        << " typeid(inode_ptr): " << typeid(inode_ptr).name() << "\t";
  }
  out << endl;
  return out;
}
