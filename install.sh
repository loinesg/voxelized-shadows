qmake -project \
    TARGET=VoxelShadows \
    CONFIG+=debug_and_release \
    CONFIG+=c++11 \
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
