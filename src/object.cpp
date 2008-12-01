/*
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

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
