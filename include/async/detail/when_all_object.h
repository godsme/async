//
// Created by Darwin Yuan on 2020/9/27.
//

#ifndef ASYNC_WHEN_ALL_OBJECT_H
#define ASYNC_WHEN_ALL_OBJECT_H

#include <async/detail/future_object.h>
#include <async/detail/tuple_trait_t.h>

namespace detail {

   template<typename ... Xs>
   struct when_all_object : detail::future_object<tuple_trait_t<Xs...>>, future_observer {
      using super = detail::future_object<tuple_trait_t<Xs...>>;

      when_all_object(future_context& context, future<Xs>& ... objects) noexcept
         : super{context}
         , objects_{objects.object_ ...}
         , num_of_pending_{sizeof...(Xs)} {
         init(std::index_sequence_for<Xs...>{});
      }

      using tuple_index = tuple_index_t<Xs...>;

      auto valid() const noexcept -> bool { return valid_; }

      auto get_context() const noexcept -> future_context* {
         return std::get<0>(objects_)->get_context();
      }

      template<std::size_t ... I>
      auto on_done(std::index_sequence<I...>) noexcept -> void {
         super::set_value(std::make_tuple(std::get<I>(objects_)->get_value() ...));
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
         if(num_of_pending_ > 0) return;
         on_done(tuple_index{});
         clear(std::index_sequence_for<Xs...>{});
      }

   private:
      template<std::size_t I>
      auto init() noexcept -> void {
         auto &&object = std::get<I>(objects_);
         if (object->ready()) num_of_pending_--;
         else object->add_observer(this);
      }

      template<std::size_t ... I>
      auto init(std::index_sequence<I...>) noexcept -> void {
         valid_ = (std::get<I>(objects_) && ...);
         if(valid_) (init<I>(), ...);
      }

      template<std::size_t ... I>
      auto clear(std::index_sequence<I...>) noexcept -> void {
         (std::get<I>(objects_).release(), ...);
      }

      template<std::size_t ... I>
      auto detach(std::index_sequence<I...> seq) noexcept -> void {
         (std::get<I>(objects_)->deregister_observer(this, status_t::ok), ...);
         clear(seq);
      }

   private:
      std::tuple<shared_ptr<future_object<Xs>>...> objects_;
      std::size_t num_of_pending_{0};
      bool valid_{true};
   };
}

#endif //ASYNC_WHEN_ALL_OBJECT_H
