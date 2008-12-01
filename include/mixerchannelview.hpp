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

#ifndef MIXER_CHANNEL_VIEW_HPP
#define MIXER_CHANNEL_VIEW_HPP

#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>
class EQView;

class MuteButton : public QPushButton {
	Q_OBJECT
	public:
		MuteButton(QWidget * parent) : QPushButton(parent){
			setToolTip("mute (toggle)");
			setCheckable(true);
		}
};

class MixerChannelView : public QObject {
	Q_OBJECT
	public:
		MixerChannelView(QWidget *parent = NULL);
		EQView * eq();
		QSlider * volumeSlider();
		MuteButton * muteButton();
		float volume() const;
		bool muted() const;
	signals:
		void volumeChanged(float volume);
		void mutedChanged(bool muted);
	public slots:
		void setVolume(float volume);
		void setVolume(int volume);
		void setMuted(bool muted = true);
	private:
		EQView * mEQView;
		QSlider * mVolumeSlider;
		MuteButton * mMuteBtn;
		//to prevent infinite loops
		bool mRecursing;
};

#endif

