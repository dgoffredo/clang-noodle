#include "field-access.h"

int main() {
  Foo foo;
  int x = foo.x;
  const Foo *f;
  return f->x == x;
}
