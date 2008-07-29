%{
#include "../include/remoteworkfiltermodel.hpp"
   %}
%rename(WorkFilter) RemoteWorkFilterModel;

%include std_vector.i

class RemoteWorkFilterModel{
   public:
      RemoteWorkFilterModel(std::string name, std::string description);
      RemoteWorkFilterModel(std::string name);
      void setWorks(std::vector<int> idlist);
      void clearWorks();
      void addWork(int id);
      virtual void reportWorks();
};
