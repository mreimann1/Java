// $Id: ubigint.cpp,v 1.10 2020-10-11 12:25:22-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"

// The maximum value that a 32 byte (windows) unsigned long can take
#define MAX_LONG_WIN    429496795   
#define MAX_LONG        1000000  // Arbitrary size limit for udigit_t 

ubigint::ubigint (unsigned long that){ // NOTE: untested. Where is this called??
   DEBUGF ('~', "unsigned long that = \"" << that << "\"");
   string that_as_string = to_string(that); // NOTE: Possibly change to ostringstream
   for(unsigned int i=0; i<that_as_string.size(); i++) {
      udigit_t digit = that_as_string[i] - '0';
      ubigvalue.push_back(digit);
   }

}

ubigint::ubigint (const string& that){//: uvalue(0) {
   DEBUGF ('~', "that = \"" << that << "\"");
   udigit_t digit = 0;
   for (auto iter=that.crbegin(); iter!=that.crend(); ++iter) {
      if (not isdigit (*iter)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      // for every digit, push digit back as a float
      digit = *iter - '0'; 
      DEBUGF ( '~', "digit = \"" << digit << "\"");
      ubigvalue.push_back(digit);
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
   //DEBUGF ('u', *this << "+" << that);

   // Precondition: the ubigints are the same size.
   //       if not, append zeros

   // initialize empty ubigint result
   // initialize remainder 0
   // for all elements i in ubigvalue:
   //    digit = ubigvalue[i] + that.ubigvalue[i] + remainder
   //    if (digit > 9) :
   //       remainder = digit/10
   //       digit = digit%10
   //    push digit

   ubigint result; // initialize empty result
   udigit_t remainder = 0; // initialize empty remainder
   for (long unsigned int i=0; i<ubigvalue.size(); i++) {
      udigit_t digit = ubigvalue[i] + that.ubigvalue[i] + remainder;
      if(digit >9) {
         remainder   = digit/10;
         digit       = digit%10;
      }
      result.ubigvalue.push_back(digit);
   }
   // if there is a remainder, append it
   if(remainder) {
      DEBUGF ('u', "remainder: \"" << remainder << "\"");
      result.ubigvalue.push_back(remainder);
   }
   DEBUGF ('u', "result.ubigvalue: \"" << result << "\"");
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   // return ubigint (uvalue - that.uvalue); //commented out to get things to run
}

ubigint ubigint::operator* (const ubigint& that) const {
   // return ubigint (uvalue * that.uvalue);
}

void ubigint::multiply_by_2() {
   // uvalue *= 2;
}

void ubigint::divide_by_2() {
   // uvalue /= 2;
}


struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   // return uvalue == that.uvalue;
}

bool ubigint::operator< (const ubigint& that) const {
   // return uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   //DEBUGF ('~', "that.ubigvalue.size(): " << that.ubigvalue.size() << ". ");
   string result = "";                       // NOTE: Possibly change to ostringstream
   for (auto iter = that.ubigvalue.crbegin(); iter!= that.ubigvalue.crend(); ++iter) {
      result += to_string(*iter);
   }
   return out << "ubigint(" << result << ")";
   // return out << "ubigint(" << that.uvalue << ")";
}

