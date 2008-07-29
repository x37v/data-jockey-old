%{
#include "scriptcallbackfilter.hpp"
%}

%{
   void wrap_callback(void *user_data, void *other_data){
      VALUE proc = (VALUE)user_data;
      if(other_data != NULL){
         VALUE proc_arg = (VALUE)other_data;
         rb_funcall(proc, rb_intern("call"), 1, proc_arg);
      } else
         rb_funcall(proc, rb_intern("call"), 0);
   }
%}

%typemap(in) (callback_t callback, void *user_data)
{
 $1 = wrap_callback;
 $2 = (void *)$input;
}
%typemap(in) (void *other_data)
{
 $1 = (void *)$input;
}
%typemap(default) (void *other_data)
{
$1 = NULL;
}

%typecheck( SWIG_TYPECHECK_POINTER ) ( callback_t callback, void *user_data)
{
 // is callable?
 static ID id = rb_intern("call");
 $1 = (rb_respond_to( $input, id ) == Qtrue);
}

/* Keep track of mappings between C/C++ structs/classes
   and Ruby objects so we can implement a mark function. */
%trackobjects;

/* Specify the mark function */
%markfunc ScriptCallbackFilter "mark_ScriptCallbackFilter";

%rename(WorkFilter) ScriptCallbackFilter;

class ScriptCallbackFilter : public RemoteWorkFilterModel {
   public:
      ScriptCallbackFilter(std::string name, std::string description, 
            callback_t callback, void * user_data, void * other_data);
      void setWorks(std::vector<int> idlist);
      void clearWorks();
      void addWork(int id);
      virtual void reportWorks();
};

%header %{
   static void mark_ScriptCallbackFilter(void* ptr) {
      ScriptCallbackFilter* cbf = (ScriptCallbackFilter*) ptr;
         void * user_data = cbf->getUserData();
         //user_data should never be null
         if(user_data != NULL){
            VALUE object = (VALUE)(user_data);
            if (object != Qnil) {
               rb_gc_mark(object);
            }
         }
         //this might be null
         void * other_data = cbf->getOtherData();
         if(other_data != NULL) {
            VALUE object = (VALUE)(other_data);
            if (object != Qnil) {
               rb_gc_mark(object);
            }
         }
   }
%}

