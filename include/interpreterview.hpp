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
		//mWaitingForOutput is true when we've sent at least one line
		//before getting any input back.. it helps us set the prompt
		//on the input display
		bool mWaitingForOutput;
		QLineEdit * mTextEntry;
		QTextEdit * mOutputDisplay;
};

#endif
