%rename(RemoteWorkFilterModel) WorkFilter(std::string, std::string);

//%include std_vector.i

class RemoteWorkFilterModel{
   public:
      RemoteWorkFilterModel(std::string name, std::string description = "Description");
      void setWorks(std::vector<int> idlist);
      void clearWorks();
      void addWork(int id);
      void reportWorks();
};
