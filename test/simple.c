#include <iostream>
struct Type {
  int a;
};

struct Type t;

int add(int a, int b) {
  return a + b + t.a;
}

int main() {
  return add(add(2, 3), 10);
}
