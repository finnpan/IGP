// copied from: https://github.com/dmlc/mshadow

#pragma once

// Example code, expression template
// with binary operator definition and extension
// for simplicity, we use struct and make all members public

// this is expression, all expressions must inheritate it,
// and put their type in subtype
template<typename SubType>
struct Exp{
  // returns const reference of the actual type of this expression
  inline const SubType& self(void) const {
    return *static_cast<const SubType*>(this);
  }
};

// binary operators
struct mul{
  inline static float Map(float a, float b) {
    return a * b;
  }
};

// binary add expression
// note how it is inheritates from Exp
// and put its own type into the template argument
template<typename OP, typename TLhs, typename TRhs>
struct BinaryMapExp: public Exp<BinaryMapExp<OP, TLhs, TRhs> >{
  const TLhs& lhs;
  const TRhs& rhs;
  BinaryMapExp(const TLhs& lhs, const TRhs& rhs)
      :lhs(lhs), rhs(rhs) {}
  // evaluation function, evaluate this expression at position i
  inline float Eval(int i) const {
    return OP::Map(lhs.Eval(i), rhs.Eval(i));
  }
};
// no constructor and destructor to allocate and de-allocate memory
// allocation done by user
struct Vec: public Exp<Vec>{
  int len;
  float* dptr;
  Vec(void) {}
  Vec(float *dptr, int len)
      : len(len), dptr(dptr) {}
  // here is where evaluation happens
  template<typename EType>
  inline Vec& operator=(const Exp<EType>& src_) {
    const EType &src = src_.self();
    for (int i = 0; i < len; ++i) {
      dptr[i] = src.Eval(i);
    }
    return *this;
  }
  // evaluation function, evaluate this expression at position i
  inline float Eval(int i) const {
    return dptr[i];
  }
};
// template binary operation, works for any expressions
template<typename OP, typename TLhs, typename TRhs>
inline BinaryMapExp<OP, TLhs, TRhs>
F(const Exp<TLhs>& lhs, const Exp<TRhs>& rhs) {
  return BinaryMapExp<OP, TLhs, TRhs>(lhs.self(), rhs.self());
}

template<typename TLhs, typename TRhs>
inline BinaryMapExp<mul, TLhs, TRhs>
operator*(const Exp<TLhs>& lhs, const Exp<TRhs>& rhs) {
  return F<mul>(lhs, rhs);
}

// user defined operation
struct maximum{
  inline static float Map(float a, float b) {
    return a > b ? a : b;
  }
};
