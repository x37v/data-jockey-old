#include "object.hpp"

using namespace DataJockey;

unsigned int Object::cIdCnt = 0;

Object::Object() : mReferences(0){
	mId = cIdCnt++;
}

#include <iostream>
using std::cout;
using std::endl;


Object::~Object(){
}

void DataJockey::intrusive_ptr_add_ref(Object * p){
	(p->mReferences)++;
//	cout << "inc ref: "<< p->getId() << " : "  << p->mReferences << endl;
}
void DataJockey::intrusive_ptr_release(Object * p){
	if (--(p->mReferences) == 0)
		delete p;
//	cout << "dec ref: "<< p->getId() << " : "  << p->mReferences << endl;
}
