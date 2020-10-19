// $Id: bigint.cpp,v 1.3 2020-10-11 12:47:51-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
  is_negative = that.size() > 0 and that[0] == '_';
  uvalue = ubigint (that.substr (is_negative ? 1 : 0));
  assert_positive_zero();
}

bigint bigint::operator+ () const {
  return *this;
}

bigint bigint::operator- () const {
  ubigint zero{0};
  if (uvalue==zero) return *this;
  return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {

  // if their signs match :
  //  add their uvalues
  //  copy the sign
  // else (their signs are different) :
  //  subtract the smaller from the larger
  //  set the result sign to the sign of the bigger
  // make sure zeros sign is positive

  bigint result;

  if (is_negative == that.is_negative) {
    result.uvalue = uvalue + that.uvalue;
    result.is_negative = is_negative;
    result.assert_positive_zero();
    return result;
  }

  // else (their signs are different)

  bool this_less_than_that = uvalue < that.uvalue;
  bool that_less_than_this = that.uvalue < uvalue;

  if (this_less_than_that ) {
    DEBUGF('+', uvalue << " < " << that.uvalue);
    result.uvalue = that.uvalue - uvalue;
    result.is_negative = that.is_negative;
  }

  else if (that_less_than_this) {
    DEBUGF('+', uvalue << " > " << that.uvalue);
    result.uvalue = uvalue - that.uvalue;
    result.is_negative = is_negative;
  }
  else /* they have equal uvalues */ {
    DEBUGF('+', uvalue << " == " << that.uvalue);
    ubigint zero {0};
    result.uvalue = zero;
    result.is_negative = false;
  }
  result.assert_positive_zero();
  return result;
}

bigint bigint::operator- (const bigint& that) const {

  // if their signs match:
  //  subtract the smaller from larger
  //  give it the opposite sign
  // else (their signs are different) :
  //  add their uvalues
  //  copy this sign
  
  bigint result;

  if (is_negative == that.is_negative) {
    bool this_less_than_that = uvalue < that.uvalue;
    bool that_less_than_this = that.uvalue < uvalue;

    if(this_less_than_that) {
      result.uvalue = that.uvalue - uvalue;
      result.is_negative = !that.is_negative;
    }
    else if (that_less_than_this) {
      result.uvalue = uvalue - that.uvalue;
      result.is_negative = is_negative;      
    }
    else { // they are the same
      ubigint zero {0};
      result.uvalue = zero;
    }

    result.assert_positive_zero();
    return result;
  }

  // else they have different signs

  result.uvalue = uvalue + that.uvalue;
  result.is_negative = is_negative;
  result.assert_positive_zero();
  return result;
}


bigint bigint::operator* (const bigint& that) const {

  // if the signs are different :
  //  result sign is negative
  // if the signs are the same :
  //  result sign is positive

  bigint result;
  result.uvalue = uvalue * that.uvalue;
  result.is_negative = (is_negative != that.is_negative);
  result.assert_positive_zero();
  return result;
}

bigint bigint::operator/ (const bigint& that) const {

  // if the signs are different :
  //  result sign is negative
  // if the signs are the same :
  //  result sign is positive

  bigint result;
  result.uvalue = uvalue / that.uvalue;
  result.is_negative = (is_negative != that.is_negative);
  result.assert_positive_zero();
  return result;
}

bigint bigint::operator% (const bigint& that) const {
  bigint result = uvalue % that.uvalue;
  return result;
}

bool bigint::operator== (const bigint& that) const {
  return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
  if (is_negative != that.is_negative) return is_negative;
  return is_negative ? uvalue > that.uvalue
                     : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {

  return out << (that.is_negative ? "-" : "")
            << that.uvalue;
}

/**
 *  @brief: This function asserts that if thebigint is zero, that it's sign is positive.
 **/
void bigint::assert_positive_zero() {
  ubigint zero{0};
  if (uvalue == zero) is_negative = false; 
}