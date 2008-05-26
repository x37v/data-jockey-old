#include "djmixerchannelview.hpp"
#include "djmixercontrolview.cpp"
#include "mixerchannelview.cpp"

DJMixerChannelView::DJMixerChannelView(QWidget * parent)
	: QWidget(parent) 
{
	mDJMixerControl = new DJMixerControlView(this);
	mMixerChannel = new MixerChannelView(this);
}

DJMixerControlView * DJMixerChannelView::DJMixerControl(){
	return mDJMixerControl;
}

MixerChannelView * DJMixerChannelView::mixerChannel(){
	return mMixerChannel;
}

