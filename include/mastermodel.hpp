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

#ifndef MASTER_MODEL_HPP
#define MASTER_MODEL_HPP

#include <QObject>

class MasterModel : public QObject {
	Q_OBJECT
	public:
		MasterModel(unsigned int numMixers = 0, QObject *parent = NULL);
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(MasterModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);
		//zero means sync to clock, 1 means sync to mixer 0..
		unsigned int syncSource() const ;
		float tempo();
		float volume();
	signals:
		void volumeChanged(float vol);
		void tempoChanged(float tempo);
		void tempoMulChanged(double mul);
		void syncSourceChanged(unsigned int src);
	public slots:
		void setVolume(float vol);
		void setTempo(float tempo);
		void setTempoMul(double mul);
		//0 is master, 1 is mixer 0, 2 is mixer 1..
		void setSyncSource(unsigned int src);
	protected slots:
		void updateVolume(float vol);
		void updateTempo(float tempo);
		void updateTempoMul(double mul);
	private:
		bool mRecursing;
		float mVolume;
		float mTempo;
		double mTempoMul;
		unsigned int mSyncSource;
		unsigned int mNumMixers;
};

#endif

