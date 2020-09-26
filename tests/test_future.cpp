//
// Created by Darwin Yuan on 2020/9/26.
//

#include <catch.hpp>
#include <async/promise.h>
#include <async/future.h>

namespace {
   SCENARIO("auto registry") {
      future_context context;
      promise<int> p;
      {
         auto future = p.get_future(context);
         REQUIRE(future.valid());
         REQUIRE(context.empty());
      }

      REQUIRE_FALSE(context.empty());
   }

   SCENARIO("only registry leaves") {
      future_context context;

      promise<int> p;
      {
         auto f = p.get_future(context)
         .map([](int value) { return value + 10; })
         .map([](int value) -> bool { return value > 20; })
         .map([&](bool value) -> future<long> {
            promise<long> p1;
            return p1.get_future(context);
         });

         REQUIRE(context.size() == 0);
      }

      REQUIRE(context.size() == 1);
   }

   SCENARIO("promise set") {
      future_context context;

      std::optional<long> value_set;
      promise<int> p;
      promise<long> p1;
      {
         auto f = p.get_future(context)
            .map([](int value) { return value + 10; })
            .map([](int value) -> bool { return value > 20; })
            .map([&](bool value) -> future<long> {
               return p1.get_future(context);})
            .map([&](long value){ value_set = value; });

         REQUIRE(context.size() == 0);
      }

      p.set_value(10);
      p.commit();

      REQUIRE_FALSE(value_set);
      REQUIRE_FALSE(p.valid());

      p1.set_value(20);
      p1.commit();

      REQUIRE(value_set);
      REQUIRE(value_set.value() == 20);

      REQUIRE(context.size() == 0);
   }
}