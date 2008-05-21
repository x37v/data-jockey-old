#include "djmixerchannelview.hpp"
#include "mixerchannelview.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>

DJMixerChannelView::DJMixerChannelView(QWidget *parent)
	: QWidget(parent)
{
	mLayout = new QVBoxLayout(this);
	mButtonLayout = new QHBoxLayout(this);
	mMixerChannel = new MixerChannelView(this);

	mLayout->addWidget(mMixerChannel);
	setLayout(mLayout);
}
