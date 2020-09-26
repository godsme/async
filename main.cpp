#include <iostream>
#include <async/future.h>
#include <async/promise.h>

using status_t = uint64_t;

future_context context;
using failure_handler = std::function<auto (status_t) -> void>;

int main() {
   future<int> f{};

   f.map([](auto) -> future<long> { return future<long>{}; });

   if(context.empty()) {
      std::cout << "OK" << std::endl;
   }

   promise<int> p;
   {
      auto future = p.get_future(context);
      if(future.valid()) {
         std::cout << "OK" << std::endl;
      }
   }

   if(!context.empty()) {
      std::cout << "OK" << std::endl;
   }

   return 0;
}
