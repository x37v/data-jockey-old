#include "scriptcallbackfilter.hpp"

ScriptCallbackFilter::ScriptCallbackFilter(std::string name, std::string description, 
		callback_t callback, void * user_data, void * other_data) :
	RemoteWorkFilterModel(name, description)
{
	mCallback = callback;
	mUserData = user_data;
	mOtherData = other_data;
}

void * ScriptCallbackFilter::getUserData(){
	return mUserData;
}

void * ScriptCallbackFilter::getOtherData(){
	return mOtherData;
}

void ScriptCallbackFilter::reportWorks(){
	mCallback(mUserData, mOtherData);
	RemoteWorkFilterModel::reportWorks();
}

