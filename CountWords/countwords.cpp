/**
 * @file: countwords.cpp
 *
 * @brief: This program reads in all words from specified files and
 *      prints each word and the number of times that words was used.
 * @arguments: A list of filenames. If this is not passed from the command line
 *      then it takes these arguments from standard input
 * @author: Marques Reimann
 */

#include <cctype>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;

/**
 * @brief: Count the number of words in a string
 * @param: string in_str - string to be parsed
 *         map<string,int>& words - map of words to their counts
 * @return: Void
 * Some code integrated from https://stackoverflow.com/questions/3672234/c-function-to-count-all-the-words-in-a-string
 *  and from https://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
 */
void num_words(string in_str, map<string,int>& words) {
  // Copy the input string to a char*
  char * str = new char[in_str.size() +1];
  copy(in_str.begin(), in_str.end(), str);
  str[in_str.size()] = '\0';

  // Check for a null string
  if (str == NULL)
    throw "String is null.";

  // Initialize variables
  bool inSpaces = true;
  int numWords = 0;
  int start = 0; // Track the start of a word
  int end = 0;  // Track the end of a word
  string word;

  // Iterate through input string and find each word
  while (*str != '\0') {
    if (std::isspace(*str)) {
      word = in_str.substr(start, end-start);
      words[word]++;
      inSpaces = true;
      start = end+1;
    }
    else if (inSpaces) {
      numWords++;
      inSpaces = false;
    }
    ++str;
    ++end;
  }
  word = in_str.substr(start, end-start);
  words[word]++;

  return;
}

/**
 * @brief: Open a file, then call num_words to find the number of words in
 *         that file.
 * @param: string filename - name of file to be opened
 *         map<string,int>& words - map of words to their counts
 * @return: Void
 */
void open_and_count(string filename, map<string, int>& words) {
  ifstream f_in;
  f_in.open(filename);
  string line;
  if (f_in.is_open()) {
    while ( getline(f_in, line)) {
      num_words(line, words); // count each word and add to total
    }
    f_in.close();
  } else {
    cout << "Unable to open file \"" << filename << "\".\n";
  }

  return;
}

/**
 * @brief: Program entrypoint
 * @param: Command line arguments. Expects texts files.
 * @return Should not return
 */
int main(int args, char **argv) {
    // Add a map which will be used by the whole program
    map<string, int> words;

    // Iterate through arguments
    for (int i = 1; i < args; i++) {
      string filename = argv[i];
      cout << "filename: " << filename << endl;
      open_and_count(filename, words);
    }

    // Print the wrods and their counts
    for(auto elem : words) {
      cout << "\"" << elem.first << "\" has a count of " << elem.second << ".\n";
    }
    return 0;
}
