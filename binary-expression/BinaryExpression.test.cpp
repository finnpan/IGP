#include <cstdio>
#include "BinaryExpression.h"

const int n = 3;
int main(void) {
  float sa[n] = {1, 2, 3};
  float sb[n] = {2, 3, 4};
  float sc[n] = {3, 4, 5};
  Vec A(sa, n), B(sb, n), C(sc, n);
  // run expression, this expression is longer:)
  A = B * F<maximum>(C, B);
  for (int i = 0; i < n; ++i) {
    printf("%d:%f == %f * max(%f, %f)\n",
           i, A.dptr[i], B.dptr[i], C.dptr[i], B.dptr[i]);
  }
  return 0;
}
