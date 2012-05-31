if (NOT WIN32)
   add_definitions("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
endif ()

find_package(Qt4 REQUIRED QtCore QtGui)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

set(librelibrecell_SOURCES
   about.cpp
   card.cpp
   columnlayout.cpp
   column.cpp
   cornerwidget.cpp
   librelibrecell.cpp
   main.cpp
   preferences.cpp
)

set(librelibrecell_HEADERS
   about.h
   card.h
   columnlayout.h
   column.h
   cornerwidget.h
   librelibrecell.h
   preferences.h
)

set(librelibrecell_LIBS
   ${QT_LIBRARIES}
)

set(librelibrecell_RESOURCES
   ../librelibrecell.qrc
)

qt4_add_resources(librelibrecell_RESOURCES_RCC ${librelibrecell_RESOURCES})
qt4_wrap_cpp(librelibrecell_HEADERS_MOC ${librelibrecell_HEADERS})

add_executable(librelibrecell
   ${librelibrecell_SOURCES}
   ${librelibrecell_HEADERS_MOC}
   ${librelibrecell_RESOURCES_RCC}
)

target_link_libraries(librelibrecell ${librelibrecell_LIBS})
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS librelibrecell RUNTIME DESTINATION bin)
