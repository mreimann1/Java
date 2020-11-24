// $Id: main.cpp,v 1.12 2020-10-22 16:50:08-07 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>

// For matchlines
#include <regex>
#include <cassert>

// For reading files in
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

const string cin_name = "-";

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

/**matchlines
 * @brief: parses lines from an input stream
 * @param: istream& infile - input stream to be parsed
 *         ssize_t& linecount - count of lines displayed so far
 *         str_str_map& xmap - the listmap which holds the pairs
 *         string filename - the filename to print on each line
 **/
void matchlines(istream& infile, ssize_t& linecount, 
                  str_str_map& xmap, string filename) 
{
   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
   for (;;) {
      // Get the next line
      string line;
      getline (infile, line);
      ++linecount;

      // Handle the next line
      if (infile.eof()) break;
      cout << filename << ": " << linecount << ": " << line << endl;
      
      smatch result;
      // if "comment"
      if (regex_search (line, result, comment_regex)) {
         continue;
      }
      // if "'=' found"
      if (regex_search (line, result, key_value_regex)) {
         // Set an xpair to the key assignment

         bool empty_key = (result[1].length() < 1);
         bool empty_val = (result[2].length() < 1);

         if (empty_key && empty_val) {
            // print all items in list
            xmap.print();
            continue;
         }
         if (empty_key) {
            // print items which have the given value
            xmap.print_by_val(result[2]);
            continue;
         }
         if (empty_val) {
            // find item based on key and erase it
            auto to_erase = xmap.find(result[1]);
            xmap.erase(to_erase);
            continue;
         }

         str_str_pair to_add (result[1], result[2]);

         // TODO: implement list and put to_add into list
         auto it = xmap.insert(to_add);
         it.print();
      }
      // if "key query"
      else if (regex_search (line, result, trimmed_regex)) {
         auto item = xmap.find(result[1]);
         if (item == xmap.end()) 
            cout << result[1] << ": key not found";
         else
            cout << item->first << " = " << item->second;
         cout << endl;
      }
      // any other result should not happen
      else {
         assert (false and "This can not happen.");
      }
   }
}

int main (int argc, char** argv) {
   int status = 0;
   ssize_t linecount = 0;
   str_str_map xmap;
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   if (argc < 2) {
      matchlines(cin, linecount, xmap, cin_name);
   }
   // Loop through arguments and create ifstream for matchlines
   for (int i=1; i<argc; ++i) {
      if (argv[i] == cin_name) 
         matchlines(cin, linecount, xmap, cin_name);
      else {
         string filename (argv[i]);
         ifstream infile(filename);
         //errorcheck
         if (infile.fail()) {
            status = 1;
            cerr << argv[0] << ": " << filename << ": "
                 << strerror(errno) << endl;
         }
         else {
            matchlines(infile, linecount, xmap, filename);
            infile.close();
         }
      }
      linecount = 0; // Set linecount to zero after each file is done
   }
   return status;
}

