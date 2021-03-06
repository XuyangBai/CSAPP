/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students: 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,

the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
    return ~ ( (~x) | (~y) );
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  int rdis = n<<3;
  int ans = 0;
  ans = (x>>rdis) & 0xff;
  return ans;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  // x + (~x) == -1
  // ~0 = -1 
  int ones = (1 << n) + (~0); 
  return (x >> n) & ~( ones << (32 + 1 + ~n ));
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
  int _mask1 = (0x55) | (0x55<<8);
  int mask1 = _mask1 | (_mask1<<16); //01010101 * 4
  int _mask2 = (0x33) | (0x33<<8);
  int mask2 = _mask2 | (_mask2<<16); //00110011 * 4
  int _mask3 = (0x0f) | (0x0f<<8);
  int mask3 = _mask3 | (_mask3<<16); //00001111 * 4
  int mask4 = 0xff | (0xff<<16); //00000000 11111111 * 2
  int mask5 = 0xff | (0xff<<8); //00000000 00000000 11111111 11111111
  
  x = (x & mask1) + ( (x>>1) & mask1);
  x = (x & mask2) + ( (x>>2) & mask2);
  x = (x & mask3) + ( (x>>4) & mask3);
  x = (x & mask4) + ( (x>>8) & mask4);
  x = (x & mask5) + ( (x>>16) & mask5);
 
  return x;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
// 就是判断x是否为0
int bang(int x) {
  int highest = (x >> 31) | ( ( (~x)+1) >> 31);
  return  ~highest & 1;
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return 1<<31;
  // 下面这个写法说明还是没有理解2s complement啊老铁
  // return ~(1 << 31) + 1;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  // 超过max ops
  //int n_minus_1 = n + (~1) + 1;
  //int tmin = ~(1 << n_minus_1) + 1;
  //int tmax = (1 << n_minus_1) + (~1) + 1;
  //return isLessOrEqual(tmin, x) & isLessOrEqual(x, tmax);
  // 正数如果能用nbit表示，则第n位一定是0，即第n到第32位都是0
  // 负数如果能用nbit表示，则第n位一定是1，即第n到第32位都是1
   return !(((x >> (n + ~0)) + 1) >> 1);
  
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
   int isNegative = (x >> 31) & 1;
   int factor = !!(((1 << n) + ~0) & x);
   return (x>>n) + (isNegative & factor);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return (~x) + 1;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
   // 此方法对于x = -2**31的时候会溢出
   // return ((1 + (~x)) >> 31 ) & 1;
   int isNegative = (x >> 31) & 1;
   //int isZero = ~( (x >> 31) | ( ( (~x)+1) >> 31) ) & 1;
   int isZero = !x;
   return !isZero & !isNegative;
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  int sign_x = x >> 31;
  int sign_y = y >> 31;
  // 如果xy符号相同 x <= y 等价于 x-y-1 < 0 即 x + ~y >> 31
  int equal = (!(sign_x ^ sign_y)) & ((~y + x) >> 31);
  // 如果符号不同 x <= y 等价于 sign_x & !sign_y
  int notEqual = (sign_x ^ sign_y) & sign_x  & !sign_y;

  return equal | notEqual;
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
  int ans = 0;
  ans = ans + ((!!(x >> 16)) << 4);
  ans = ans + ((!!(x >> (8 + ans))) << 3);
  ans = ans + ((!!(x >> (4 + ans))) << 2);
  ans = ans + ((!!(x >> (2 + ans))) << 1);
  ans = ans + ((!!(x >> (1 + ans))) << 0);
  return ans;
}
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
   int exp_all_1 = !((( uf >> 23) + 1) << 24);
   int m_not_all_zero = !!(uf & 0x007fffff);
   if (exp_all_1 & m_not_all_zero){
       return uf; 
   }
   return uf ^ 0x80000000;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&.gh; also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  if(x == 0) return 0; 
  int sign = (1 << 31) & x;
  if(sign){
     x = -x;
  }
   // highest 是最高位1的位置
  int highest = ilog2(x) + 1;
  x = x ^ (1 << (highest -1));
  printf("highest : %d\n", highest);
  if(highest > 24){
    unsigned guard_bit = (1 << (highest - 24)) & x;
    unsigned round_bit = (1 << (highest - 25)) & x;
    unsigned sticky_bit = !!((round_bit - 1) & x);
    if(round_bit && sticky_bit){
        x = (x >> (highest - 24)) + 1;
    }else if(guard_bit && round_bit && !sticky_bit){
        x = (x >> (highest - 24)) + 1;
    }else{
        x = x >> (highest - 24);
    }
  }
  else{
      x = x << (24 - highest);
  }
  unsigned frac = x & 0x007fffff;
  unsigned exp =((highest - 1 + 127) << 23) & 0x7f800000;
  printf("x    : %.8x\n", x);
  printf("sign : %.8x\n", sign);
  printf("frac : %.8x\n", frac);
  printf("exp  : %.8x\n", exp);
  unsigned ans =  sign | exp | frac;
  return ans;
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  if(uf == 0 || uf == 0x80000000) return uf;
  unsigned tmp = uf & (0x7fffffff);
  if(tmp >= 0x7f800000) return uf;
  unsigned exp  = uf & 0x7f800000;
  unsigned frac = uf & 0x007fffff;;
  if( !exp ){
      exp  = (frac << 1) & 0x00800000;
      frac = (frac << 1) & 0x007fffff;       
  }else{
      exp  = (exp + 0x00800000) & 0x7f800000;
  }
  unsigned sign = uf & 0x80000000;
  unsigned ans = sign | exp | frac;
  return ans;
}

