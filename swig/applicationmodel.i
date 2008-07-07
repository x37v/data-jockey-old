class MixerPanelModel;
class RemoteWorkFilterModel;

%rename(ApplicationModel) ApplicationModelProxy;

/* Tell SWIG to keep track of mappings between C/C++ structs/classes. */
%trackobjects;

%markfunc ApplicationModelProxy "mark_ApplicationModelProxy";

class ApplicationModelProxy {
   public:
      ApplicationModelProxy(Qt::ConnectionType type = Qt::QueuedConnection, QObject * parent = ApplicationModel::instance());
      MixerPanelModel * mixerPanel() const;
      void addFilter(RemoteWorkFilterModel * filter);
      void removeFilter(RemoteWorkFilterModel * filter);
      void removeFilter(unsigned int i);
      unsigned int numFilters();
      RemoteWorkFilterModel * filter(unsigned int i);
};

%header %{

   static void mark_ApplicationModelProxy(void* ptr) {
      ApplicationModelProxy* proxy = (ApplicationModelProxy*) ptr;

      /* Loop over each object and tell the garbage collector
         that we are holding a reference to them. */
      unsigned int count = proxy->numFilters();

      for(unsigned int i = 0; i < count; ++i) {
         RemoteWorkFilterModel* filter = proxy->filter(i);
         if(filter){
            VALUE object = SWIG_RubyInstanceFor(filter);

            if (object != Qnil) {
               rb_gc_mark(object);
            }
         }
      }
   }
%}
