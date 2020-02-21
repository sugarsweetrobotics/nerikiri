#pragma once

#include <memory>

namespace nerikiri {

  class Runnable {

  public:
    virtual ~Runnable() {}
  public:
    virtual bool run() = 0;

    virtual bool shutdown() = 0;

  };

  using Runnable_ptr = std::unique_ptr<Runnable>;

}
