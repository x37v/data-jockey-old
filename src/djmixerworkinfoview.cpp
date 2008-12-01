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

#include "djmixerworkinfoview.hpp"

#include <QLineEdit>
#include <QVBoxLayout>

DJMixerWorkInfoView::DJMixerWorkInfoView(QWidget * parent) :
	QWidget(parent)
{
	mLayout = new QVBoxLayout(this);
	mArtistLabel = new QLineEdit(tr("artist"), this);
	mWorkLabel = new QLineEdit(tr("title"), this);
	mArtistLabel->setReadOnly(true);
	mWorkLabel->setReadOnly(true);
	mArtistLabel->setToolTip(tr("artist"));
	mWorkLabel->setToolTip(tr("title"));
	mLayout->addWidget(mArtistLabel, 0);
	mLayout->addWidget(mWorkLabel, 0);
	mLayout->setContentsMargins(0,0,0,0);
	mLayout->setSpacing(1);
	setLayout(mLayout);
}

void DJMixerWorkInfoView::setArtistText(QString text){
	mArtistLabel->setText(text);
	mArtistLabel->setCursorPosition(0);
}

void DJMixerWorkInfoView::setTitleText(QString text){
	mWorkLabel->setText(text);
	mWorkLabel->setCursorPosition(0);
}

void DJMixerWorkInfoView::reset(){
	setArtistText("artist");
	setTitleText("title");
}

