#include "mixerpanelview.hpp"
#include <QApplication>
#include <QWidget>
#include <QCleanlooksStyle>

#include "crossfadeview.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	 app.setStyle(new QCleanlooksStyle);

	 MixerPanelView * mixerPannel = new MixerPanelView;
	 mixerPannel->setWindowTitle("floatme: datajockey");
	 mixerPannel->show();
    return app.exec();
}

