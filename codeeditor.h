#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QtWidgets>
#include <QCompleter>
#include "languagetypes.h"

//classes neccessary later in class
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class LineNumberArea;

/*
 * Header file for the code editor. The code editor is a modified QPlainTextEdit with extra functionality to show line numbers
 * and a highlighting component. Also added functionality for dragging and dropping of code files. Most work happens in the keypressevent
*/
class CodeEditor: public QPlainTextEdit {
    Q_OBJECT

    public:
        CodeEditor(QWidget *parent = 0);

        //responsible for the painting of the line number area
        void lineNumberAreaPaintEvent(QPaintEvent *event);
        int lineNumberAreaWidth();

        //updates the completer language when a new language is selected
        bool updateCompleter(LanguageType language);

        //master list and additional item list for all language keywords and words entered by the user
        QStringList* getMasterList();
        QStringList* getAdditionalItems();

    protected:
        void resizeEvent(QResizeEvent *event) override;
        //most work is done in here
        void keyPressEvent(QKeyEvent* event) override;
        void focusInEvent(QFocusEvent* event) override;

        //used for when a user drags and drops a file onto the editor. Allowing them to open or edit them
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dropEvent(QDropEvent *e) override;

    public slots:
        void updateLineNumberAreaWidth(int newBlockCount);
        void highlightCurrentLine();
        void updateLineNumberArea(const QRect &, int);
        void insertCompletion(const QString &completion);

        //updates the document title when the contents of the editor are modified
        void updateWindowTitleDocumentChanged();

    private:
        QStringList masterList;
        QStringList additionalItems;
        QWidget *lineNumberArea;
        //used to update the tabstop to 4. Its 1 by default can you believe that
        const int tabStop = 4;
        //set completer to null. Null means no completer is loaded. Either the language file was corrupted or an unknown language was selected
        QCompleter* completer = nullptr;
        bool createCompleter();
        void updateCompleter();

        //flags for whether paren or braces are added for autocompletion of paren and braces. used in keypress function
        bool parenAdded = false;
        bool braceAdded = false;
        bool dblQuoteAdded = false;
        bool singleQuoteAdded = false;

        //used to see if a word should be added to the auto complete list
        bool checkForWordInList(QString word);
};


//class for the line number area.
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_H
