#include <iostream>
#include <async/future.h>

using status_t = uint64_t;

using failure_handler = std::function<auto (status_t) -> void>;

int main() {
   future<int> f{};

   f.map([](auto) -> void {});

   return 0;
}
