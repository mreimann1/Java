/**
 * countwords.cpp
 *
 * @description: This program reads in all words from specified files and
 *      prints each word and the number of times that words was used.
 * @arguments: A list of filenames. If this is not passed from the command line
 *      then it takes these arguments from standard input
 */

#include <cctype>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;

/**
 * Function to count the number of words in a string
 * Some code integrated from https://stackoverflow.com/questions/3672234/c-function-to-count-all-the-words-in-a-string
 *  and from https://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
 */

void num_words(string in_str, int* result, map<string,int>* words)
{

  char * str = new char[in_str.size() +1];
  copy(in_str.begin(), in_str.end(), str);
  str[in_str.size()] = '\0';

  if (str == NULL)
    throw "String is null.";

  bool inSpaces = true;
  int numWords = 0;

  // Use int to keep track of start and end of string
  int start = 0; // start at the beginning
  int end = 0;
  string word;

  while (*str != '\0')
  {
    if (std::isspace(*str))
    {
      word = in_str.substr(start, end-start);
      cout << word << " found in text\n";
      inSpaces = true;
      start = end+1;
    }
    else if (inSpaces)
    {
      numWords++;
      inSpaces = false;
    }
    ++str;
    ++end;
  }

  *result = numWords;
  return;
}

/**
 * Functio to open a file, then call num_words to find the number of words in
 * that file.
 */
void open_and_count(string arg, int* result, map<string, int>* words) {
  cout << arg << " is being opened...\n";
  ifstream f_in;
  f_in.open(arg);

  string line;
  if (f_in.is_open()) {
    while ( getline(f_in, line)) {
      cout << line << endl;
      int line_count;
      num_words(line, &line_count, words); // count each word and add to total
      *result += line_count;
    }
    f_in.close();
  } else {
    cout << "Unable to open file\n";
  }

  return;
}

// Type your code here, or load an example.
int main(int args, char **argv)
{
    // Add a map which will be used by the whole program
    map<string, int> words;

    //TODO: add code to iterate through list of arguments
    for (int i = 0; i < args; i++)
    { //loop through arguments
        cout << argv[i] << " ";
    }
    int result = 0;
    //num_words("ab abc abcd", &result);
    string filename = argv[1];
    //strncpy(filename, argv[1]);
    cout << "filename: " << filename << endl;
    open_and_count(filename, &result, &words);
    cout << "The count of words: " << result << endl;
    return 0;
}
