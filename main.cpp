#include "djmixerchannelview.hpp"
#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	 QWidget * win = new QWidget;
    MixerChannelView * mixerChans = new MixerChannelView[4];
	 QHBoxLayout * layout = new QHBoxLayout(win);

	 layout->setSpacing(5);

	 for(unsigned int i = 0; i < 4; i++){
		 mixerChans[i].setParent(win);
		 layout->addWidget(&mixerChans[i]);
	 }
	 win->setLayout(layout);
	 win->show();

    return app.exec();
}
