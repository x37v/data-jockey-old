#ifndef SCRIPT_CALLBACK_FILTER_HPP
#define SCRIPT_CALLBACK_FILTER_HPP

#include "remoteworkfiltermodel.hpp"
typedef void (*callback_t) (void *user_data, void* other_data);

class ScriptCallbackFilter : public RemoteWorkFilterModel {
	private:
		callback_t mCallback;
		void * mUserData;
		void * mOtherData;
	public:
		ScriptCallbackFilter(std::string name, std::string description, callback_t callback, void * user_data = NULL, void * other_data = NULL);
		void * getUserData();
		void * getOtherData();
	public slots:
		virtual void reportWorks();
};

#endif

