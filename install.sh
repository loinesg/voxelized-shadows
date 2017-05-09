qmake -project \
    CONFIG+=c++11 \
    CONFIG-=app_bundle \
    QT+=opengl \
    QT+=gui \
    "INCLUDEPATH += . Source/" \
    "INCLUDEPATH += . Source/Rendering" \
    "INCLUDEPATH += . Source/Interface" \
    "INCLUDEPATH += . Source/Math" \
    "INCLUDEPATH += . Source/Assets" \
    "INCLUDEPATH += . Source/Voxels" \
    "INCLUDEPATH += . Source/Scene"

qmake
make
