qmake -project \
    CONFIG+=c++11 \
    QT+=opengl \
    QT+=gui \
    "INCLUDEPATH += . Source/Rendering" \
    "INCLUDEPATH += . Source/Interface" \
    "INCLUDEPATH += . Source/Math" 

qmake
make
