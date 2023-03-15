HEADERS += \
    board.h \
    choosedlg.h \
    ctrlpanel.h \
    login.h \
    mainwnd.h \
    singlegame.h \
    Step.h \
    stone.h \
    netgame.h \
    my_types.h \
    net_msg.pb.h \
    tool.h \
    user_data.h \
    in_game_msg.pb.h \

SOURCES += \
    board.cpp \
    choosedlg.cpp \
    ctrlpanel.cpp \
    login.cpp \
    main.cpp \
    mainwnd.cpp \
    singlegame.cpp \
    Step.cpp \
    stone.cpp \
    netgame.cpp \
    net_msg.pb.cc \
    tool.cpp \
    user_data.cpp \
    in_game_msg.pb.cc \

INCLUDEPATH += D:\mylibs\protobuf-cpp-3.15.0\protobuf-3.15.0\src

LIBS += -LD:\mylibs\protobuf-cpp-3.15.0\protobuf-3.15.0\my_build -lprotobuf

QT += widgets gui network

FORMS += \
    login.ui

