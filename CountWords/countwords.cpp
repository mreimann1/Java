/**
 * countwords.cpp
 *
 * @description: This program reads in all words from specified files and
 *      prints each word and the number of times that words was used.
 * @arguments: A list of filenames. If this is not passed from the command line
 *      then it takes these arguments from standard input
 */
#include <bits/stdc++.h>
##include <cctype>
using namespace std;

// Function to count words
// Counts the number of spaces or newlines and then adds 1
void num_words(string str, int *result)
{
    //Base case: String is empty;
    if (str.size() == 0)
    {
        *result = 0;
        return;
    }

    for (auto x : str)
    {
        //if there's a space or newline, then there's a string
        if (x == ' ' || x == '\n')
        {
            *result = *result + 1;
        }
    }

    *result = *result + 1;
    return;
}

int CountWords(const char* str)
{
   if (str == NULL)
      return error_condition;  // let the requirements define this...

   bool inSpaces = true;
   int numWords = 0;

   while (*str != '\0')
   {
      if (std::isspace(*str))
      {
         inSpaces = true;
      }
      else if (inSpaces)
      {
         numWords++;
         inSpaces = false;
      }

      ++str;
   }

   return numWords;
}

// Type your code here, or load an example.
int main(int args, char **argv)
{
    cout << "args: " << args << endl;

    for (int i = 0; i < args; i++)
    { //loop through arguments
        cout << argv[i] << " ";
    }

    int result = 0;
    num_words("ab abc abcd", &result);
    cout << "The count of words: " << result;
    return 0;
}
