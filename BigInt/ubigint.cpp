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
   DEBUGF ('u', *this << "+" << that);

   // Find largest size and smallest size
   // initialize empty ubigint result
   // initialize remainder 0
   // for all elements i in smaller vector:
   //    digit = ubigvalue[i] + that.ubigvalue[i] + remainder
   //    if (digit > 9) :
   //       remainder = digit/10
   //       digit = digit%10
   //    push digit
   // push the rest of the elements from bigger vector

   // Determine larger and smaller vector
   long unsigned int smaller_size, larger_size;
   bool this_is_smaller_than_that = ubigvalue.size() < that.ubigvalue.size();
   bool that_is_smaller_than_this = ubigvalue.size() > that.ubigvalue.size();
   bool that_and_this_same_size  = ubigvalue.size() == that.ubigvalue.size();

   DEBUGF('u', "this_is_smaller_than_that: " << this_is_smaller_than_that << " that_is_smaller_than_this: " << that_is_smaller_than_this << " that_and_this_same_size: " << that_and_this_same_size);

   ubigint result; // initialize empty result
   udigit_t remainder = 0; // initialize empty remainder
   if(this_is_smaller_than_that) {
      DEBUGF('u', "second element from top \"" << *this << "\" is smaller than top element \"" << that << "\"");
      smaller_size =  ubigvalue.size();
      larger_size = that.ubigvalue.size();
      
      for (long unsigned int i=0; i<smaller_size; i++) {
         // add up the elements of the two vectors
         udigit_t digit = ubigvalue[i] + that.ubigvalue[i] + remainder;
         // carry the remainder
         remainder   = digit/10;
         digit       = digit%10;
         result.ubigvalue.push_back(digit);
      }
      // push the rest of the elements from that.ubigvalue
      for(long unsigned int i=smaller_size; i<larger_size; i++) {
         udigit_t digit = that.ubigvalue[i] + remainder;
         // carry the remainder
         remainder   = digit/10;
         digit       = digit%10;
         result.ubigvalue.push_back(digit);
      }
      // check for remainder
      if (remainder) {
         DEBUGF ('u', "remainder: \"" << remainder << "\"");      
         result.ubigvalue.push_back(remainder);
      }
   }
   else if (that_is_smaller_than_this) {
      DEBUGF('u', "second element from top \"" << *this << "\" is larger than top element \"" << that << "\"");
      smaller_size = that.ubigvalue.size();
      larger_size = ubigvalue.size();

      for (long unsigned int i=0; i<smaller_size; i++) {
         // add up the elements of the two vectors
         udigit_t digit = ubigvalue[i] + that.ubigvalue[i] + remainder;
         // carry the remainder
         remainder   = digit/10;
         digit       = digit%10;
         result.ubigvalue.push_back(digit);
      }
      // push the rest of the elements from this.ubigvalue
      for(long unsigned int i=smaller_size; i<larger_size; i++) {
         udigit_t digit = ubigvalue[i] + remainder;
         // carry the remainder
         remainder   = digit/10;
         digit       = digit%10;
         result.ubigvalue.push_back(digit);
      }
      // check for remainder
      if (remainder) {
         DEBUGF ('u', "remainder: \"" << remainder << "\"");      
         result.ubigvalue.push_back(remainder);
      }
   }
   else { // they are same size
      DEBUGF('u', "second element from top \"" << *this << "\" is same size as top element \"" << that << "\"");
      smaller_size = ubigvalue.size();
      larger_size = smaller_size;

      for (long unsigned int i=0; i<smaller_size; i++) {
         // add up the elements of the two vectors
         udigit_t digit = ubigvalue[i] + that.ubigvalue[i] + remainder;
         // carry the remainder
         remainder   = digit/10;
         digit       = digit%10;
         result.ubigvalue.push_back(digit);
      }
      // check for remainder
      if (remainder) {
         DEBUGF ('u', "remainder: \"" << remainder << "\"");      
         result.ubigvalue.push_back(remainder);
      }
   }

   DEBUGF ('u', "result.ubigvalue: \"" << result << "\"");
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   DEBUGF ('u', *this << "-" << that);

   // Error check: this.uvalue must be greater than that.uvalue
   // if (this<that) : throw error
   // initialize empty ubigint result
   // initialize udigit_t borrow to zero
   // for all elements in that.ubigvalue:
   //    right_operand = that.ubigvalue[i] - borrow
   //    if ubigvalue[i] < right_operand :
   //       right_operand+=10
   //       borrow = 1
   //    digit = ubigvalue[i] - right_operand
   //    push digit to result
   // for  all remaining elements in this.ubigvalue:
   //    push element to result
   //    only consideration is borrow
   // pop high-order zeros
   // return result

   // Error check
   // if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   if ( *this < that ) {
      throw domain_error("Subtraction of larger ubigint from smaller ubigint");
   }

   long unsigned int right_size = that.ubigvalue.size();
   long unsigned int left_size = ubigvalue.size();

   ubigint result;
   udigit_t borrow = 0;
   for(long unsigned int i=0; i<right_size; i++) {
      udigit_t right_operand = that.ubigvalue[i] + borrow;
      udigit_t left_operand = ubigvalue[i];
      borrow = 0;
      DEBUGF('u', "left_operand: " << left_operand << " right_operand: " << right_operand);
      
      if (left_operand < right_operand) { 
         DEBUGF('u', "Performing borrow...");
         // Perform borrow operation
         left_operand += 10;
         borrow = 1;
      }
      result.ubigvalue.push_back(left_operand - right_operand);
   
   }
   // Handle the rest of the digits
   for(long unsigned int i=right_size; i<left_size; i++) {
      udigit_t left_operand = ubigvalue[i];
      if(left_operand) { // if you can borrow from this digit
         left_operand -= borrow;
         borrow=0;
      }
      else { // borrow from another digit
         left_operand = 9;
         borrow=1; //redundant
      }
      result.ubigvalue.push_back(left_operand);
   }

   // Pop high-order zeros
   while (result.ubigvalue.back()==0 && result.ubigvalue.size() > 1) {
      result.ubigvalue.pop_back();
   }

   return result;
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
   DEBUGF ('u', *this << "==" << that);

   // if sizes are not the same;
   //    return false
   // for i=0 to length
   //    if ubigvalue[i] != that.ubigvalue[i] :
   //       return false
   // return true


   // compare the sizes
   long unsigned int length = ubigvalue.size();
   if (length != that.ubigvalue.size()) {
      return false;
   }
   string debug_statement = "";
   // search through vectors for any difference
   for (int i = 0; i < int(length)-1; i++) {
      if (ubigvalue[i] != that.ubigvalue[i]) {
         return false;
      }
   }
   // no differences have been found
   return true;
   // return uvalue == that.uvalue;
}

bool ubigint::operator< (const ubigint& that) const {
   DEBUGF ('u', *this << "<" << that);

   // Check the sizes :
   // BASE CASE: Both arrays have length zero
   // if this.size < that.size: return true
   // if this.size > that.size: return false
   // else: length = size()
   // for i=length-1 to 0 :
   //    if this[i] < that[i] : return true
   //    if this[i] > that[i] : return false
   // return false

   // Compare the sizes
   long unsigned int length = ubigvalue.size();

   // BASE CASE: Left array or both arrays have length 0;
   if (length==0) {
      DEBUGF('<', "Left hand array length: " << length);
      return false;
   }
   if (length < that.ubigvalue.size()) {
      DEBUGF('<', "Left operand is less than right operand");
      return true;
   }
   if (length > that.ubigvalue.size()) {
      DEBUGF('<', "Left operand is greater than right operand");
      return false;
   }
   // Loop through elements starting with highest order element
   for(int i =int(length)-1; i>0; i--) {
      if (ubigvalue[i] < that.ubigvalue[i]) {
         DEBUGF('<', "this: " << *this << "that: " << "that: " <<  that << endl
            << "i: " << i << "\"" << ubigvalue[i] << "\" < \"" << that.ubigvalue[i] << "\"");
         return true;
      }
      if (ubigvalue[i] > that.ubigvalue[i]) {
         return false;
      }
   }
   // No difference found
   DEBUGF('<', "Left operand is less than right operand");  
   return false;
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

