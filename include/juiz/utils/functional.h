#pragma once


#include <vector>
#include <map>
#include <functional>
#include <numeric>

namespace juiz {


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
/*
  template<typename T, typename K, typename V>
  std::vector<T> map(std::map<K, V>& map, std::function<T(const K&, V&)> func) {
    std::vector<T> ret;
    for(auto& [k, v] : map) {
      ret.emplace_back(func(k, v));
    }
    return ret;
  }
*/
  template<typename T, typename K, typename V>
  std::vector<T> map(const std::map<K, V>& map, std::function<T(const K&, const V&)> func) {
    std::vector<T> ret;
    for(const auto& [k, v] : map) {
      ret.emplace_back(func(k, v));
    }
    return ret;
  }

  template<typename T, typename K, typename V>
  std::vector<T> map(std::multimap<K, V>& map, std::function<T(const K&,V&)> func) {
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

  template<typename K, typename V>
  void foreach(const std::map<K, V>& map, std::function<void(const K&, const V&)> func) {
    for(const auto& [k, v] : map) {
      func(k, v);
    }
  }

  template<typename K, typename V>
  void foreach(std::multimap<K, V>& map, std::function<void(const K&,V&)> func) {
    for(auto& [k, v] : map) {
      func(k, v);
    }
  }


  template<typename V>
  void foreach(std::vector<V>& vec, std::function<void(V&)> func) {
    for(auto& v : vec) {
      func(v);
    }
  }

  template<typename V>
  void foreach(std::vector<V>&& vec, std::function<void(V&)> func) {
    for(auto& v : vec) {
      func(v);
    }
  }


  template<typename V>
  std::vector<V> filter(std::vector<V>& vec, std::function<bool(V&)> func) {
    std::vector<V> ret;
    for(auto& v : vec) {
      if (func(v)) {
        ret.push_back(v);
      }
    }
    return ret;
  }

  template<typename T, typename V>
  std::vector<T> map(std::vector<V>& vec, std::function<T(V&)> func) {
    std::vector<T> ret;
    for(auto& v : vec) {
      ret.emplace_back(func(v));
    }
    return ret;
  }

  template<typename T, typename V>
  T find_first(const std::vector<V>& vec, std::function<bool(const V&)> filter, std::function<T(const V&)> output, const T& defaultValue) {
    for(auto& v : vec) {
      if (filter(v)) {
        return output(v);
      }
    }
    return defaultValue;
  }

  template<typename T, typename V>
  std::vector<T> map(const std::vector<V>& vec, std::function<T(const V&)> func) {
    std::vector<T> ret;
    for(const auto& v : vec) {
      ret.emplace_back(func(v));
    }
    return ret;
  }



   template<typename V, typename T>
  T reduce(std::vector<V>& vec, std::function<void(T&, V&)> func, T acc) {
    /*
    foreach<V>(vec, [&acc, func](V& v) { func(acc, v); });*/
    return acc;
  }


  namespace functional {

    template<typename V>
    std::vector<V> join(const std::vector<V>& v0, const std::vector<V>& v1) {
      std::vector<V> v{v0.begin(), v0.end()};
      v.insert(v.end(), v1.begin(), v1.end());
      return v;
    }

    template<typename V>
    bool for_all(const std::vector<V>& vec, std::function<bool(const V&)> func) {
      bool flag = true;
      for(const auto& v : vec) {
        flag = flag && func(v);
      }
      return flag;
    }

    template<typename V>
    bool for_any(const std::vector<V>& vec, std::function<bool(const V&)> func) {
      bool flag = false;
      for(const auto& v : vec) {
        flag = flag || func(v);
      }
      return flag;
    }

    template<typename V>
    void for_each(const std::vector<V>& vec, std::function<void(const V&)> func) {
      for(const auto& v : vec) {
        func(v);
      }
    }

    template<typename T, typename V>
    std::vector<T> map(const std::vector<V>& vec, std::function<T(const V&)> func) {
      std::vector<T> ret;
      for(const auto& v : vec) {
        ret.emplace_back(func(v));
      }
      return ret;
    }

    template<typename T, typename V, typename U>
    std::vector<T> map(const std::map<V, U>& vec, std::function<T(const V&, const U&)> func) {
      std::vector<T> ret;
      for(const auto& [v, u] : vec) {
        ret.emplace_back(func(v, u));
      }
      return ret;
    }


    template<typename V>
    std::optional<V> find(const std::vector<V>& vec, std::function<bool(const V&)> filter) {
      for(auto& v : vec) {
        if (filter(v)) {
          return v;
        }
      }
      return std::nullopt;
    }



  }
}
