// Global module fragment where #includes can happen
module;

#include <string>
#include <iostream>

#include <fmt/core.h>

// first thing after the Global module fragment must be a module command
export module Foo;

export class foo {
public:
  foo();
  ~foo();
  void helloworld(int ans = 42);
};

foo::foo() = default;
foo::~foo() = default;

void foo::helloworld(int ans)
{ 
  std::cout << fmt::format("The answer is {}.", ans) << std::endl;
}
