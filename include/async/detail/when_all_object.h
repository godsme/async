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

   template<std::size_t I, typename SEQ, typename ... Xs>
   struct filter_out;

   template<std::size_t I, std::size_t ...Is, typename H, typename ... Xs>
   struct filter_out<I, std::index_sequence<Is...>, H, Xs...> {
      using type = typename filter_out<I+1, std::index_sequence<Is..., I>, Xs...>::type;
   };

   template<std::size_t I, std::size_t ...Is, typename ... Xs>
   struct filter_out<I, std::index_sequence<Is...>, void, Xs...> {
      using type = typename filter_out<I+1, std::index_sequence<Is...>, Xs...>::type;
   };

   template<std::size_t I, typename SEQ>
   struct filter_out<I, SEQ> {
      using type = SEQ;
   };

   template<typename T> struct wrapper_t {
      using type = T;
   };

   template <typename ... Xs>
   using tuple_index_t = typename filter_out<0, std::index_sequence<>, Xs...>::type;

   template<typename ... Xs, std::size_t ... Is>
   auto tuple_trait(std::tuple<Xs...> t, std::index_sequence<Is...>)
      -> std::tuple<std::decay_t<decltype(std::declval<typename std::decay_t<decltype(std::get<Is>(t))>::type>())> ...>;

   template<typename ... Xs>
   using tuple_trait_t = decltype(tuple_trait(std::tuple<wrapper_t<Xs>...>{}, tuple_index_t<Xs...>{}));
}

namespace detail {
   template<typename ... Xs>
   struct when_all_object : detail::future_object<tuple_trait_t<Xs...>>, future_observer {
      using super = detail::future_object<tuple_trait_t<Xs...>>;

      when_all_object(future_context& context, future<Xs>& ... objects)
         : super{context}
         , objects_{objects.object_ ...}
         , num_of_pending_{sizeof...(Xs)}{
         init(std::index_sequence_for<Xs...>{});
      }

      using tuple_index = tuple_index_t<Xs...>;

      auto valid() const noexcept -> bool { return valid_; }

      auto get_context() const noexcept -> future_context* {
         return std::get<0>(objects_)->get_context();
      }

      template<std::size_t ... I>
      auto on_done(std::index_sequence<I...>) {
         super::set_value(std::make_tuple((std::get<I>(objects_)->get_value(), ...)));
         super::commit();
      }

      auto on_future_ready() noexcept -> void override {
         --num_of_pending_;
         check_done();
      }

      auto on_future_fail(status_t cause) noexcept -> void override {
         super::on_fail(cause);
         super::commit();
         clear(std::index_sequence_for<Xs...>{});
      }

      auto cancel(status_t cause) noexcept -> void override {
         super::cancel(cause);
         detach(std::index_sequence_for<Xs...>{});
      }

      auto check_done() noexcept -> void {
         if(num_of_pending_ == 0) {
            on_done(tuple_index{});
            clear(std::index_sequence_for<Xs...>{});
         }
      }

   private:
      template<std::size_t I>
      auto init() {
         auto &&object = std::get<I>(objects_);
         if (object->ready()) {
            num_of_pending_--;
         } else {
            object->add_observer(this);
         }
      }

      template<std::size_t ... I>
      auto init(std::index_sequence<I...>) {
         valid_ = (std::get<I>(objects_) && ...);
         if(valid_) {
            (init<I>(), ...);
         }
      }

      template<std::size_t ... I>
      auto clear(std::index_sequence<I...>) {
         (std::get<I>(objects_).reset(), ...);
      }

      template<std::size_t ... I>
      auto detach(std::index_sequence<I...> seq) {
         (std::get<I>(objects_)->deregister_observer(this, status_t::ok), ...);
         clear(seq);
      }

   private:
      std::tuple<std::shared_ptr<future_object<Xs>>...> objects_;
      std::size_t num_of_pending_{0};
      bool valid_{true};
   };
}


#endif //ASYNC_WHEN_ALL_OBJECT_H
