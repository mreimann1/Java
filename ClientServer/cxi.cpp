// $Id: cxi.cpp,v 1.1 2020-11-22 16:51:43-08 - - $

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

// For stat
#include <sys/stat.h>

// For reading files
#include <fstream>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cxi_exit: public exception {};

unordered_map<string,cxi_command> command_map {
   {"exit", cxi_command::EXIT},
   {"help", cxi_command::HELP},
   {"ls"  , cxi_command::LS  },
   {"put" , cxi_command::PUT },
   {"get" , cxi_command::GET },
   {"rm"  , cxi_command::RM  },
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cxi_help() {
   cout << help;
}

void cxi_ls (client_socket& server) {
   cxi_header header;
   header.command = cxi_command::LS;
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cxi_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      buffer[host_nbytes] = '\0';
      cout << buffer.get();
   }
}


void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cxi_exit();
}

/** split
 *  @brief: Loops over the string, splits into a vector of substrings
 *  @param: string& line - A string to be parsed
 *          string& delimeters - the delimeters to split the word on
 *  @notes: This code is copied from the asgn2 util.cpp
 **/
using wordvec = vector<string>;
wordvec split (const string& line, const string& delimeters) {
   wordvec words;
   size_t end = 0;

   for (;;) {
      size_t start = line.find_first_not_of (delimeters, end);
      if (start == string::npos) break;
         end = line.find_first_of (delimeters, start);
      words.push_back (line.substr (start, end-start));
   }

   return words;
}

/** get_file_size
 *  @brief: Stats a file to get size
 *  @param: string filename - name of the file to stat
 *          ssize_t result - value to set the size of the file to
 *  @return: 0 if successful, -1 if FNF
 **/
int get_file_size(string filename, ssize_t& result) {
   struct stat stat_buf;
   if (stat( filename.c_str(), &stat_buf) !=0) return -1;
   result = stat_buf.st_size;
   return 0;
}

/** cxi_put
 *  @brief: Handles the put request for client
 *          Puts file from local dir to server
 *  @param: client_socket& server - the socket connection to the server
 *          wordvec& command - name of file to put
 *          char** argv - for error handling
 **/
void cxi_put(client_socket& server, wordvec& command, char** argv) {
   // errorcheck
   if (command.size() < 2) {
      cerr << argv[0] << ": " << command[0] << ": too few arguments\n";
      return;
   }
   if (command.size() > 2) {
      cerr << argv[0] << ": " << command[0] << ": too many arguments\n";
      return;
   }

   string fname = command[1];
   ssize_t fsize;
   if (get_file_size(fname, fsize) < 0) 
      cerr << argv[0] << ": " << fname << ": "<< strerror (errno) << endl;
   else {
      // create buffer with size
      char* buffer = new char[fsize+1];
      memset(buffer, 0, fsize);

      // fill buffer with file contents
      ifstream is (fname.c_str(), ifstream::binary);
      if (is) {
         is.read(buffer, fsize);
         is.close();
      }
      else {    
         cerr << argv[0] << ": " << fname << ": "<< strerror (errno) << endl;
         return;
      }
      buffer[fsize] = '\0';

      // send bytes to server
      cxi_header header;
      header.nbytes = fsize;
      header.command = cxi_command::PUT;
      strcpy(header.filename, fname.c_str());
      outlog << "sending header " << header << endl;
      send_packet (server, &header, sizeof header);

      // send buffer to server
      send_packet (server, buffer, fsize);

      recv_packet (server, &header, sizeof header);
      outlog << "received header " << header << endl;

      // delete buffer
      delete[] buffer;
   }
   return;
}

/** cxi_get
 *  @brief: Handles the get request for client
 *          Gets file from server dir to client
 *  @param: client_socket& server - the socket connection to the server
 *          wordvec& command - name of file to get
 *          char** argv - for error handling
 **/
void cxi_get(client_socket& server, wordvec& command, char** argv) {
   // errorcheck
   if (command.size() < 2) {
      cerr << argv[0] << ": " << command[0] << ": too few arguments\n";
      return;
   }
   if (command.size() > 2) {
      cerr << argv[0] << ": " << command[0] << ": too many arguments\n";
      return;
   }

   // create header message 
   string fname = command[1];
   cxi_header header;
   header.nbytes = 0;
   header.command = cxi_command::GET;
   strcpy(header.filename, fname.c_str());

   // send header to server
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);

   // recv first response
   recv_packet (server, &header, sizeof header);

   // check for NAK
   if (header.command == cxi_command::NAK) {
      outlog << "Server did not find file\n";
      return;
   }

   // create a buffer with response size
   ssize_t size = ntohl (header.nbytes);
   char* buffer = new char[size];
   outlog << "buffer of size: " << size << " created...\n";

   // receive bytes to buffer
   recv_packet(server, buffer, size);

   // write buffer to file
   ofstream file(header.filename);
   if (!file) {                                 // Error check
      cerr << argv[0] << ": " << fname << ": "<< strerror (errno) << endl;
      return;
   }
   file.write(buffer, size);
   file.close();

   // delete buffer
   delete[] buffer;

   return;
}

/** cxi_rm
 *  @brief: Handles the rm request for client
 *          Removes file from server dir
 *  @param: client_socket& server - the socket connection to the server
 *          wordvec& command - name of file to get
 *          char** argv - for error handling
 **/
void cxi_rm(client_socket& server, wordvec& command, char** argv) {
   // errorcheck
   if (command.size() < 2) {
      cerr << argv[0] << ": " << command[0] << ": too few arguments\n";
      return;
   }
   if (command.size() > 2) {
      cerr << argv[0] << ": " << command[0] << ": too many arguments\n";
      return;
   }

   // create header message 
   string fname = command[1];
   cxi_header header;
   header.nbytes = 0;
   header.command = cxi_command::RM;
   strcpy(header.filename, fname.c_str());

   // send header to server
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);

   // receive response
   recv_packet (server, &header, sizeof header);

   // check for NAK or ACK
   if (header.command == cxi_command::NAK)
      outlog << "Server did not find file\n";
   else 
      outlog << "File removed successfully\n";
   return;
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cxi_server_host (args, 0);
   in_port_t port = get_cxi_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cxi_exit();
         outlog << "command " << line << endl;

         wordvec command = split(line, " ");

         const auto& itor = command_map.find (command[0]);
         cxi_command cmd = itor == command_map.end()
                         ? cxi_command::ERROR : itor->second;
         switch (cmd) {
            case cxi_command::EXIT:
               throw cxi_exit();
               break;
            case cxi_command::HELP:
               cxi_help();
               break;
            case cxi_command::LS: 
               cxi_ls (server);
               break;
            case cxi_command::PUT:
               cxi_put(server, command, argv);
               break;
            case cxi_command::GET:
               cxi_get(server, command, argv);
               break;
            case cxi_command::RM:
               cxi_rm(server, command, argv);
               break;
            default:
               outlog << command[0] << ": invalid command" << endl;
               break;

         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      outlog << "caught cxi_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}

