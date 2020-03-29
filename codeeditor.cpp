#include <QtWidgets>
#include <QFont>
#include "codeeditor.h"
#include <QFontMetrics>
#include "gatortext.h"

//new
CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    //creates the updating line number area
    lineNumberArea = new LineNumberArea(this);


    //connects the changing line numbers to the line number editor
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    //sets the editor to update the window title whenever the editor is modified. Useful in that it shows if something is saved or unsavesd
    connect(this->document(), &QTextDocument::contentsChanged, this, &CodeEditor::updateWindowTitleDocumentChanged);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    //sets editor default font and size. The other stuff is to make it look like an IDE
    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(11);

    //updates the default tab stop of the editor. Sets the width to 4. Otherwise tabs only equal one space by default
    QFontMetrics metrics(font);
    this->setTabStopWidth(tabStop * metrics.width(" "));
    this->setFont(font);

    //attempts to create the completer. If its unsuccessful display an error
    if (!createCompleter()) {
        QMessageBox errorBox;
        errorBox.setText("Error. Unable to open wordlist. AutoComplete Disabled");
        errorBox.exec();
    }

    //allows the editor to accept drop events for drag and drop
    setAcceptDrops(true);
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}



void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> selections = extraSelections();

    if (!isReadOnly()) {
        for (int index = 0; index < selections.size(); index++) {
            if (selections[index].format.background().color().name() == QColor(Qt::blue).lighter(160).name()) {
                selections.removeAt(index);
            }
        }

        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::blue).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        selections.append(selection);
    }

    setExtraSelections(selections);
}



void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

bool CodeEditor::createCompleter() {
    QFile file(":/cpp.txt");
    file.open(QFile::ReadOnly);

    if (file.isOpen()) {
        while(!file.atEnd()) {
            QString line = file.readLine();
            masterList.append(line);

        }

        completer = new QCompleter(masterList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setWidget(this);

        //connects the activated completer signal to the insert completion slot
        QObject::connect(completer, SIGNAL(activated(QString)),
                             this, SLOT(insertCompletion(QString)));

        file.close();

        return true;
    } else {
        return false;
    }
}

//where the majority of the work is done
void CodeEditor::keyPressEvent(QKeyEvent *event) {
    //check the currently entered text and apply it  to the completer
    if (completer) {
        QTextCursor cursor = textCursor();
        cursor.select(QTextCursor::WordUnderCursor);

        QString temp = cursor.selectedText();
        if (temp.length() >= 2) {
            completer->setCompletionPrefix(temp);

            //the rect displays the contents of the completion window
            //TODO. dress up the window a bit. maybe add some more information
            QRect rect = cursorRect();
            rect.setWidth(completer->popup()->sizeHintForColumn(0)
                          + completer->popup()->verticalScrollBar()->sizeHint().width());

            completer->complete(rect);
        }
    }

    //inserts the next paren
    if (event->key() == Qt::Key_ParenLeft) {
        QTextCursor tc = textCursor();
        int oldPos = tc.position();
        tc.insertText(")");
        tc.setPosition(oldPos);
        setTextCursor(tc);
        parenAdded = true;
    }

    //skips the current paren if one is already added
    if (event->key() == Qt::Key_ParenRight) {
        if (parenAdded == true) {
            QTextCursor tc = textCursor();
            int pos = tc.position();
            tc.setPosition(pos + 1);
            setTextCursor(tc);
            parenAdded = false;
            event->ignore();
            return;
        }
    }

     //inserts the matching quote and skips the next one when the appropriate character is pressed
    if (event->key() == Qt::Key_QuoteDbl) {
        QTextCursor tc = textCursor();
        int pos = tc.position();

        if (dblQuoteAdded == false) {
            tc.insertText("\"");
            tc.setPosition(pos);
            setTextCursor(tc);
            dblQuoteAdded = true;
        } else {
            tc.setPosition(pos + 1);
            setTextCursor(tc);
            dblQuoteAdded = false;
            event->ignore();
            return;
        }
    }

    //removes the completer when space is added
    if (event->key() == Qt::Key_Space) {
        //remove completer if space is pressed
        completer->popup()->hide();

        QTextCursor tc = textCursor();
        tc.movePosition(QTextCursor::WordLeft);
        tc.select(QTextCursor::WordUnderCursor);
        QString temp = tc.selectedText();


        //dont add the word to the list if its in master or additional list
        if (!checkForWordInList(temp)) {
            masterList.insert(0, temp);
            additionalItems.insert(0, temp);
            updateCompleter();
        }
    }


    //when the user hits enter used for adding indentation
    if (event->key() == Qt::Key_Return) {
        QTextCursor tc = textCursor();
        QString line = tc.block().text();
        QTextBlock nextBlock = document()->findBlockByNumber(tc.blockNumber() + 1);

        qDebug()<< nextBlock.text();
        int count = line.count("\t");
        //if  indentation is added.
        if (count > 0) {
            tc.insertText("\n");

            if (nextBlock.text().contains("}")) {
                tc.insertText("\t");
            }

            for (int i = 0; i < count; i++) {
                tc.insertText("\t");
            }
            return;
            //if no indentation is added check for the end brace
        } else if (nextBlock.text().contains("}")) {
            tc.insertText("\n");
            tc.insertText("\t");
            return;
        }
    }

    if (event->key() == Qt::Key_BraceLeft) {
        QTextCursor tc = textCursor();
        int originalPosition = tc.position();
        int tabCount = tc.block().text().count("\t");

        QString prependString = "\n";

        if (tabCount > 0) {
            for (int i =0; i <tabCount; i++) {
                prependString += "\t";
            }
        }
        prependString += "}";
        tc.insertText(prependString);
        tc.setPosition(originalPosition);
        setTextCursor(tc);
    }

    //passes the key to the original event handler to do normal processing
    QPlainTextEdit::keyPressEvent(event);
}


//sets the completer to the widget
void CodeEditor::focusInEvent(QFocusEvent *event) {
    if (completer) {
        completer->setWidget(this);
    }
    QPlainTextEdit::focusInEvent(event);
}

//when the completer is activated insert the word
void CodeEditor::insertCompletion(const QString& completion) {
    QString temp = completer->completionPrefix();
    QTextCursor tc = textCursor();

    int oldPosition = tc.position();
    tc.select(QTextCursor::WordUnderCursor);
    tc.insertText(completion);

    tc.setPosition(oldPosition);
    tc.movePosition(QTextCursor::WordRight);
    setTextCursor(tc);
}

bool CodeEditor::checkForWordInList(QString word) {
    if (masterList.contains(word)) {
        return true;
    } else {
        return false;
    }
}

//updates the completer
void CodeEditor::updateCompleter() {
    completer = new QCompleter(masterList, this);
    //i have to reset this everytime. I thought the point of passing in the old one as param was to update the new one?
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setWidget(this);

    //gotta reconnect it too
    QObject::connect(completer, SIGNAL(activated(QString)),
                         this, SLOT(insertCompletion(QString)));
}


bool CodeEditor::updateCompleter(LanguageType language) {
    QString filename;
    QString finalName;

    switch (language) {
    case CPP:
        filename = "cpp";
        break;
    case C:
        filename = "c";
        break;
    case JAVASCRIPT:
        filename = "javascript";
        break;
    case PYTHON:
        filename = "python";
        break;
    case JAVA:
        filename = "java";
        break;
    }

    //update the masterlist to be empty for the new file
    masterList.clear();

    finalName = ":/" + filename + ".txt";
    QFile file(finalName);
    file.open(QFile::ReadOnly);

    if (file.isOpen()) {
        while(!file.atEnd()) {
            QString line = file.readLine();
            masterList.append(line);

        }

        for (QStringList::const_iterator it = additionalItems.constBegin(); it != additionalItems.constEnd(); it++) {
            masterList.push_back(*it);
        }

        completer = new QCompleter(masterList, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setWidget(this);

        QObject::connect(completer, SIGNAL(activated(QString)),
                             this, SLOT(insertCompletion(QString)));

        file.close();

        return true;
    } else {
        return false;
    }
}

QStringList* CodeEditor::getMasterList() {
    return &masterList;
}

QStringList* CodeEditor::getAdditionalItems() {
    return &additionalItems;
}

void CodeEditor::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void CodeEditor::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasUrls()) {
        QStringList  pathList;

        if (event->mimeData()->urls().size() > 1) {
            QMessageBox errorBox;
            errorBox.setText("Sorry. You are only able to drag and drop one file at a time");
            errorBox.exec();
        } else {
            QUrl url = event->mimeData()->urls()[0];
            QString filename = url.toLocalFile();

            GatorText* parent = (GatorText*) this->parent();
            if (parent->checkForSave()) {
                parent->loadFile(filename);
            }
        }
    }
}

void CodeEditor::updateWindowTitleDocumentChanged() {
    GatorText* gatorText = (GatorText*) this->parent();
    QString windowTitle = gatorText->windowTitle();

    if (!windowTitle.contains("*")) {
        windowTitle.append("*");
        gatorText->setWindowTitle(windowTitle);
    }
}
