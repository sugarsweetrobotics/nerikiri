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

}
