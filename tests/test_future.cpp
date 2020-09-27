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
            .then([](int value) { return value + 10; })
            .then([](int value) -> bool { return value > 20; })
            .then([&](bool value) -> future<long> {
               promise<long> p1;
               return p1.get_future(context);
            });

         REQUIRE(context.size() == 0);
      }

      REQUIRE(context.size() == 1);
   }

   struct remote_calc {
      auto set_value(long value) {
         if(cond) {
            p.set_value((long)(value + 10));
         } else {
            p.set_value((long)(value - 10));
         }
         p.commit();
      }

      auto on_fail(status_t cause) {
         p.on_fail(cause);
         p.commit();
      }

      bool cond{false};
      promise<long> p;
   };

   SCENARIO("promise set") {
      future_context context;

      std::optional<long> value_set;

      promise<long> p1;


      promise<int> p2;
      remote_calc  calc;
      {
         auto f1 = p1.get_future(context)
            .then([&](long value) { value_set = value; });

         auto f2 = p2.get_future(context)
            .then([](int value) { return value + 10; })
            .then([](int value) -> bool { return value > 20; })
            .then([&](bool value) -> future<long> {
               calc.cond = value;
               return calc.p.get_future(context);
            })
            .sink(p1);

         REQUIRE(context.size() == 0);
      }

      p2.set_value(10);
      p2.commit();

      REQUIRE_FALSE(value_set);
      REQUIRE_FALSE(p2.valid());

      calc.set_value(20);

      p1.commit();

      REQUIRE(value_set.has_value());
      REQUIRE(value_set.value() == 10);

      REQUIRE(context.size() == 0);
   }

   SCENARIO("fail") {
      future_context context;

      std::optional<long> value_set;
      std::optional<status_t> fail_set;
      promise<long> p1;
      promise<int> p2;
      remote_calc  calc;
      {
         auto f1 = p1.get_future(context)
            .then([&](long value) { value_set = value; })
            .fail([&](auto cause) {
               REQUIRE_FALSE(fail_set);
               fail_set = cause; });

         auto f2 = p2.get_future(context)
            .then([](int value) -> int { return value + 10; })
            .then([](int value) -> bool { return value > 20; })
            .then([&](bool value) -> future<long> {
               calc.cond = value;
               return calc.p.get_future(context); });

         f2.sink(p1);
      }

      p2.set_value(10);
      p2.commit();

      REQUIRE_FALSE(value_set);
      REQUIRE_FALSE(p2.valid());

      calc.on_fail(status_t::out_of_memory);
      p1.commit();

      REQUIRE_FALSE(value_set.has_value());
      REQUIRE(fail_set.has_value());
      REQUIRE(fail_set.value() == status_t::out_of_memory);

      REQUIRE(context.size() == 0);
   }

   SCENARIO("cancel") {
      future_context context;

      std::optional<long> value_set;
      std::optional<status_t> fail_set;
      promise<long> p1;
      promise<int> p2;
      remote_calc  calc;
      {
         auto f1 = p1.get_future(context)
            .then([&](long value) { value_set = value; })
            .fail([&](auto cause) {
               REQUIRE_FALSE(fail_set);
               fail_set = cause; });

         auto f2 = p2.get_future(context)
            .then([](int value) -> int { return value + 10; })
            .then([](int value) -> bool { return value > 20; })
            .then([&](bool value) -> future<long> {
               calc.cond = value;
               return calc.p.get_future(context); });

         f2.sink(p1).cancel(status_t::cancelled);
      }

      p1.commit();
      REQUIRE(fail_set.has_value());
      REQUIRE(*fail_set == status_t::cancelled);
      REQUIRE_FALSE(p2.valid());
      REQUIRE(context.size() == 0);
   }
}