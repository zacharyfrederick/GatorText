#include "gatortext.h"
#include "codeeditor.h"
#include <QMenuBar>
#include <QIcon>
#include "highlighter.h"
#include <iostream>
#include <QWebEngineView>
#include <QUrl>
#include "findreplacedialog.h"
#include "changefontdialog.h"
#include "changefontsizedialog.h"

GatorText::GatorText() {
    editor = new CodeEditor();
    highlighter = new Highlighter(editor->document());
    setCentralWidget(editor);
    //creates the menu and toolbar
    createMenus();
    //creates all the actions neccessary and usually conenct them to their functions
    createActions();
    //create web docks
    createWebDock();
    //creates the code viewer dock
    createWordDock();
    //update the list automatically
    updateMasterList();
}

void GatorText::createWebDock() {
    //i have to create the web engine in this function
    //if i attempt to declare it in my header file the compiler crashes
    //idk what the hell is wrong with it. I can later access it by finding it through
    //its children so its not a big deal
    QString finalUrl = url + urlModifier;
    QWebEngineView* webEngine = new QWebEngineView(this);
    webEngine->setUrl(finalUrl);
    dock = new  QDockWidget("Documentation Viewer", this);
    //i only want the docks on the left or right
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(webEngine);
    dock->setVisible(false);

    addDockWidget(Qt::RightDockWidgetArea, dock);

    toolsMenu->addAction(dock->toggleViewAction());

    //default window title when an unsaved window is open
    setWindowTitle("GatorText - Untitled");
}

void GatorText::createWordDock() {
    dock = new QDockWidget("Function and Variable explorer", this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    //set it invisible by default
    dock->setVisible(false);

    QListWidget* list = new QListWidget(this);
    list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(list, &QListWidget::customContextMenuRequested, this, &GatorText::listItemContextSelected);
    list->addItems(*masterListPtr);
    dock->setWidget(list);
    addDockWidget(Qt::RightDockWidgetArea, dock);


}

void GatorText::createMenus() {
    fileMenu = menuBar()->addMenu("&File");
    editMenu = menuBar()->addMenu("&Edit");
    viewMenu = menuBar()->addMenu("&View");
    optionsMenu = menuBar()->addMenu("&Options");
    helpMenu = menuBar()->addMenu("&Help");

    toolsMenu = viewMenu->addMenu("&Tools");

    toolBar = addToolBar("&File");
    //I dont want anyone to move the toolbar
    toolBar->setMovable(false);
}

void GatorText::createActions() {
    //load all the icons for the actions with icons
    const QIcon newIcon = QIcon(":/new.png");
    const QIcon openIcon = QIcon(":/open.png");
    const QIcon saveIcon = QIcon(":/save.png");
    const QIcon cutIcon = QIcon(":/cut.png");
    const QIcon pasteIcon = QIcon(":/paste.png");
    const QIcon copyIcon = QIcon(":/copy.png");
    const QIcon saveAsIcon = QIcon(":/saveas.png");

    //declare all the actions
    QAction* newAction = new QAction(newIcon, "&New", this);
    QAction* openAction = new QAction(openIcon, "&Open", this);
    QAction* saveAction = new QAction(saveIcon, "&Save", this);
    QAction* cutAction = new QAction(cutIcon, "&Cut", this);
    QAction* copyAction = new QAction(copyIcon, "&Copy", this);
    QAction* pasteAction = new QAction(pasteIcon, "&Paste", this);
    QAction* saveAsAction = new QAction(saveAsIcon, "&Save as", this);
    QAction* commentOutAction = new QAction("&Comment out selected text", this);
    QAction* unCommentOutAction = new QAction("&Uncomment out selected Text", this);
    QAction* quitAction = new QAction("&Exit Program", this);
    QAction* displayCodeViewerAction = new QAction("&Toggle Code Viewer", this);
    QAction* findWordAction = new QAction ("&Find/Replace", this);
    QAction* undoAction = new QAction("&Undo", this);
    QAction* redoAction = new QAction("&Redo", this);
    QAction* tabAction = new QAction("&Add indentation");
    QAction* changeFontAction = new QAction("&Change Font");
    QAction* changeFontSizeAction = new QAction("&Change Font Size");
    QAction* darkAction = new QAction("&Dark");
    QAction* darkOrangeAction = new QAction("&Dark Orange");
    QAction* darkFusionAction = new QAction("&Dark Fusion");
    QAction* defaultThemeAction = new QAction("Default");
    QAction* aboutAction = new QAction("&About");

    //view language actions
    QAction* cppAction = new QAction("&C++", this);
    QAction* cAction = new QAction("&C", this);
    QAction* pythonAction = new QAction("&Python", this);
    QAction* javascriptAction = new QAction("&Javascript", this);
    QAction* javaAction = new QAction("&Java", this);

    //set language options to be checkable and set the default to C++
    cppAction->setCheckable(true);
    cppAction->setChecked(true);
    cAction->setCheckable(true);
    pythonAction->setCheckable(true);
    javascriptAction->setCheckable(true);
    javaAction->setCheckable(true);

    //setup shortcut sequences for several actions. all shortcuts are the commong default
    //so it should be intuitive for the user
    newAction->setShortcut(QKeySequence::New);
    openAction->setShortcut(QKeySequence::Open);
    saveAction->setShortcut(QKeySequence::Save);
    cutAction->setShortcut(QKeySequence::Cut);
    copyAction->setShortcut(QKeySequence::Copy);
    pasteAction->setShortcut(QKeySequence::Paste);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    quitAction->setShortcut(QKeySequence::Quit);
    undoAction->setShortcut(QKeySequence::Undo);
    redoAction->setShortcut(QKeySequence::Redo);

    //handle all connections
    //normal connections
    connect(saveAction, &QAction::triggered, this, &GatorText::save);
    connect(newAction, &QAction::triggered, this, &GatorText::newFile);
    connect(openAction, &QAction::triggered, this, &GatorText::openFile);
    connect(saveAsAction, &QAction::triggered, this, &GatorText::saveAs);
    connect(cutAction, &QAction::triggered, this, &GatorText::cut);
    connect(copyAction, &QAction::triggered, this, &GatorText::copy);
    connect(pasteAction, &QAction::triggered, this, &GatorText::paste);
    connect(commentOutAction, &QAction::triggered, this, &GatorText::commentOutCode);
    connect(unCommentOutAction, &QAction::triggered, this, &GatorText::unCommentOutCode);
    connect(quitAction, &QAction::triggered, this, &GatorText::quitProgram);
    connect(displayCodeViewerAction, &QAction::triggered, this, &GatorText::toggleCodeViewer);
    connect(undoAction, &QAction::triggered, editor, &CodeEditor::undo);
    connect(redoAction, &QAction::triggered, editor, &CodeEditor::redo);
    connect(findWordAction, &QAction::triggered, this, &GatorText::find);
    connect(tabAction, &QAction::triggered, this, &GatorText::addIndentation);
    connect(changeFontAction, &QAction::triggered, this, &GatorText::changeFont);
    connect(changeFontSizeAction, &QAction::triggered, this, &GatorText::changeFontSize);
    connect(aboutAction, &QAction::triggered, this, &GatorText::about);

    //lambda language connections
    //lambda features were useful because it allowed me to not use a signal mapper and cut down on the code needed
    connect(cppAction, &QAction::triggered, [=] {languageChanged(CPP);});
    connect(cAction, &QAction::triggered, [=] {languageChanged(C);});
    connect(javascriptAction, &QAction::triggered, [=] {languageChanged(JAVASCRIPT);});
    connect(javaAction, &QAction::triggered, [=] {languageChanged(JAVA);});
    connect(pythonAction, &QAction::triggered, [=] {languageChanged(PYTHON);});

    //lambda theme connections
    connect(defaultThemeAction, &QAction::triggered, [=] {changeTheme("default");});
    connect(darkAction, &QAction::triggered, [=] {changeTheme("dark");});
    connect(darkOrangeAction, &QAction::triggered, [=] {changeTheme("dark-orange");});
    connect(darkFusionAction, &QAction::triggered, [=] {changeTheme("dark-fusion");});

    //add the actions to the toolbar
    toolBar->addAction(newAction);
    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
    toolBar->addAction(saveAsAction);
    toolBar->addAction(cutAction);
    toolBar->addAction(copyAction);
    toolBar->addAction(pasteAction);

    //add the actions to the file menu
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(cutAction);
    fileMenu->addAction(copyAction);
    fileMenu->addAction(pasteAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    //add actions to the edit menu
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addAction(findWordAction);
    editMenu->addAction(commentOutAction);
    editMenu->addAction(unCommentOutAction);
    editMenu->addAction(tabAction);

    //add a separator for A E S T H E T I C S
    editMenu->addSeparator();
    languageMenu = editMenu->addMenu("Language");

    //adds the language options to the language menu
    languageMenu->addAction(cppAction);
    languageMenu->addAction(cAction);
    languageMenu->addAction(pythonAction);
    languageMenu->addAction(javascriptAction);
    languageMenu->addAction(javaAction);

    toolsMenu->addAction(displayCodeViewerAction);

    optionsMenu->addAction(changeFontAction);
    optionsMenu->addAction(changeFontSizeAction);

    helpMenu->addAction(aboutAction);

    //also couldnt declare the themeMenu in the header or I had the same issue as the webengine
    //this is the only way I can make it work
    QMenu* themeMenu = optionsMenu->addMenu("Change Theme");
    themeMenu->addAction(defaultThemeAction);
    themeMenu->addAction(darkAction);
    themeMenu->addAction(darkOrangeAction);
    themeMenu->addAction(darkFusionAction);

    //updates the window title
    setWindowTitle("GatorText - Untitled");
}

//checks if the document has been save if it has been modified.
//presents a dialog to confirm
//just like all programs
bool GatorText::checkForSave() {
    if (!editor->document()->isModified()) {
        return true;
    } else {
        const QMessageBox::StandardButton ret
                = QMessageBox::warning(this, tr("Application"),
                                       tr("The document has been modified.\n"
                                          "Do you want to save your changes?"),
                                       QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        switch (ret) {
        case QMessageBox::Save:
            return save();
        case QMessageBox::Cancel:
            return false;
        default:
            break;
        }
        return true;
    }
}


//checks for save before closing
void GatorText::closeEvent(QCloseEvent* event) {
    (checkForSave()) ? event->accept() : event->ignore();
}


//creates new file
void GatorText::newFile() {
    if (checkForSave()) {
        editor->clear();
        setCurrentFile(QString());
    }
    setWindowTitle("GatorText - Untitled");
}

//open file action. not responsible for loading the file
void GatorText::openFile() {
    if (checkForSave()) {
        QString filename = QFileDialog::getOpenFileName(this);
        if (!filename.isEmpty()) {
            loadFile(filename);
        }
    }
}

//save file action. not responsible for saving the data
bool GatorText::save() {
    if (currentFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(currentFile);
    }
}


//save as action. Also not responsible for saving the data. just a check function
bool GatorText::saveAs() {
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted) {
        return false;
    } else {
        return saveFile(dialog.selectedFiles().first());
    }
}

//function responsible for loading a file and setting the editor to display it
void GatorText::loadFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return;
    } else {
        QTextStream in(&file);

        editor->setPlainText(in.readAll());
        setCurrentFile(filename);
        setWindowTitle("GatorText - " + filename);

        QFileInfo fileInfo(filename);
        QString fileType = fileInfo.completeSuffix();

        LanguageType newLanguage;

        //reads the language type by file extension
        //and chnages the language type to match
        if (fileType == "cpp") {
            newLanguage = CPP;
        } else if (fileType == "c") {
            newLanguage = C;
        } else if (fileType == "py") {
            newLanguage = PYTHON;
        } else if (fileType == "js") {
            newLanguage = JAVASCRIPT;
        } else if (fileType == "java") {
            newLanguage = JAVA;
        } else {
            newLanguage = NOTSET;
        }

        //only change the language if its different than the other one
        if (newLanguage != NOTSET && newLanguage != selectedLanguage) {
            languageChanged(newLanguage);
            QMessageBox languageUpdateBox;
            //display an update so the use knows
            languageUpdateBox.setText(fileType + " file detected. Switching language to " + mapLanguageToEnum(newLanguage));
            languageUpdateBox.exec();
        }
    }
}


//responsible for saving the file data. Used by save and save as function
bool GatorText::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    //changes the currsor to waiting if reading is slow
    QTextStream out(&file);
    #ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
    #endif
    out << editor->toPlainText();
    #ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
    #endif

    QFileInfo fileInfo(fileName);
    QString fileType = fileInfo.completeSuffix();

    LanguageType newLanguage;

    //reads the language type and updates the language
    if (fileType == "cpp") {
        newLanguage = CPP;
    } else if (fileType == "c") {
        newLanguage = C;
    } else if (fileType == "py") {
        newLanguage = PYTHON;
    } else if (fileType == "js") {
        newLanguage = JAVASCRIPT;
    } else if (fileType == "java") {
        newLanguage = JAVA;
    } else {
        newLanguage = NOTSET;
    }

    if (newLanguage != NOTSET && newLanguage != selectedLanguage) {
        languageChanged(newLanguage);
        QMessageBox languageUpdateBox;
        languageUpdateBox.setText(fileType + " file detected. Switching language to " + mapLanguageToEnum(newLanguage));
        languageUpdateBox.exec();
    }

    setCurrentFile(fileName);
    this->setWindowTitle("GatorText - " + fileName);
    return true;
}

//sets the current file and updates the window title
void GatorText::setCurrentFile(const QString& filename) {
    currentFile = filename;
    editor->document()->setModified(false);
    QString shownName = "GatorText - " + currentFile;
    if (currentFile.isEmpty()) {
        shownName = "GatorText - untitled.txt";
    }
}

//changes the language and updates the completer
//loads the neccessary text file and updates the completer
void GatorText::languageChanged(LanguageType type) {
    this->selectedLanguage = type;

    QString actionName;

    switch(selectedLanguage) {

    case CPP:
        urlModifier = "cpp";
        actionName = "C++";
        break;
    case C:
        urlModifier = "c";
        actionName = "C";
        break;
    case JAVASCRIPT:
        urlModifier = "javascript";
        actionName = "Javascript";
        break;
    case PYTHON:
        urlModifier = "python";
        actionName = "Python";
        break;
    case JAVA:
        urlModifier = "openjdk";
        actionName = "Python";
        break;
    default:
        break;

    }

    QList<QAction*> actions = languageMenu->actions();

    foreach(QAction* action, actions) {
        if (action->iconText() != actionName) {
            action->setChecked(false);
        }

    }

    QString finalUrl = url + urlModifier;
    changeWebUrl(finalUrl);

    if (!editor->updateCompleter(type)) {
        QMessageBox errorBox;
        errorBox.setText("Error. Unable to open wordlist. AutoComplete Disabled");
        errorBox.exec();
    }

    updateMasterList();
    highlighter->updateKeyWordList(masterListPtr);

}

void GatorText::updateMasterList() {
    masterListPtr = editor->getMasterList();
}

void GatorText::updateAdditionalItems() {
    additionalItemsPtr = editor->getAdditionalItems();
}


//comment out code action
void GatorText::commentOutCode() {
    QTextCursor tc = editor->textCursor();
    if (tc.selectedText() != "") {
        QString tempString = tc.selection().toPlainText();
        tempString.prepend("//");
        tempString.replace("\n", "\n//");
        tc.removeSelectedText();
        tc.insertText(tempString);
        editor->setTextCursor(tc);
    }
}

//uncomment out code action
void GatorText::unCommentOutCode() {
    QTextCursor tc = editor->textCursor();
    if (tc.selectedText() != "") {
        QString tempString = tc.selection().toPlainText();
        if (tempString.left(2) == "//") {
            tempString = tempString.right(tempString.length() -2);
        }
        tempString.replace("\n//", "\n");
        tc.removeSelectedText();
        tc.insertText(tempString);
        editor->setTextCursor(tc);
    }
}

//quits the program
void GatorText::quitProgram() {
    if (checkForSave()) {
        QApplication::quit();
    }
}

//displays the code viewer
void GatorText::toggleCodeViewer() {
    QList<QDockWidget*> dockWidgets = this->findChildren<QDockWidget*>();
    foreach (QDockWidget* widget, dockWidgets) {
        QString widgetName = widget->widget()->metaObject()->className();
        if (widgetName == "QListWidget") {
            if (widget->isVisible()) {
                widget->setVisible(false);
                return;
            }
            updateAdditionalItems();
            QListWidget* listWidget = (QListWidget*) widget->widget();
            listWidget->clear();
            listWidget->addItems(*additionalItemsPtr);
            widget->setVisible(true);
        }
    }
}

//used by the code viewer to view all occurences of a function or var in code
void GatorText::highlightSelectedWord(QListWidgetItem *item) {
    QList<QTextEdit::ExtraSelection> selections = editor->extraSelections();
    QTextCursor cursor(editor->document());
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow);

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = editor->document()->find(item->text(), cursor, QTextDocument::FindWholeWords);
        //cursor.select(QTextCursor::WordUnderCursor);

        //extra selections overrides text format so we have to check for that
        if (cursor.block() == selections[0].cursor.block()) {
            QTextEdit::ExtraSelection selection;
            selection.cursor = cursor;
            selection.format.setBackground(Qt::yellow);
            selections.append(selection);
            editor->setExtraSelections(selections);
        } else {
            cursor.mergeCharFormat(highlightFormat);
        }
    }
}

void GatorText::unHighlightSelectedWord(QListWidgetItem *item) {
    QList<QTextEdit::ExtraSelection> selections = editor->extraSelections();

    selections.clear();
    editor->setExtraSelections(selections);
    editor->highlightCurrentLine();

    QTextCursor cursor(editor->document());
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::white);

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = editor->document()->find(item->text(), cursor, QTextDocument::FindWholeWords);
        cursor.select(QTextCursor::WordUnderCursor);


            cursor.mergeCharFormat(highlightFormat);
    }
}

//creates a context menu for when a code viewer item is right clicked
void GatorText::listItemContextSelected(const QPoint &point) {
    QList<QDockWidget*> dockWidgets = this->findChildren<QDockWidget*>();
    foreach (QDockWidget* widget, dockWidgets) {
        QString widgetName = widget->widget()->metaObject()->className();
        if (widgetName == "QListWidget") {
            QListWidget* list = (QListWidget*) widget->widget();
            QListWidgetItem* item = list->itemAt(point);
            QString selectedWord = item->text();

            QPoint globalPos = list->mapToGlobal(point);

            QMenu contextMenu;

            QAction* highlightWordAction = new QAction("&Highlight word", this);
            QAction* unselectWordAction = new QAction("&Unhighlight word", this);

            contextMenu.addAction(highlightWordAction);
            contextMenu.addAction(unselectWordAction);

            connect(highlightWordAction, &QAction::triggered, [=] {highlightSelectedWord(item);});
            connect(unselectWordAction, &QAction::triggered, [=] {unHighlightSelectedWord(item);});
            contextMenu.exec(globalPos);
        }
    }
}

//cut action
void GatorText::cut() {
    QClipboard* clipboard = QApplication::clipboard();
    QTextCursor cursor = editor->textCursor();
    QString selectedText = cursor.selectedText();

    if (selectedText != "") {
        cursor.removeSelectedText();
        clipboard->setText(selectedText);
    }
}


//copy action
void GatorText::copy() {
    QClipboard* clipboard = QApplication::clipboard();
    QTextCursor cursor = editor->textCursor();
    QString selectedText = cursor.selectedText();

    if (selectedText != "") {
        clipboard->setText(selectedText);
    }
}

void GatorText::paste() {
    QClipboard* clipboard = QApplication::clipboard();
    QTextCursor cursor = editor->textCursor();

    if (clipboard->mimeData()->hasText()) {
        cursor.insertText(clipboard->mimeData()->text());
        clipboard->clear();
    }
}

//helper function that mapes a languagetype to a string for display purposes
//should rewrite the wordlist to use this same function but it basically does the same process again.
QString GatorText::mapLanguageToEnum(LanguageType type) {
    QString language;

    switch (type) {
        case CPP:
            language = "C++";
            break;
        case C:
            language = "C";
            break;
        case JAVASCRIPT:
            language = "Javascript";
            break;
        case PYTHON:
            language = "Python";
            break;
        case JAVA:
            language = "Python";
            break;
        case NOTSET:
            language = "You Should Never See this";
        break;
    }

    return language;
}

//check if current file exist
bool GatorText::currentFileExist() {
    if (currentFile.isEmpty()) {
        return false;
    } else {
        return true;
    }
}

//opens the find and replace dialog
void GatorText::find() {
    FindReplaceDialog* dialog = new FindReplaceDialog(this);
    dialog->setEditor(editor);
    dialog->show();
}

//indents code
void GatorText::addIndentation() {
    QTextCursor tc = editor->textCursor();
    QString selectedText = tc.selection().toPlainText();

    if (!selectedText.isEmpty()) {
        selectedText.prepend("\t");
        selectedText.replace(QRegularExpression("\n"), "\n\t");
        tc.removeSelectedText();
        tc.insertText(selectedText);
    }
}

//open the change font dialog
void GatorText::changeFont() {
    ChangeFontDialog* dialog = new ChangeFontDialog(this);
    dialog->setEditor(editor);
    dialog->show();
}

//change font size dialog
void GatorText::changeFontSize() {
    ChangeFontSizeDialog* dialog = new ChangeFontSizeDialog(this);
    dialog->setEditor(editor);
    dialog->show();
}


//i couldnt access the web engine normally so i need to do this shit and find
//it through the parent then update the page
void GatorText::changeWebUrl(QString url) {
    QList<QDockWidget*> widgets = this->findChildren<QDockWidget*>();

    foreach (QDockWidget* widget, widgets) {
        QString name = widget->widget()->metaObject()->className();

        if (name == "QWebEngineView") {
            QWebEngineView* engine = (QWebEngineView*) widget->widget();
            engine->setUrl(url);
        }
    }
}

//change the theme
void GatorText::changeTheme(QString theme) {
    QString filename;
    QString stylesheet;

    if (theme == "default") {
        stylesheet = "";
    }

    //i had to do this because this file wouldnt work either. Any time I would try it would crash the
    //compiler like the webengine. Idk why but this works so dont touch
    if (theme == "dark") {
       stylesheet = R"(/*
               Copyright 2013 Emanuel Claesson

               Licensed under the Apache License, Version 2.0 (the "License");
               you may not use this file except in compliance with the License.
               You may obtain a copy of the License at

                   http://www.apache.org/licenses/LICENSE-2.0

               Unless required by applicable law or agreed to in writing, software
               distributed under the License is distributed on an "AS IS" BASIS,
               WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
               See the License for the specific language governing permissions and
               limitations under the License.
           */

           /*
               COLOR_DARK     = #191919
               COLOR_MEDIUM   = #353535
               COLOR_MEDLIGHT = #5A5A5A
               COLOR_LIGHT    = #DDDDDD
               COLOR_ACCENT   = #3D7848
           */

           * {
               background: #191919;
               color: #DDDDDD;
               border: 1px solid #5A5A5A;
           }

           QWidget::item:selected {
               background: #3D7848;
           }

           QCheckBox, QRadioButton {
               border: none;
           }

           QRadioButton::indicator, QCheckBox::indicator {
               width: 13px;
               height: 13px;
           }

           QRadioButton::indicator::unchecked, QCheckBox::indicator::unchecked {
               border: 1px solid #5A5A5A;
               background: none;
           }

           QRadioButton::indicator:unchecked:hover, QCheckBox::indicator:unchecked:hover {
               border: 1px solid #DDDDDD;
           }

           QRadioButton::indicator::checked, QCheckBox::indicator::checked {
               border: 1px solid #5A5A5A;
               background: #5A5A5A;
           }

           QRadioButton::indicator:checked:hover, QCheckBox::indicator:checked:hover {
               border: 1px solid #DDDDDD;
               background: #DDDDDD;
           }

           QGroupBox {
               margin-top: 6px;
           }

           QGroupBox::title {
               top: -7px;
               left: 7px;
           }

           QScrollBar {
               border: 1px solid #5A5A5A;
               background: #191919;
           }

           QScrollBar:horizontal {
               height: 15px;
               margin: 0px 0px 0px 32px;
           }

           QScrollBar:vertical {
               width: 15px;
               margin: 32px 0px 0px 0px;
           }

           QScrollBar::handle {
               background: #353535;
               border: 1px solid #5A5A5A;
           }

           QScrollBar::handle:horizontal {
               border-width: 0px 1px 0px 1px;
           }

           QScrollBar::handle:vertical {
               border-width: 1px 0px 1px 0px;
           }

           QScrollBar::handle:horizontal {
               min-width: 20px;
           }

           QScrollBar::handle:vertical {
               min-height: 20px;
           }

           QScrollBar::add-line, QScrollBar::sub-line {
               background:#353535;
               border: 1px solid #5A5A5A;
               subcontrol-origin: margin;
           }

           QScrollBar::add-line {
               position: absolute;
           }

           QScrollBar::add-line:horizontal {
               width: 15px;
               subcontrol-position: left;
               left: 15px;
           }

           QScrollBar::add-line:vertical {
               height: 15px;
               subcontrol-position: top;
               top: 15px;
           }

           QScrollBar::sub-line:horizontal {
               width: 15px;
               subcontrol-position: top left;
           }

           QScrollBar::sub-line:vertical {
               height: 15px;
               subcontrol-position: top;
           }

           QScrollBar:left-arrow, QScrollBar::right-arrow, QScrollBar::up-arrow, QScrollBar::down-arrow {
               border: 1px solid #5A5A5A;
               width: 3px;
               height: 3px;
           }

           QScrollBar::add-page, QScrollBar::sub-page {
               background: none;
           }

           QAbstractButton:hover {
               background: #353535;
           }

           QAbstractButton:pressed {
               background: #5A5A5A;
           }

           QAbstractItemView {
               show-decoration-selected: 1;
               selection-background-color: #3D7848;
               selection-color: #DDDDDD;
               alternate-background-color: #353535;
           }

           QHeaderView {
               border: 1px solid #5A5A5A;
           }

           QHeaderView::section {
               background: #191919;
               border: 1px solid #5A5A5A;
               padding: 4px;
           }

           QHeaderView::section:selected, QHeaderView::section::checked {
               background: #353535;
           }

           QTableView {
               gridline-color: #5A5A5A;
           }

           QTabBar {
               margin-left: 2px;
           }

           QTabBar::tab {
               border-radius: 0px;
               padding: 4px;
               margin: 4px;
           }

           QTabBar::tab:selected {
               background: #353535;
           }

           QComboBox::down-arrow {
               border: 1px solid #5A5A5A;
               background: #353535;
           }

           QComboBox::drop-down {
               border: 1px solid #5A5A5A;
               background: #353535;
           }

           QComboBox::down-arrow {
               width: 3px;
               height: 3px;
               border: 1px solid #5A5A5A;
           }

           QAbstractSpinBox {
               padding-right: 15px;
           }

           QAbstractSpinBox::up-button, QAbstractSpinBox::down-button {
               border: 1px solid #5A5A5A;
               background: #353535;
               subcontrol-origin: border;
           }

           QAbstractSpinBox::up-arrow, QAbstractSpinBox::down-arrow {
               width: 3px;
               height: 3px;
               border: 1px solid #5A5A5A;
           }

           QSlider {
               border: none;
           }

           QSlider::groove:horizontal {
               height: 5px;
               margin: 4px 0px 4px 0px;
           }

           QSlider::groove:vertical {
               width: 5px;
               margin: 0px 4px 0px 4px;
           }

           QSlider::handle {
               border: 1px solid #5A5A5A;
               background: #353535;
           }

           QSlider::handle:horizontal {
               width: 15px;
               margin: -4px 0px -4px 0px;
           }

           QSlider::handle:vertical {
               height: 15px;
               margin: 0px -4px 0px -4px;
           }

           QSlider::add-page:vertical, QSlider::sub-page:horizontal {
               background: #3D7848;
           }

           QSlider::sub-page:vertical, QSlider::add-page:horizontal {
               background: #353535;
           }

           QLabel {
               border: none;
           }

           QProgressBar {
               text-align: center;
           }

           QProgressBar::chunk {
               width: 1px;
               background-color: #3D7848;
           }

           QMenu::separator {
               background: #353535;
           }))";
    }

    if (theme == "dark-orange") {
        filename = ":/darkorange.txt";
    }

    if (theme == "dark-fusion") {
        filename = ":/DarkFusion.txt";
    }

    if (!filename.isEmpty()) {
        QFile file(":/style.qss");
        file.open(QFile::ReadOnly);
        QTextStream stream(&file);
        stylesheet = stream.readAll();

        QApplication* ptr = (QApplication*) QApplication::instance();
        ptr->setStyleSheet(stylesheet);
    } else {
        QApplication* ptr = (QApplication*) QApplication::instance();
        ptr->setStyleSheet(stylesheet);
    }
}

//shows an about us dialog
void GatorText::about() {
    QMessageBox* aboutMsg = new QMessageBox(this);
    aboutMsg->setText("GatorText was created by a group of UF students for their COP3503 term project. Please give us an A :)");
    aboutMsg->exec();
}
