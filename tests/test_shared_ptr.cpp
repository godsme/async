//
// Created by Darwin Yuan on 2020/9/28.
//

#include <catch.hpp>
#include <async/memory/shared_ptr.h>

namespace {
   struct object {
      int a;
      double b;
      char c;
   };

   SCENARIO("make shared") {
      page_allocator allocator;
      auto p1 = make_shared<int>(allocator, 20);
      REQUIRE(p1 != nullptr);
      REQUIRE(*p1 == 20);

      auto p2 = make_shared<long long>(allocator, 10);
      REQUIRE(p2 != nullptr);
      REQUIRE(*p2 == 10);

      auto p3 = make_shared<object>(allocator, 10, 4.9, 'a');
      REQUIRE(p3 != nullptr);
      REQUIRE(p3->a == 10);
      REQUIRE(p3->b == 4.9);
      REQUIRE(p3->c == 'a');

      REQUIRE(p3.unique());
      auto p4 = p3;
      REQUIRE_FALSE(p4.unique());
      REQUIRE(p4.use_count() == 2);

      auto p5 = std::move(p4);
      REQUIRE_FALSE(p5.unique());
      REQUIRE(p5.use_count() == 2);
      REQUIRE(!p4);
   }
}