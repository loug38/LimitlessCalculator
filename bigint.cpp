// $Id: bigint.cpp,v 1.61 2014-06-26 17:06:06-07 - - $
//Lou George ligeorge
//Ryan Kwok rfkwok

#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
#include <cmath>
using namespace std;

#include "bigint.h"
#include "debug.h"

//bigint::bigint (long that): long_value (that) {
//   DEBUGF ('~', this << " -> " << long_value)
//}

//long ctor
bigint::bigint(long that){
    int itor = 0;
    string longValue = to_string(that);
    if (longValue[0] == '_'){
        this->negative = true;
        itor++;
    }

    for (int i = longValue.size(); i >= itor; i-- )
        this->big_value.push_back(longValue[i]);
   
}//long ctor

//stringctor
bigint::bigint(const string &that){
    auto itor = that.cbegin();
    this->negative = false;

    if (itor != that.cend() && *itor == '_'){
       this->negative = true;
       itor++;
    }

    char charPtr;
    for (auto i = that.cend(); i != itor; i--){
        charPtr = *i;
        this->big_value.push_back(charPtr);
    }
    charPtr = *itor;
    this->big_value.push_back(charPtr);
}//string ctor

//bigint::bigint (const string& that) {
//   auto itor = that.cbegin();
//   bool isnegative = false;
//   if (itor != that.cend() && *itor == '_') {
//      isnegative = true;
//      ++itor;
//   }
//   int newval = 0;
//   while (itor != that.end()) newval = newval * 10 + *itor++ -'0';
//   long_value = isnegative ? - newval : + newval;
//   DEBUGF ('~', this << " -> " << long_value)
//}

bigint operator+ (const bigint& left, const bigint& right) {
// return left.long_value + right.long_value;
// Make sure this isn't a subtraction problem in disguise
   //return bigSum(left, right);
   
//   return left.long_value - right.long_value;
    bigint ret, leftPass, rightPass;
    int carry =0;
    int leftInt, rightInt;
    int sizeLeft = left.big_value.size() - 1;
    int sizeRight = right.big_value.size() - 1;
    int maxSize = sizeLeft > sizeRight ? sizeLeft : sizeRight;
    // if the signs are different, return it as an addition problem.

    if (left.negative != right.negative){
        if(right.negative){
        // left is positive
            rightPass = right;
            leftPass = left;
            rightPass.negative = false;
            leftPass.negative = false;
            return leftPass - rightPass;
        } else {
        // right is positive
            rightPass = right;
            leftPass = left;
            rightPass.negative = false;
            leftPass.negative = false;
           return rightPass - leftPass;
        }
    } else {
    //  Both signs are the same
        if(left.negative){ // Both neg
            ret.negative = true;
            ret.big_value.push_back('_');
        } else {           // Both pos
            ret.negative = false;
            ret.big_value.push_back(' ');
       }
    }

    for (int i = 1; i <= maxSize; i++){
        //convert from chars to int

        if (i > sizeLeft){                              
            leftInt = 0;
        }else{
            leftInt = left.big_value.at(i)  - 48;
        }

        if (i > sizeRight){
            rightInt = 0;
        }else{
            rightInt = right.big_value.at(i) - 48;
        }

        if (leftInt > 9 || leftInt < 0)
            leftInt = 0;

        // Perform addition
        // carry is left over from the previous digit
        int sum = leftInt + rightInt + carry;
        carry = 0; // refresh carry
        while (sum > 9){
            sum -= 10;
            carry += 1;
        }
        
        ret.big_value.push_back(sum + 48);
    }
    //  Push the leftover to the back of the list
    if( carry > 0 ){
        ret.big_value.push_back(carry+ 48);
    }/*
    for (int i = maxSize; i > 0; i--) {                   
       if (ret.big_value.at(i) != '0') break;
       ret.big_value.pop_back();
    } */
    return ret;
}

bigint operator- (const bigint& left, const bigint& right) {
//   return left.long_value - right.long_value;
    bigint ret;
    bool borrow = false;
    int leftInt, rightInt;
    int sizeLeft = left.big_value.size() - 1;
    int sizeRight = right.big_value.size() - 1;
    int maxSize = sizeLeft > sizeRight ? sizeLeft : sizeRight;

    if (left < right){
        ret.negative = true;
        ret.big_value.push_back('_');
    }else{
        ret.negative = false;
        ret.big_value.push_back(' ');
    }

    if ((left == right) && (left.negative == right.negative)){
        ret.big_value.push_back('0');
        return ret;
    }

    for (int i = 1; i <= maxSize; i++){
        //convert from chars to int
        //if we know answer is positive
        if(!ret.negative){                     
            if (i > sizeLeft){                              
                leftInt = 0;
            }else{
                leftInt = left.big_value.at(i)  - 48;
            }

            if (i > sizeRight){
                rightInt = 0;
            }else{
                rightInt = right.big_value.at(i) - 48;
            }
        }
        //if we know answer is negative,
        //flip left and right, do normal subtract
        if(ret.negative){                               
            if (i > sizeRight){                         
                leftInt = 0;                            
            }else{
                leftInt = right.big_value.at(i) - 48;
            }

            if (i > sizeLeft){
                rightInt = 0;
            }else{
                rightInt = left.big_value.at(i) - 48;
            }
        }

        //Case previous subtraction borrowed:
        if (borrow && leftInt != 0){
            leftInt--;
            borrow = false;
        }
        if (leftInt == 0 && borrow){
            leftInt = 9;
            borrow = true;
            ret.big_value.push_back((leftInt - rightInt) + 48);
            continue;
        }

        if ((left.negative && !right.negative) 
        || (!left.negative && right.negative)){
            ret.big_value.push_back((leftInt + rightInt) + 48);
            continue;
        }

        //Basic Subtractions:
        //top int has to borrow
        if (leftInt < rightInt){                               
            borrow = true;
            leftInt += 10;
            ret.big_value.push_back((leftInt - rightInt) + 48); 
            continue;
        }
         //no borrow needed
        if (leftInt > rightInt){                               
            borrow = false;
            ret.big_value.push_back((leftInt - rightInt) + 48);
        }
         //left and right are equal no borrow
        if (leftInt == rightInt){                              
            borrow = false;
            ret.big_value.push_back('0');   
        }
    }
    //Pop off excessive 0s
    for (int i = maxSize; i > 0; i--){                          
        if (ret.big_value.at(i) != '0')
            break;
        ret.big_value.pop_back();
    }
    return ret;
}

bigint operator+ (const bigint& right) {
    return right;
}

bigint operator- (const bigint& right) {
    //   return -right.long_value;
    bigint returnVal = right;
    returnVal.negative = !right.negative;
    return returnVal;
}

long bigint::to_long() const {
  //  if (*this <= bigint (numeric_limits<long>::min())
  //      or *this > bigint (numeric_limits<long>::max()))
  //      throw range_error ("bigint__to_long: out of range");

    long ret = 0;
    long expnt = 0, mult = 0;

    for (int i = 1; i <= this->big_value.size() - 1; i++){
        expnt = this->big_value.at(i) - 48;
        mult = pow(10, i - 1);
        expnt *= mult;
        ret += expnt;
    }

    return ret;
}

bool abs_less (const bigint& left, const bigint& right) {
    int lDigits = left.big_value.size();
    int rDigits = right.big_value.size();
    //cout << "most significant digit of " << left.bigToString() 
    //     << " is " << left.big_value.front << "?";
    if ( lDigits < rDigits){
    // If there are fewer digits in left than right
        return true;
    }
    if (lDigits > rDigits){
        return false;
    }
    else if ( lDigits == rDigits){
        int maxSize = left.big_value.size() - 1;
        if (left.big_value.at(maxSize) > 
            right.big_value.at(maxSize))
            return false;
        if (left.big_value.at(maxSize) < 
            right.big_value.at(maxSize))
            return true;
    }
}

//
// Multiplication algorithm.
//
bigint operator* (const bigint& left, const bigint& right) {
//   return left.long_value * right.long_value;
    bool neg;
    bigint bigger, smaller;
    bigint twos(1);
    twos.negative = false;
    bigint ret; 

    if ((left.negative == true && right.negative == true) 
    || (left.negative == false && right.negative == false)){
        ret.negative = false;
        ret.big_value.push_back(' ');
        neg = false;
    }else{
        ret.negative = true;
        ret.big_value.push_back('_');
        neg = true;
    }

    if (abs_less(left, right)){
        bigger = right;
        smaller = left;
        bigger.negative = false;//right.negative;
        smaller.negative = false;//left.negative;
    }else{
        bigger = left;
        smaller = right;
        bigger.negative = false;//left.negative;
        smaller.negative = false;//right.negative;
    }

    stack <bigint> stk1;
    stack <bigint> stk2;

    while(abs_compare(twos, bigger) == 2){
        stk1.push(twos);
        stk2.push(smaller);
        smaller = smaller.multiply_by_2();
        twos = twos.multiply_by_2();
    }

    while(!stk1.empty() || !stk2.empty()){
        twos = stk1.top();
        smaller = stk2.top();
        stk1.pop();
        stk2.pop();
        if (abs_compare(twos, bigger) == 2){
            bigger = bigger - twos;
            ret = ret + smaller;
        }
    }

    ret = ret + smaller;
    if (neg){
        ret.big_value[0] = '_';
        ret.negative = true;
    }
    return (ret);
}


//returns 0: left == right
//returns 1: left > right;
//returns 2: left < right;
int abs_compare(const bigint& left, const bigint& right){
    if (left.big_value.size() < right.big_value.size())
        return 2;
    if(left.big_value.size() > right.big_value.size())
        return 1;

    int maxSize = left.big_value.size() - 1;
    for (int i = maxSize; i>= 1; i--){
        if (left.big_value.at(i) > right.big_value.at(i))
            return 1;
    if (left.big_value.at(i) < right.big_value.at(i))
        return 2;
    }
    return 0;

}
//
// Division algorithm.
//

bigint bigint::multiply_by_2 () {
    bigint ret = (*this);
    ret = ret + *this;
    return ret;
}

void divide_by_2 (bigint::unumber& unumber_value) {
   unumber_value /= 2;
}

bigint::quot_rem divide (const bigint& left, const bigint& right) {
    //   return left.long_value * right.long_value;
    bool neg; 
    bigint bigger, smaller;
    bigint twos(1);
    twos.negative = false;
    bigint ret;

    if (right.big_value.at(1) == '0')
        throw domain_error("divide by 0");

    if ((left.negative == true && right.negative == true) 
    || (left.negative == false && right.negative == false)){
        ret.negative = false;
        ret.big_value.push_back(' ');
        neg = false;
    }else{
        ret.negative = true;
        ret.big_value.push_back('_');
        neg = true;
    }

    if (abs_less(left, right)){
        bigger = right;
        smaller = left;
        bigger.negative = false;
        smaller.negative = false;
    }else{
        bigger = left;
        smaller = right;
        bigger.negative = false;
        smaller.negative = false;
    }
    
    stack <bigint> stk1;
    stack <bigint> stk2;

    while(abs_compare(smaller, bigger) == 2){
        stk1.push(twos);
        stk2.push(smaller);
        smaller = smaller.multiply_by_2();
        twos = twos.multiply_by_2();
    }

    while(!stk1.empty() || !stk2.empty()){
        twos = stk1.top();
        smaller = stk2.top();
        stk1.pop();
        stk2.pop();
        if (abs_compare(smaller, bigger) == 2){
            bigger = bigger - smaller;
            ret = ret + twos;
        }
    }
    if (neg){
        ret.big_value[0] = '_';
        ret.negative = true;
    }

    if (abs_compare(bigger, right) == 0){
        bigger.big_value.clear();
        bigger.big_value.push_back(' ');
        bigger.big_value.push_back('0');
    }

    if (ret.big_value.size() == 1){
        ret.big_value.push_back('1');
        bigger.big_value.clear();
        bigger.big_value.push_back(' ');
        bigger.big_value.push_back('0');
    }

    ret = ret + twos;

    return {ret, bigger};
/* if (right == 0) throw domain_error ("divide by 0");
   using unumber = unsigned long;
   static unumber zero = 0;
   if (right == 0) throw domain_error ("bigint::divide");
   unumber divisor = right.long_value;
   unumber quotient = 0;
   unumber remainder = left.long_value;
   unumber power_of_2 = 1;
   while (abs_less (divisor, remainder)) {
      multiply_by_2 (divisor);
      multiply_by_2 (power_of_2);
   }
   while (abs_less (zero, power_of_2)) {
      if (not abs_less (remainder, divisor)) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divide_by_2 (divisor);
      divide_by_2 (power_of_2);
   }
   return {quotient, remainder};
   */
}

bool dominantSign (const bigint& left, const bigint& right){
// Returns the sign of the larger absolute bigint value
   if(abs_less(left, right)){
      return right.negative;
   }
   return left.negative;
}

bigint operator/ (const bigint& left, const bigint& right) {
   return divide(left, right).first;
}

bigint operator% (const bigint& left, const bigint& right) {
   return divide (left, right).second;
}

bool operator== (const bigint& left, const bigint& right) {
//   return left.long_value == right.long_value;
    int size = left.big_value.size();
    for (int i = 0; i < size; i++){
        if (left.big_value.at(i) != right.big_value.at(i))
            return false;
    }
    return true;
}

bool operator< (const bigint& left, const bigint& right) {
//   return left.long_value < right.long_value;
    if (left.negative == false && right.negative == true)
        return false;
    if (left.negative == true && right.negative == false)
        return true;
    if (left.negative == false && right.negative == false){
        if (left.big_value.size() < right.big_value.size())
            return true;
        if (left.big_value.size() > right.big_value.size())
            return false;

        if (left.big_value.size() == right.big_value.size()){
            int maxSize = left.big_value.size() - 1;
            if (left.big_value.at(maxSize) > 
                right.big_value.at(maxSize))
                return false;
            if (left.big_value.at(maxSize) < 
                right.big_value.at(maxSize))
                return true;
            if (left.big_value.at(maxSize) == 
                right.big_value.at(maxSize))
                return false;
        }
    }

    if (left.negative == true && right.negative == true){ //both neg
        if (left.big_value.size() < right.big_value.size())
            return false;
        if (left.big_value.size() > right.big_value.size())
            return true;

        if (left.big_value.size() == right.big_value.size()){ 
            int maxSize = left.big_value.size() - 1;
            if (left.big_value.at(maxSize) > 
                right.big_value.at(maxSize))
                return true;
            if (left.big_value.at(maxSize) < 
                right.big_value.at(maxSize))
                return false;
            if (left.big_value.at(maxSize) == 
                right.big_value.at(maxSize))
                return false;
        }
    }  
}

ostream& operator<< (ostream& out, const bigint& that) {
    if (that.negative)
        out<<'-';

    int count = 0;
    for (int i = that.big_value.size() - 1; i> 0; i--){
        count++;
        if(count == 70){
            out<<endl;
            count = 1;
        }
    out<<that.big_value.at(i);
    }
    return out;
}


bigint pow (const bigint& base, const bigint& exponent) {
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   if (base == 0) return 0;
   bigint base_copy = base;
   long expt = exponent.to_long();
   bigint result = 1;
   if (expt < 0) {
      base_copy = 1 / base_copy;
      expt = - expt;
   }
   while (expt > 0) {
      if (expt & 1) { //odd
         result = result * base_copy;
         --expt;
      }else { //even
         base_copy = base_copy * base_copy;
         expt /= 2;
      }
   }
    cout<<"result: "<<result<<endl;
    DEBUGF ('^', "result = " << result);
    return result;
}