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

#ifndef MIXER_CHANNEL_MODEL_HPP
#define MIXER_CHANNEL_MODEL_HPP

#include <QObject>

class EQModel;

class MixerChannelModel : public QObject {
	Q_OBJECT
	public:
		MixerChannelModel(QObject *parent = NULL);
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(MixerChannelModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);

		float volume() const;
		bool muted() const;
		EQModel * eq() const;
	signals:
		void volumeChanged(float volume, QObject *me);
		void mutedChanged(bool muted, QObject *me);
	public slots:
		void setVolume(float volume);
		void setMuted(bool muted = true);
	protected slots:
		void updateVolume(float volume);
	private:
		float mVolume;
		bool mMuted;
		EQModel * mEQ;
};

#endif
