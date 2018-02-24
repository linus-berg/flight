#ifndef MATH_H_
#define MATH_H_
int abs (int n) {
    const int ret[2] = { n, -n };
    return ret [n<0];
}
#endif
