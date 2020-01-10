#pragma once


namespace nerikiri {

  class State {

  public:
    State();
    ~State();

  public:
    int64_t intValue() { return 0; }

    State& operator=(const int64_t ivalue);
  };


}
