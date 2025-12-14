cmake_minimum_required(VERSION 3.16)
project(PhoneBook)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Qt path if specified
if(DEFINED ENV{QT_DIR})
    set(CMAKE_PREFIX_PATH "$ENV{QT_DIR};${CMAKE_PREFIX_PATH}")
elseif(EXISTS "E:/qt")
    # Try common Qt installation paths
    if(EXISTS "E:/qt/6.8.0/mingw_64")
        set(CMAKE_PREFIX_PATH "E:/qt/6.8.0/mingw_64;${CMAKE_PREFIX_PATH}")
    elseif(EXISTS "E:/qt/6.7.0/mingw_64")
        set(CMAKE_PREFIX_PATH "E:/qt/6.7.0/mingw_64;${CMAKE_PREFIX_PATH}")
    elseif(EXISTS "E:/qt/6.6.0/mingw_64")
        set(CMAKE_PREFIX_PATH "E:/qt/6.6.0/mingw_64;${CMAKE_PREFIX_PATH}")
    elseif(EXISTS "E:/qt/6.5.0/mingw_64")
        set(CMAKE_PREFIX_PATH "E:/qt/6.5.0/mingw_64;${CMAKE_PREFIX_PATH}")
    elseif(EXISTS "E:/qt/5.14.2/mingw73_64")
        set(CMAKE_PREFIX_PATH "E:/qt/5.14.2/mingw73_64;${CMAKE_PREFIX_PATH}")
    elseif(EXISTS "E:/qt/5.15.2/mingw_64")
        set(CMAKE_PREFIX_PATH "E:/qt/5.15.2/mingw_64;${CMAKE_PREFIX_PATH}")
    elseif(EXISTS "E:/qt/5.15.0/mingw_64")
        set(CMAKE_PREFIX_PATH "E:/qt/5.15.0/mingw_64;${CMAKE_PREFIX_PATH}")
    else
        # Try to find any Qt version
        file(GLOB QT_VERSIONS "E:/qt/*")
        foreach(QT_VERSION ${QT_VERSIONS})
            if(IS_DIRECTORY "${QT_VERSION}")
                # Try mingw73_64 first (Qt 5.14.2)
                if(EXISTS "${QT_VERSION}/mingw73_64")
                    set(CMAKE_PREFIX_PATH "${QT_VERSION}/mingw73_64;${CMAKE_PREFIX_PATH}")
                    break()
                # Then try mingw_64
                elseif(EXISTS "${QT_VERSION}/mingw_64")
                    set(CMAKE_PREFIX_PATH "${QT_VERSION}/mingw_64;${CMAKE_PREFIX_PATH}")
                    break()
                endif()
            endif()
        endforeach()
    endif()
endif()

# Find Qt6 or Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

# Enable automatic MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files
set(SOURCES
    contact.cpp
    phonebook.cpp
    phonenumber.cpp
    validator.cpp
    mainwindow.cpp
    main_qt.cpp
)

set(HEADERS
    contact.h
    phonebook.h
    phonenumber.h
    validator.h
    mainwindow.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

