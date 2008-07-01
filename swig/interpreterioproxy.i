#ifndef SWIG_INTEPRETER_IO_PROXY_HPP
#define SWIG_INTEPRETER_IO_PROXY_HPP

class InterpreterIOProxy {
   public:
      InterpreterIOProxy();
      void addToOutput(std::string output);
      bool newInput();
      std::string getInput();
      static void processEvents();
};

#endif

