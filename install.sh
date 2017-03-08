qmake -project \
    CONFIG+=c++11 \
    QT+=opengl \
    QT+=gui \
    "INCLUDEPATH += . Source/" \
    "INCLUDEPATH += . Source/Rendering" \
    "INCLUDEPATH += . Source/Interface" \
    "INCLUDEPATH += . Source/Math" \
    "INCLUDEPATH += . Source/Assets" \
    "INCLUDEPATH += . Source/Scene"

qmake
make
