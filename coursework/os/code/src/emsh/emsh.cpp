#include <unistd.h>

#include <iostream>

#include "repl.h"
#include "login.h"
#include "repl.h"

int main() {
  emsh::REPL context = emsh::login();
  std::cout << "Welcome, type `help` for more information." << std::endl;
  do {
    context.do_loop();
  } while (!context.is_exit());
  return 0;
}