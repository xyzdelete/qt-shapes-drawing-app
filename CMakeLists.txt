cmake_minimum_required(
  VERSION 3.21
)

project(
  qt-shapes-drawing-app
  LANGUAGES CXX
)

# Force Qt to honor NEW behavior of CMP0156
set(
  QT_FORCE_CMP0156_TO_VALUE
  NEW
)

if(
  WIN32
  AND
    (NOT
     DEFINED
     Qt6_DIR
     OR Qt6_DIR
        STREQUAL
        ""
    )
  AND
    (NOT
     DEFINED
     CMAKE_PREFIX_PATH
     OR CMAKE_PREFIX_PATH
        STREQUAL
        ""
    )
)
  file(
    GLOB
    _qt_versions
    "C:/Qt/6.*"
  )
  list(
    SORT
    _qt_versions
    COMPARE NATURAL
  )
  list(
    REVERSE
    _qt_versions
  ) # prefer newest version first

  set(
    _qt_msvc_suffixes
    msvc2026_64
    msvc2022_64
    msvc2019_64
  )

  foreach(
    _qt_ver
    ${_qt_versions}
  )
    foreach(
      _qt_suffix
      ${_qt_msvc_suffixes}
    )
      set(
        _qt_guess
        "${_qt_ver}/${_qt_suffix}"
      )
      if(
        EXISTS
        "${_qt_guess}/lib/cmake/Qt6"
      )
        set(
          QT6_INSTALL_PREFIX
          "${_qt_guess}"
          CACHE PATH
                "Path to Qt6 installation root"
                FORCE
        )
        break()
      endif()
    endforeach()
    if(
      QT6_INSTALL_PREFIX
    )
      break()
    endif()
  endforeach()
endif()

if(
  QT6_INSTALL_PREFIX
)
  list(
    PREPEND
    CMAKE_PREFIX_PATH
    ${QT6_INSTALL_PREFIX}
    ${QT6_INSTALL_PREFIX}/lib/cmake
  )
endif()

set(
  CMAKE_CXX_STANDARD
  23
)
set(
  CMAKE_CXX_STANDARD_REQUIRED
  ON
)
set(
  CMAKE_CXX_EXTENSIONS
  OFF
)
set(
  CMAKE_AUTOMOC
  ON
)
set(
  CMAKE_AUTOUIC
  ON
)
set(
  CMAKE_AUTORCC
  ON
)

find_package(
  Qt6
  6
  REQUIRED
  COMPONENTS Core
             Widgets
)

qt_standard_project_setup()

qt_add_executable(
  ${CMAKE_PROJECT_NAME}
  WIN32
  MACOSX_BUNDLE
  main.cpp
  mainwindow.cpp
  mainwindow.hpp
  mainwindow.ui
  paintcanvas.hpp
  paintcanvas.cpp
  resources.qrc
)

target_link_libraries(
  ${CMAKE_PROJECT_NAME}
  PRIVATE Qt::Core
          Qt::Widgets
)

# On Windows/MSVC, run windeployqt after each non static build so the exe has Qt
# DLLs

if(
  WIN32
  AND MSVC
  AND NOT
      QT_FEATURE_static
)
  find_program(
    WINDEPLOYQT_EXECUTABLE
    NAMES windeployqt.exe
          windeployqt
    HINTS ${QT6_INSTALL_PREFIX}/bin
  )

  if(
    WINDEPLOYQT_EXECUTABLE
  )
    add_custom_command(
      TARGET ${CMAKE_PROJECT_NAME}
      POST_BUILD
      COMMAND
        "${WINDEPLOYQT_EXECUTABLE}"
        $<$<CONFIG:Debug>:--debug>$<$<NOT:$<CONFIG:Debug>>:--release>
        --no-translations --no-opengl-sw --dir
        "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
        "$<TARGET_FILE:${CMAKE_PROJECT_NAME}>"
      COMMENT "Deploying Qt runtime with windeployqt"
      VERBATIM
    )
  endif()
endif()

include(
  GNUInstallDirs
)

install(
  TARGETS ${CMAKE_PROJECT_NAME}
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
  TARGET
  ${CMAKE_PROJECT_NAME}
  OUTPUT_SCRIPT
  deploy_script
  NO_UNSUPPORTED_PLATFORM_ERROR
)

install(
  SCRIPT ${deploy_script}
)
