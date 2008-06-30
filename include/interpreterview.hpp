#ifndef INTERPRETER_VIEW_HPP
#define INTERPRETER_VIEW_HPP

#include <QWidget>

class QLineEdit;
class QTextEdit;
class QKeyEvent;

class InterpreterView : public QWidget {
	Q_OBJECT
	public:
		InterpreterView(QWidget * parent = NULL);
		//virtual void keyPressEvent ( QKeyEvent * event );
	signals:
		void newInput(QString line);
	public slots:
		void setInput(QString line);
		void addToOutput(QString line);
	protected slots:
		void acceptNewInput();
	private:
		QLineEdit * mTextEntry;
		QTextEdit * mOutputDisplay;
};

#endif
