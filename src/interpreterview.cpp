#include "interpreterview.hpp"
#include "interpretermodel.hpp"
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QKeyEvent>

#define TAB_STOP "\t"

InterpreterView::InterpreterView(InterpreterModel * model, QWidget * parent) :
	QWidget(parent)
{
	mModel = model;
	mHistoryIndex = 0;
	mWaitingForOutput = false;
	mTextEntry = new QLineEdit(this);
	mOutputDisplay = new QTextEdit(this);
	QVBoxLayout * layout = new QVBoxLayout(this);

	mOutputDisplay->setReadOnly(true);

	layout->addWidget(mOutputDisplay, 10);
	layout->addWidget(mTextEntry, 0);

	setLayout(layout);

	//set up the cancel action
	mTextEntry->installEventFilter(this);

	//connect internal sigs and slots
	QObject::connect(
			mTextEntry, SIGNAL(returnPressed()),
			this, SLOT(acceptNewInput()));
}

void InterpreterView::keyPressEvent ( QKeyEvent * event ){
	/*
	//if(event->modifiers() && Qt::ControlModifier){
	if(event->key() == Qt::Key_C){
		cout << "c" << endl;
		if(event->modifiers() && Qt::ControlModifier){
			cout << "control" << endl;
		}
		*/
	if(event->matches(QKeySequence::MoveToPreviousLine)){
		if (mHistoryIndex == 0){
			mCurrentInput = mTextEntry->text();
			mHistoryIndex = 1;
			mTextEntry->setText(mModel->line(0));
		} else if (mHistoryIndex < mModel->inputHistoryLines()) {
			mTextEntry->setText(mModel->line(mHistoryIndex));
			mHistoryIndex += 1;
			if (mHistoryIndex > mModel->inputHistoryLines())
				mHistoryIndex = mModel->inputHistoryLines();
		}
	} else if(event->matches(QKeySequence::MoveToNextLine)){
		if (mHistoryIndex == 0){
			//do nothing
		} else if (mHistoryIndex == 1){
			mTextEntry->setText(mCurrentInput);
			mHistoryIndex = 0;
		} else {
			mHistoryIndex -= 1;
			mTextEntry->setText(mModel->line(mHistoryIndex - 1));
		}
	} else {
		QWidget::keyPressEvent(event);
	}
}

bool InterpreterView::eventFilter(QObject *obj, QEvent *ev){
	if(obj == mTextEntry){
		if (ev->type() == QEvent::KeyPress) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(ev);
			if(keyEvent->key() == Qt::Key_C && (keyEvent->modifiers() & Qt::ControlModifier)){
				cancelInput();
				return true;
			} else if(keyEvent->key() == Qt::Key_Tab){
				mTextEntry->insert(TAB_STOP);
				return true;
			}
		}
	}
	return QWidget::eventFilter(obj, ev);
}

void InterpreterView::setInput(QString line){
	mTextEntry->setText(line);
}

void InterpreterView::addToOutput(QString line){
	mWaitingForOutput = false;
	mOutputDisplay->moveCursor(QTextCursor::End);
	mOutputDisplay->insertPlainText(line);
	mOutputDisplay->moveCursor(QTextCursor::End);
}

void InterpreterView::acceptNewInput(){
	QString newText = mTextEntry->text();
	mHistoryIndex = 0;
	mTextEntry->clear();

	//move the cursor to the end because the user may have moved it
	mOutputDisplay->moveCursor(QTextCursor::End);
	if(mWaitingForOutput)
		mOutputDisplay->insertHtml(QString("<strong>?>&nbsp;"));
	else 
		mOutputDisplay->insertHtml(QString("<strong>>>&nbsp;"));
	mOutputDisplay->insertPlainText(newText);
	mOutputDisplay->insertHtml(QString("</strong><br>\n"));

	mWaitingForOutput = true;
	emit(newInput(newText));
	mOutputDisplay->moveCursor(QTextCursor::End);
}

void InterpreterView::cancelInput(){
	mOutputDisplay->moveCursor(QTextCursor::End);
	if(mWaitingForOutput){
		QString html = QString("<strong>?> ^C</strong><br>\n");
		mOutputDisplay->insertHtml(html);
	} else {
		QString html = QString("<strong>>> ^C</strong><br>\n");
		mOutputDisplay->insertHtml(html);
	}
	mWaitingForOutput = false;
	mTextEntry->clear();
	emit(cancelingInput());
}

