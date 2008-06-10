#ifndef DATAJOCKEY_OBJECT_HPP
#define DATAJOCKEY_OBJECT_HPP

#include <boost/intrusive_ptr.hpp>

namespace DataJockey {

	class Object;

	//for intrusive_ptr
	void intrusive_ptr_add_ref(Object * p);
	void intrusive_ptr_release(Object * p);

	class Object {
		public:
			Object();
			virtual ~Object();
			unsigned int getId(){return mId;}
		private:
			friend void intrusive_ptr_add_ref(Object * p);
			friend void intrusive_ptr_release(Object * p);
			unsigned int mId;
			static unsigned int cIdCnt;
		protected:
			long mReferences;
	};
}

#endif
