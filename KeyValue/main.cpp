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

// TODO: Integrate xlist

/**matchlines
 * @brief: parses lines from an input stream
 * @param: istream& infile - input stream to be parsed
 *         ssize_t& linecount - count of lines displayed so far
 *         str_str_map& xmap - the listmap which holds the pairs
 **/
void matchlines(istream& infile, ssize_t& linecount, 
                  str_str_map& xmap) 
{
   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
   for (;;) {
      string line;
      getline (infile, line);
      if (infile.eof()) break;
      cout << endl << "input: \"" << line << "\"" << endl; //@DELETE
      smatch result;
      // if "comment"
      if (regex_search (line, result, comment_regex)) {
         cout << "-: " << linecount << ": " << line << endl;
         continue;
      }
      // if "key assignment"
      if (regex_search (line, result, key_value_regex)) {
         // Set an xpair to the key assignment
         str_str_pair to_add (result[1], result[2]);
         cout << "to_add.first: " << to_add.first 
              << " to_add.second: " << to_add.second << endl;

         // TODO: implement list and put to_add into list
         xmap.insert(to_add);
      }
      // if "query"
      else if (regex_search (line, result, trimmed_regex)) {
         cout << "query: \"" << result[1] << "\"" << endl;
      }
      // any other result should not happen
      else {
         assert (false and "This can not happen.");
      }
   }
}

int main (int argc, char** argv) {
   int status = 0;
   ssize_t linecount = 1;                       // track # lines
   str_str_map xmap;
   cout << "Hello, this is the keyvalue program:\n";
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   if (argc < 2) {
      cout << "Reading from standard input:\n";
      matchlines(cin, linecount, xmap);
   }
   // Loop through arguments and create ifstream for matchlines
   for (int i=1; i<argc; ++i) {
      if (argv[i] == cin_name) matchlines(cin, linecount, xmap);
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
            matchlines(infile, linecount, xmap);
            infile.close();
         }
      }
   }

   cout << "Done reading from files\n"; //@DELETE

   str_str_map test;
   cout << "test:" << test << endl;
   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      str_str_pair pair (*argp, to_string<int> (argp - argv));
      cout << "Before insert: " << pair << endl;
      test.insert (pair);
   }

   cout << "test.empty():" << test.empty() << endl;
   for (str_str_map::iterator itor = test.begin();
        itor != test.end(); ++itor) {
      cout << "During iteration: " << *itor << endl;
   }

   str_str_map::iterator itor = test.begin();
   test.erase (itor);

   cout << "Exit status: " << status << endl;
   return status;
}

