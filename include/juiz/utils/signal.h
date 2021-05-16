#pragma once



namespace juiz {

  enum Signal {SIGNAL_INT = 1,
  };

  
  bool wait_for(const Signal signal=SIGNAL_INT);
};
