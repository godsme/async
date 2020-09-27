//
// Created by Darwin Yuan on 2020/9/27.
//

#ifndef ASYNC_WHEN_ALL_OBJECT_H
#define ASYNC_WHEN_ALL_OBJECT_H

#include <async/detail/future_object.h>
#include <tuple>

namespace detail {
   template<typename ... Xs>
   struct type_list {};

   template<typename INPUT, typename ... Ys>
   struct filter_out;

   template<typename H, typename ... Xs, typename ... Ys>
   struct filter_out<type_list<H, Xs...>, Ys...> {
      using type = typename filter_out<type_list<Xs...>, H, Ys...>::type;
   };

   template<typename ... Xs, typename ... Ys>
   struct filter_out<type_list<void, Xs...>, Ys...> {
      using type = typename filter_out<type_list<Xs...>, Ys...>::type;
   };

   template<typename ... Ys>
   struct filter_out<type_list<>, Ys...> {
      using type = std::tuple<Ys...>;
   };

   template <typename ... Xs>
   using tuple_trait_t = typename filter_out<type_list<Xs...>>::type;
}


namespace detail {
   template<typename ... Xs>
   struct when_all_object : detail::future_object<tuple_trait_t<Xs...>>, future_observer {
      when_all_object(std::shared_ptr<future_object<Xs>> ... objects)
         : objects_{std::move(objects)...}
      {}

   private:
      std::tuple<std::shared_ptr<future_object<Xs>>...> objects_;
   };
}


#endif //ASYNC_WHEN_ALL_OBJECT_H
