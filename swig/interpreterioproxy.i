#ifndef SWIG_INTEPRETER_IO_PROXY_HPP
#define SWIG_INTEPRETER_IO_PROXY_HPP

class InterpreterIOProxy {
   public:
      InterpreterIOProxy();
      void addToOutput(std::string output);
      static std::string pipeLocation();
      static void processEvents();
};

#endif

