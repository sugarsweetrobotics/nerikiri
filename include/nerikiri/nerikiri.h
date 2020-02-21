#pragma once

#include "process.h"
#include "operation.h"

namespace nerikiri {


  template<typename T, typename... R>
  std::function<T(T)> compose(std::function<T(T)> fst) {
    return fst;
  }

  template<typename T, typename... R>
  std::function<T(T)> compose(std::function<T(T)> fst, std::function<T(T)> snd, R... remain) {
    return compose<T>([fst, snd](T&& t) { 
		     return snd(fst(std::forward<T>(t)));
	       },
      remain...);
  }

  template<typename T, typename K, typename V>
  std::vector<T> map(std::map<K, V>& map, std::function<T(const K&,V&)> func) {
    std::vector<T> ret;
    for(auto& [k, v] : map) {
      ret.emplace_back(func(k, v));
    }
    return ret;
  }

  template<typename K, typename V>
  void foreach(std::map<K, V>& map, std::function<void(const K&,V&)> func) {
    for(auto& [k, v] : map) {
      func(k, v);
    }
  }

}
