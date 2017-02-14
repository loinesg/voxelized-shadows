qmake -project \
    CONFIG+=c++11 \
    QT+=opengl \
    QT+=gui \
    "INCLUDEPATH += . Source/Rendering" \
    "INCLUDEPATH += . Source/Interface"

qmake
make
