#pragma once

int Sum(int a, int b) { return a + b; }

class SimpleClass {
 public:
  SimpleClass(int n) : m_n(n) {}

  int Result(int d, int m) { return m_n * d / m; }

  double DoNothing(double a) { return a; }

  static int Sum(int a, int b) { return a + b; }

 private:
  int m_n;
};