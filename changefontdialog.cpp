#include "changefontdialog.h"
#include "ui_changefontdialog.h"
#include <QDebug>

ChangeFontDialog::ChangeFontDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeFontDialog)
{
    ui->setupUi(this);

    fontBox = ui->fontComboBox;
    changeFontButton = ui->changeFontBtn;

    connect(changeFontButton, &QPushButton::pressed, this, &ChangeFontDialog::changeFont);
}

ChangeFontDialog::~ChangeFontDialog()
{
    delete ui;
}

void ChangeFontDialog::changeFont() {
    QFont newFont = fontBox->currentFont();
    QString fontName = newFont.toString();

    int size = editorPtr->font().pointSize();

    //sets the editor details to monospace and fixed pictch so that it looks like an ida
    //idk what that stuff means but  you can tell when its not added for sure
    newFont.setPointSize(size);
    newFont.setStyleHint(QFont::Monospace);
    newFont.setFixedPitch(true);

    editorPtr->setFont(newFont);

    this->close();
}

//we need the editor to get access to the current font and be able to set it.
//TODO rewrite editor to have a get font and set font fucntion
void ChangeFontDialog::setEditor(CodeEditor *editor) {
    editorPtr = editor;
}
