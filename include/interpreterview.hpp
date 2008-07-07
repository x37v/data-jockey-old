#ifndef INTERPRETER_VIEW_HPP
#define INTERPRETER_VIEW_HPP

#include <QWidget>

class QLineEdit;
class QTextEdit;
class QKeyEvent;
class InterpreterModel;

class InterpreterView : public QWidget {
	Q_OBJECT
	public:
		InterpreterView(InterpreterModel * model, QWidget * parent = NULL);
		virtual void keyPressEvent ( QKeyEvent * event );
	protected:
		virtual bool eventFilter(QObject *obj, QEvent *ev);
	signals:
		void newInput(QString line);
		void cancelingInput();
	public slots:
		void setInput(QString line);
		void addToOutput(QString line);
	protected slots:
		void acceptNewInput();
		void cancelInput();
	private:
		//mWaitingForOutput is true when we've sent at least one line
		//before getting any input back.. it helps us set the prompt
		//on the input display
		bool mWaitingForOutput;
		QLineEdit * mTextEntry;
		QTextEdit * mOutputDisplay;
		//zero means current, use mHistoryIndex - 1 to get history line
		unsigned int mHistoryIndex;
		QString mCurrentInput;
		InterpreterModel * mModel;
};

#endif
