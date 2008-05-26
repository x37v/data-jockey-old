#include "djmixerchannelview.hpp"
#include "masterview.hpp"
#include <QApplication>
#include <QWidget>
#include <QCleanlooksStyle>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	 app.setStyle(new QCleanlooksStyle);
	 QWidget * win = new QWidget;
	 win->setWindowTitle("floatme: datajockey");
    DJMixerChannelView * mixerChans = new DJMixerChannelView[4];
	 QHBoxLayout * layout = new QHBoxLayout(win);
	 MasterView * master = new MasterView(win);

	 layout->setSpacing(2);

	 for(unsigned int i = 0; i < 4; i++){
		 mixerChans[i].setParent(win);
		 layout->addWidget(&mixerChans[i]);
	 }
	 layout->addWidget(master);
	 win->setLayout(layout);
	 win->show();

    return app.exec();
}
