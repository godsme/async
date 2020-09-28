//
// Created by Darwin Yuan on 2020/9/28.
//

#include <catch.hpp>
#include <async/memory/make_shared.h>
#include <async/memory/weak_ptr.h>

namespace {
   struct object {
      int a;
      double b;
      char c;
   };

   SCENARIO("make shared") {
      page_allocator allocator;
      auto p1 = alloc_shared<int>(allocator, 20);
      REQUIRE(p1 != nullptr);
      REQUIRE(*p1 == 20);

      auto p2 = alloc_shared<long long>(allocator, 10);
      REQUIRE(p2 != nullptr);
      REQUIRE(*p2 == 10);

      auto p3 = alloc_shared<object>(allocator, 10, 4.9, 'a');
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

   SCENARIO("weak ptr") {
      weak_ptr<bool> wp;
      REQUIRE(wp.expired());

      page_allocator allocator;
      weak_ptr<int> wp1 = alloc_shared<int>(allocator, 20);
      auto p1 = wp1.lock();
      REQUIRE_FALSE(p1);
      REQUIRE(wp1.expired());

      auto p2 = alloc_shared<object>(allocator, 10, 4.9, 'a');
      weak_ptr<object> wp2 = p2;
      auto p3 = wp2.lock();
      REQUIRE(p2);
      REQUIRE(p3);
      REQUIRE(p2.use_count() == 2);
      REQUIRE_FALSE(wp2.expired());
   }

   SCENARIO("alloc pages") {
      page_allocator allocator;

      shared_ptr<object> sps[100000];
      for(int i=0; i<100000; i++) {
         sps[i] = alloc_shared<object>(allocator, 10, 4.9, 'a');
         weak_ptr<object> wp = sps[i];
         REQUIRE(sps[i].use_count() == 1);
         auto p3 = wp.lock();
         REQUIRE(p3);

         auto wp2 = wp;
         REQUIRE(sps[i].use_count() == 2);
         auto p4 = wp2.lock();
         REQUIRE(p4);

         REQUIRE(sps[i]);
         REQUIRE(sps[i].use_count() == 3);

         REQUIRE_FALSE(wp2.expired());
         auto wp3 = std::move(wp2);
         REQUIRE(wp2.expired());
         REQUIRE_FALSE(wp3.expired());

         auto p5 = wp3.lock();
         REQUIRE(p5);
         REQUIRE(sps[i].use_count() == 4);

         REQUIRE_FALSE(wp.expired());
      }
   }
}