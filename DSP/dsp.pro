################################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
################################################################

INCLUDEPATH += /usr/local/qwt-6.1.0-rc3/include \
               /usr/include/qt4/Qt              \
               /home/adrian/Downloads/tinyalsa/include

LIBS += -L/usr/local/qwt-6.1.0-rc3/lib/ -lqwt \
        -L/home/adrian/Downloads/tinyalsa -ltinyalsa \
        -L/usr/local/lib -lfftw3 \
        -lm

TARGET       = DSP

HEADERS = \
    plot.h \
    mainwindow.h \
    includes.h \
    rightpannel.h \
    leftpanel.h \
    wavframes.h \
    coefab.h \
    processingthread.h

SOURCES = \
    plot.cpp \
    mainwindow.cpp \
    main.cpp \
    rightpannel.cpp \
    leftpanel.cpp \
    coefab.cpp \
    processingthread.cpp

