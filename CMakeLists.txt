cmake_minimum_required(VERSION 3.19)

project(AA2QtEdit
        VERSION 2.0.0
        DESCRIPTION "Artificial Academy 2 Character Editor"
        LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Core5Compat)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core5Compat)

set(PROJECT_SOURCES
        src/main.cpp
        src/carddatamodel.cpp
        src/cardfile.cpp
        src/cardlistmodel.cpp
        src/classsavecardlistmodel.cpp
        src/clothdata.cpp
        src/constants.cpp
        src/datablock.cpp
        src/dataenumerable.cpp
        src/datareader.cpp
        src/datastruct.cpp
        src/datatype.cpp
        src/dictionary.cpp
        src/filesystemcardlistmodel.cpp
        src/filesystemcardlistmodelloader.cpp
        src/pngimage.cpp
        src/settings.cpp
        src/ui/mainwindow.ui
        src/ui/classsavedata.ui
        src/ui/preferences.ui
        src/ui/cardview.ui
        src/ui/carddatadelegate.cpp
        src/ui/carddataview.cpp
        src/ui/cardlistdelegate.cpp
        src/ui/cardlistview.cpp
        src/ui/cardview.cpp
        src/ui/classsavedata.cpp
        src/ui/coloritemeditor.cpp
        src/ui/filedialog.cpp
        src/ui/mainwindow.cpp
        src/ui/preferences.cpp
        icons/icons.qrc
        XML/datadef.qrc
)

qt_add_executable(AA2QtEdit
    ${PROJECT_SOURCES}
)

target_link_libraries(AA2QtEdit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Core5Compat)

set_target_properties(AA2QtEdit PROPERTIES
    WIN32_EXECUTABLE TRUE
)
