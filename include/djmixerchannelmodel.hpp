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

#ifndef DJ_MIXER_CHANNEL_MODEL_HPP
#define DJ_MIXER_CHANNEL_MODEL_HPP

#include <QObject>
#include "mixerchannelmodel.hpp"

class DJMixerControlModel;

class DJMixerChannelModel : public MixerChannelModel {
	Q_OBJECT
	public:
		DJMixerChannelModel(QObject * parent = NULL);
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(DJMixerChannelModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);

		DJMixerControlModel * control() const;
		int work() const;
	signals:
		void workChanged(int work);
		void loadingWork(int work, QObject *me);
	public slots:
		void setWork(int work);
		//loadWork doesn't set our work.. though, eventually, once the work is
		//loaded set work will be called
		void loadWork(int work);
		void reset();
	private:
		int mWork;
		DJMixerControlModel * mDJMixerControl;
};

#endif
