#include "interpreterview.hpp"
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QKeyEvent>

InterpreterView::InterpreterView(QWidget * parent) :
	QWidget(parent)
{
	mWaitingForOutput = false;
	mTextEntry = new QLineEdit(this);
	mOutputDisplay = new QTextEdit(this);
	QVBoxLayout * layout = new QVBoxLayout(this);

	mOutputDisplay->setReadOnly(true);

	layout->addWidget(mOutputDisplay, 10);
	layout->addWidget(mTextEntry, 0);

	setLayout(layout);

	//connect internal sigs and slots
	QObject::connect(
			mTextEntry, SIGNAL(returnPressed()),
			this, SLOT(acceptNewInput()));
}

/*
void InterpreterView::keyPressEvent ( QKeyEvent * event ){
	if(event->matches(QKeySequence::Copy)){
		emit(newInput(""));
	} else {
		QWidget::keyPressEvent(event);
	}
}
*/

void InterpreterView::setInput(QString line){
	mTextEntry->setText(line);
}

void InterpreterView::addToOutput(QString line){
	mWaitingForOutput = false;
	mOutputDisplay->insertPlainText(line);
	mOutputDisplay->moveCursor(QTextCursor::End);
}

void InterpreterView::acceptNewInput(){
	QString newText = mTextEntry->text();
	mTextEntry->clear();

	if(mWaitingForOutput){
		QString html = QString("<strong>?> %1</strong><br>\n").arg(newText);
		mOutputDisplay->insertHtml(html);
	} else {
		QString html = QString("<strong>>> %1</strong><br>\n").arg(newText);
		mOutputDisplay->insertHtml(html);
	}
	mWaitingForOutput = true;
	emit(newInput(newText));
	mOutputDisplay->moveCursor(QTextCursor::End);
}

