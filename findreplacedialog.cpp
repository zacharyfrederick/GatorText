#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"

FindReplaceDialog::FindReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);

    //manually ask for ui components
    dataLabel = ui->dataLabel;
    findTextEdit = ui->findTextEdit;
    replaceTextEdit = ui->replaceWithTextEdit;

    findBtn = ui->findNextBtn;
    replaceBtn = ui->replaceBtn;
    replaceAllBtn = ui->replaceAllBtn;
    cancelBtn = ui->cancelBtn;

    connect(findBtn, &QPushButton::clicked, this, &FindReplaceDialog::findNext);
    connect(replaceBtn, &QPushButton::clicked, this, &FindReplaceDialog::replace);
    connect(replaceAllBtn, &QPushButton::clicked, this, &FindReplaceDialog::replaceAll);
    connect(cancelBtn, &QPushButton::clicked, this, &FindReplaceDialog::close);

    dataLabel->setVisible(false);
}

void FindReplaceDialog::setEditor(CodeEditor* editor) {
    this->editor = editor;
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

void FindReplaceDialog::findNext() {
    if (oldFormatPosition != -1) {
        QTextCharFormat regularFormat;
        regularFormat.setBackground(Qt::white);
        QTextCursor removalCursor = editor->textCursor();
        removalCursor.setPosition(oldFormatPosition, QTextCursor::MoveAnchor);
        removalCursor.setPosition(oldFormatEndPosition, QTextCursor::KeepAnchor);
        removalCursor.setCharFormat(regularFormat);
        oldFormatPosition = -1;
        oldFormatEndPosition = -1;
    }
    replacedOnce = false;
    QString textToFind = findTextEdit->text();

    if (!textToFind.isEmpty()) {
        if (textToFind == previousFind) {
            cursor = editor->document()->find(QRegularExpression(textToFind), cursor);
        } else {
            wordFoundBefore = false;
            cursor = editor->document()->find(QRegularExpression(textToFind));
        }
    }

   if (cursor.position() == -1) {
        dataLabel->setText("Error. No matches found");
        dataLabel->setStyleSheet("QLabel {color:red}");
        dataLabel->setVisible(true);

   } else if (cursor.position() != -1) {
       QTextCharFormat highlightFormat;
       highlightFormat.setBackground(Qt::yellow);
       cursor.setCharFormat(highlightFormat);
       oldFormatPosition = cursor.selectionStart();
       oldFormatEndPosition = cursor.selectionEnd();
       dataLabel->setStyleSheet("QLabel {color:black}");
       dataLabel->setText("\"" + textToFind + "\" found at cursor position: " + QString::number(cursor.position()));
       dataLabel->setVisible(true);
       wordFoundBefore = true;

       //focus in on word
       int currentBlock = cursor.block().blockNumber();
       editor->verticalScrollBar()->setSliderPosition(currentBlock);
   }

    previousFind = textToFind;
}

void FindReplaceDialog::replace() {
    QString replaceWith = replaceTextEdit->text();

    if (cursor.position() != -1 && replacedOnce == false) {
        cursor.removeSelectedText();
        cursor.insertText(replaceWith);
        replacedOnce = true;
    }
}

void FindReplaceDialog::replaceAll() {
    QString wordToReplace = findTextEdit->text();
    QString replaceWith = replaceTextEdit->text();

    cursor = editor->document()->find(QRegularExpression(wordToReplace));

    do {
        cursor = editor->document()->find(QRegularExpression(wordToReplace));
        if (cursor.position() != -1) {
            cursor.removeSelectedText();
            cursor.insertText(replaceWith);
        }
    } while (cursor.position() != -1);
}

void FindReplaceDialog::closeEvent(QCloseEvent *e) {
    if (oldFormatPosition != -1) {
        QTextCharFormat regularFormat;
        regularFormat.setBackground(Qt::white);
        QTextCursor removalCursor = editor->textCursor();
        removalCursor.setPosition(oldFormatPosition, QTextCursor::MoveAnchor);
        removalCursor.setPosition(oldFormatEndPosition, QTextCursor::KeepAnchor);
        removalCursor.setCharFormat(regularFormat);
        oldFormatPosition = -1;
        oldFormatEndPosition = -1;
    }

    QDialog::closeEvent(e);
}
