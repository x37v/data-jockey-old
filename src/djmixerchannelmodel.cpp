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

#include "djmixerchannelmodel.hpp"
#include "djmixercontrolmodel.hpp"
#include "mixerchannelmodel.cpp"

DJMixerChannelModel::DJMixerChannelModel(QObject * parent):
	MixerChannelModel(parent) 
{
	mDJMixerControl = new DJMixerControlModel(this);
	mWork = -1;
}

void DJMixerChannelModel::syncToModel(DJMixerChannelModel * other, Qt::ConnectionType connectionType){
	//this -> other (state changes)
	//XXX this could be problematic as it doens't actually load the work.. but
	//just be careful
	QObject::connect(this,
			SIGNAL(workChanged(int)),
			other,
			SLOT(setWork(int)),
			connectionType);
	//other -> this (state changes)
	QObject::connect(other,
			SIGNAL(workChanged(int)),
			this,
			SLOT(setWork(int)),
			connectionType);
	//this -> other (non [direct] state changes)
	QObject::connect(this,
			SIGNAL(loadingWork(int, QObject *)),
			other,
			SLOT(loadWork(int)),
			connectionType);

	//connect our children too
	mDJMixerControl->syncToModel(other->control(), connectionType);
	MixerChannelModel::syncToModel(other, connectionType);
}

DJMixerControlModel * DJMixerChannelModel::control() const {
	return mDJMixerControl;
}

int DJMixerChannelModel::work() const {
	return mWork;
}

void DJMixerChannelModel::setWork(int work){
	if(mWork != work){
		mWork = work;
		emit(workChanged(mWork));
	}
}

void DJMixerChannelModel::loadWork(int work){
	if(mWork != work){
		emit(loadingWork(work, this));
	}
}

void DJMixerChannelModel::reset(){
	mDJMixerControl->reset();
}

