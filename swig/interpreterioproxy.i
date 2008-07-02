#ifndef SWIG_INTEPRETER_IO_PROXY_HPP
#define SWIG_INTEPRETER_IO_PROXY_HPP

class InterpreterIOProxy {
   public:
      static void addToOutput(std::string output);
      static bool newInput();
      static std::string getInput();
      static void processEvents();
   protected:
      InterpreterIOProxy();
      InterpreterIOProxy(const InterpreterIOProxy&);
      //InterpreterIOProxy& operator= (const InterpreterIOProxy&);
};

#endif

