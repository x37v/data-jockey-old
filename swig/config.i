#ifndef SWIG_CONFIG_HPP
#define SWIG_CONFIG_HPP

%{
   using namespace datajockey;
   %}

class Configuration {
   public:
      static std::string getFile();
   protected:
      Configuration();
      Configuration(const Configuration&);
      Configuration& operator=(const Configuration&);
      virtual ~Configuration();
};
#endif
