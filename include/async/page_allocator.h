//
// Created by Darwin Yuan on 2020/9/28.
//

#ifndef ASYNC_PAGE_ALLOCATOR_H
#define ASYNC_PAGE_ALLOCATOR_H

#include <cstddef>
#include <memory>
#include <deque>
#include <algorithm>
#include <iostream>

constexpr std::size_t PAGE_ALIGNMENT = 4 * 1024 * 1024;
constexpr std::size_t PAGE_SIZE = PAGE_ALIGNMENT;
constexpr std::size_t CACHE_LINE_SIZE = 64;
constexpr std::size_t MAX_SIZE_OF_FREE_BLOCK = PAGE_ALIGNMENT - CACHE_LINE_SIZE;

struct alignas(PAGE_ALIGNMENT) page {
   void* p_free{start_};
   std::size_t size_of_free_block_ {MAX_SIZE_OF_FREE_BLOCK};
   std::size_t actual_allocated_size_{0};
   void* boundary = start_ + PAGE_SIZE;
   alignas(CACHE_LINE_SIZE) char start_[0];

   auto alloc(std::size_t size, std::align_val_t alignment) noexcept -> void* {
      auto p = std::align(size, static_cast<std::size_t>(alignment), p_free, size_of_free_block_);
      if(p == nullptr) return p;
      p_free = (char*)p_free + size;
      size_of_free_block_ -= size;
      actual_allocated_size_ += size;
      return p;
   }

   auto contains(void* p) const noexcept -> bool {
      return p >= start_ && p < boundary;
   }

   auto free(std::size_t size) noexcept -> bool {
      actual_allocated_size_ -= size;
      return actual_allocated_size_ == 0;
   }
};

struct page_allocator {
   auto alloc(std::size_t size, std::align_val_t alignment) noexcept -> void* {
      if(size > MAX_SIZE_OF_FREE_BLOCK) {
         return ::operator new(size, alignment);
      }
      auto r = alloc_local(size, alignment);
      if(r != nullptr) return r;

      return ::operator new(size, alignment);
   }

   auto free(void* p, std::size_t size) noexcept -> void {
      auto result = std::find_if(pages_.begin(), pages_.end(), [=](auto& page) {
         return page->contains(p);
      });
      if(result == pages_.end()) {
         ::operator delete(p);
      } else if((*result)->free(size)) {
         std::cout << "page free" << std::endl;
         pages_.erase(result);
      }
   }

private:
   auto alloc_new_page() noexcept -> page* {
      auto new_page = new (std::nothrow) page{};
      if(new_page == nullptr) return nullptr;
      pages_.emplace_back(new_page);
      std::cout << "page alloc" << std::endl;
      return new_page;
   }

   auto alloc_local(std::size_t size, std::align_val_t alignment) noexcept -> void* {
      if(pages_.empty()) alloc_new_page();

      auto& current = pages_.back();
      auto r = current->alloc(size, alignment);
      if(r != nullptr) return r;

      auto p = alloc_new_page();
      if(p == nullptr) return nullptr;

      return p->alloc(size, alignment);
   }

private:
   std::deque<std::unique_ptr<page>> pages_;
};

#endif //ASYNC_PAGE_ALLOCATOR_H
