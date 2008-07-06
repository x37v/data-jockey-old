#include "scriptcallbackfilter.hpp"

ScriptCallbackFilter::ScriptCallbackFilter(callback_t callback, void * user_data, void * other_data){
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
}

