// rak - Rakshasa's toolbox
// Copyright (C) 2005-2007, Jari Sundell
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// In addition, as a special exception, the copyright holders give
// permission to link the code of portions of this program with the
// OpenSSL library under certain conditions as described in each
// individual source file, and distribute linked combinations
// including the two.
//
// You must obey the GNU General Public License in all respects for
// all of the code used other than OpenSSL.  If you modify file(s)
// with this exception, you may extend this exception to your version
// of the file(s), but you are not obligated to do so.  If you do not
// wish to do so, delete this exception statement from your version.
// If you delete this exception statement from all source files in the
// program, then also delete it here.
//
// Contact:  Jari Sundell <jaris@ifi.uio.no>
//
//           Skomakerveien 33
//           3185 Skoppum, NORWAY

// priority_queue is a priority queue implemented using a binary
// heap. It can contain multiple instances of a value.

#ifndef RAK_PRIORITY_QUEUE_H
#define RAK_PRIORITY_QUEUE_H

#include <algorithm>
#include <functional>
#include <vector>

namespace rak {

template <typename Value, typename Compare, typename Equal, typename Alloc = std::allocator<Value> >
class priority_queue : public std::vector<std::pair<Value, uint64_t>, Alloc> {
public:
  using LocalValue = std::pair<Value, uint64_t>;
  using RealCompare = bool(*)(const LocalValue&, const LocalValue);
  RealCompare realCompare;

  typedef std::vector<LocalValue, Alloc>           base_type;
  typedef  Value&                     reference;
  typedef const Value&               const_reference;
  struct iterator :public std::iterator<std::random_access_iterator_tag, Value> {
    bool operator!=(const iterator& rhs) const {
      return base_ != rhs.base_;
    }
    bool operator==(const iterator& rhs) const {
      return base_ != rhs.base_;
    }
    typename base_type::iterator base() const {
      return base_;
    }
    int operator-(const iterator& rhs) const {
      return base_ - rhs.base_;
    }
    iterator operator+(int i) const {
      return base_ + i;
    }
    iterator& operator++() {
      ++base_;
      return *this;
    }
    const Value& operator*() const {
      return *base_;
    }

    iterator() = default;
    iterator(const typename base_type::iterator& b) : base_(b) {}

    typename base_type::iterator base_;
  };
  struct const_iterator {};
  typedef Value      value_type;

  uint64_t next = 0;

  iterator begin() { return iterator(base_type::begin()); }
  iterator end() { return iterator(base_type::end()); }
  using base_type::size;
  using base_type::empty;
  using base_type::clear;

  priority_queue(Compare l = Compare(), Equal e = Equal())
    : m_compare(l), m_equal(e), realCompare(
      
    ) {}

  const_reference top() const {
    return base_type::front().first;
  }

  void pop() {
    std::pop_heap(base_type::begin(), base_type::end(), realCompare);
    base_type::pop_back();
  }

  void push(const value_type& value) {
    base_type::push_back(std::make_pair(value, ++next));
    std::push_heap(base_type::begin(), base_type::end(), realCompare);
  }

  template <typename Key>
  iterator find(const Key& key) {
    return std::find_if(base_type::begin(), base_type::end(), 
      [&key, this](auto el) { return m_equal(key, el.first); });
  }

  template <typename Key>
  bool erase(const Key& key) {
    iterator itr = find(key);

    if (itr == end())
      return false;

    base_type::erase(itr.base());
    return true;
  }

  // Removes 'itr' from the queue. This assumes 'itr' has been
  // modified such that it has a higher priority than any other
  // element in the queue.
  void erase(iterator itr) {
//     std::push_heap(begin(), ++itr, m_compare);
//     pop();
    base_type::erase(itr.base());
    std::make_heap(begin(), end(), m_compare);
  }

private:
  Compare             m_compare;
  Equal               m_equal;
};

// Iterate while the top node has higher priority, as 'Compare'
// returns false.
template <typename Queue, typename Compare>
class queue_pop_iterator
  : public std::iterator<std::forward_iterator_tag, void, void, void, void> {
public:
  typedef Queue container_type;

  queue_pop_iterator() : m_queue(NULL) {}
  queue_pop_iterator(Queue* q, Compare c) : m_queue(q), m_compare(c) {}

  queue_pop_iterator& operator ++ ()                     { m_queue->pop(); return *this; }
  queue_pop_iterator& operator ++ (int)                  { m_queue->pop(); return *this; }

  typename container_type::const_reference operator * () { return m_queue->top(); }

  bool operator != (const queue_pop_iterator& itr)       { return !m_queue->empty() && !m_compare(m_queue->top()); }
  bool operator == (const queue_pop_iterator& itr)       { return m_queue->empty() || m_compare(m_queue->top()); }

private:
  Queue*  m_queue;
  Compare m_compare;
};

template <typename Queue, typename Compare>
inline queue_pop_iterator<Queue, Compare>
queue_popper(Queue& queue, Compare comp) {
  return queue_pop_iterator<Queue, Compare>(&queue, comp);
}

}

#endif
