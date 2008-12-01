/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
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
