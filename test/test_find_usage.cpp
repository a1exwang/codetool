#include "external_struct.h"

int x;

struct Type {
  int a;
  Type(int a) :a(a) {}
  int add(int a, int b) {
    return a + b + this->a;
  }
  int operator()(int a) {
    return a * a;
  }
  static void static_run(int a) {
  }
};

int mul(int a, int b) {
  return a * b;
}

int main() {
  Type obj(234);
  int a = 1;
  int b = 2;

  obj.add(a, b);
  (&obj)->add(b, mul(a, a));
  Type::static_run(x);

  obj(222);
}
