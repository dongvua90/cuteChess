include(boardview/boardview.pri)
DEPENDPATH += $$PWD
HEADERS += $$PWD/chessclock.h \
    $$PWD/mainwindow.h \
    $$PWD/newgamedlg.h \
    $$PWD/cutechessapp.h \
    $$PWD/movelist.h \
    $$PWD/gameviewer.h \
    $$PWD/pgntoken.h \
    $$PWD/movenumbertoken.h \
    $$PWD/movetoken.h \
    $$PWD/movecommenttoken.h \
    $$PWD/gametabbar.h
SOURCES += $$PWD/main.cpp \
    $$PWD/chessclock.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/newgamedlg.cpp \
    $$PWD/cutechessapp.cpp \
    $$PWD/movelist.cpp \
    $$PWD/gameviewer.cpp \
    $$PWD/pgntoken.cpp \
    $$PWD/movenumbertoken.cpp \
    $$PWD/movetoken.cpp \
    $$PWD/movecommenttoken.cpp \
    $$PWD/gametabbar.cpp
