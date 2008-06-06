#ifndef DJ_MIXER_WORK_INFO_VIEW_HPP
#define DJ_MIXER_WORK_INFO_VIEW_HPP

#include <QWidget>
#include <QString>

class QLineEdit;
class QVBoxLayout;

class DJMixerWorkInfoView : public QWidget {
	Q_OBJECT
	public:
		DJMixerWorkInfoView(QWidget * parent);
	public slots:
		void setArtistText(QString text);
		void setTitleText(QString text);
		void reset();
	private:
		QVBoxLayout * mLayout;
		QLineEdit * mArtistLabel;
		QLineEdit * mWorkLabel;
};

#endif
