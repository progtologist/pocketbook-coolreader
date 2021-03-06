if (NOT DEFINED DEVICE_NAME)
  if ( ${GUI} STREQUAL CRGUI_XCB )
    SET(DEVICE_NAME all)
  else ()
    SET(DEVICE_NAME v3)
  endif ( ${GUI} STREQUAL CRGUI_XCB )
  message("Device type is not specified! It's necessary to know device type in order to choose proper keyboard layout, etc...")
  message("Using ${DEVICE_NAME} as default")
  message("Add cmake parameter -D DEVICE_NAME={v3|v5|n516|v3a} to use another device settings")
else ()
  message("Device type is set to ${DEVICE_NAME}")
endif (NOT DEFINED DEVICE_NAME)


#FIND_PACKAGE(Gettext REQUIRED)

#use macros from kwwidgets instead of usual
SET(KWWidgets_CMAKE_DIR ${CMAKE_SOURCE_DIR}/tools/)
INCLUDE(${KWWidgets_CMAKE_DIR}FindGettext.cmake)
INCLUDE(${KWWidgets_CMAKE_DIR}KWWidgetsInternationalizationMacros.cmake)

SET (LANGUAGES bg ca cs de es fr he lt nl pl ro ru uk zh_CN zh_TW)
message( "Languages: ${LANGUAGES}" )

if (DEFINED POCKETBOOK_PRO)
  SET (CR3_SOURCES 
    src/bgfit.cpp
    src/citedlg.cpp 
    src/fsmenu.cpp    
    src/mainwnd.cpp  
    src/recentdlg.cpp 
    src/selnavig.cpp 
    src/tocdlg.cpp 
    src/bmkdlg.cpp   
    src/cr3_def_cover.cpp 
    src/cr3main.cpp  
    src/dictdlg.cpp 
    src/linksdlg.cpp 
    src/numedit.cpp 
    src/scrkbd.cpp   
    src/settings.cpp 
    src/viewdlg.cpp
    src/web.cpp
    src/ota_update.cpp
    src/pb_toc.cpp
  )
else ()
  SET (CR3_SOURCES 
    src/bgfit.cpp
    src/citedlg.cpp 
    src/fsmenu.cpp    
    src/mainwnd.cpp  
    src/recentdlg.cpp 
    src/selnavig.cpp 
    src/tocdlg.cpp 
    src/bmkdlg.cpp   
    src/cr3_def_cover.cpp 
    src/cr3main.cpp  
    src/dictdlg.cpp 
    src/linksdlg.cpp 
    src/numedit.cpp 
    src/scrkbd.cpp   
    src/settings.cpp 
    src/viewdlg.cpp
  )
endif (DEFINED POCKETBOOK_PRO)

if (${GUI} STREQUAL CRGUI_PB)
    SET (CR3_STYLES
         data/epub.css  data/fb2.css  data/htm.css  data/rtf.css  data/txt.css data/chm.css data/doc.css data/dict.css
        )
    SET (CR3_SOURCES ${CR3_SOURCES} src/cr3pocketbook.cpp)
    SET (CR3_BIN_DESTINATION system/bin)
else ()
    SET (CR3_STYLES
        data/epub.css  data/fb2.css  data/htm.css  data/rtf.css  data/txt.css data/chm.css
        )
    SET (CR3_BIN_DESTINATION bin)
endif (${GUI} STREQUAL CRGUI_PB)

#   
KWWidgets_CREATE_GETTEXT_TARGETS(
   DOMAIN_NAME "cr3"
   LOCALE_LIST "${LANGUAGES}"
   SOURCES "${CR3_SOURCES}"
   PO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/po
   CREATE_POT_TARGET 1
   CREATE_PO_TARGET 1
   MO_BUILD_DIR ${CMAKE_BINARY_DIR}${CMAKE_DATA_DIR}/i18n
   ADD_MO_TARGET_TO_ALL 1
)
#   CREATE_PO_TARGET
#   CREATE_MO_TARGET

if ( ${GUI} STREQUAL CRGUI_XCB )

    ADD_DEFINITIONS( -DUSE_FONTCONFIG=1 )
    SET(CR3_SOURCES ${CR3_SOURCES} src/cr3xcb.cpp)
    SET (EXTRA_LIBS xcb xcb-aux xcb-atom xcb-image xcb-keysyms xcb-randr fontconfig )

    INSTALL( FILES ${CR3_STYLES} DESTINATION share/cr3)
    INSTALL( FILES data/desktop/cr3.desktop DESTINATION share/applications)
    INSTALL( DIRECTORY data/hyph/ DESTINATION share/cr3/hyph
      FILES_MATCHING PATTERN "*.pdb" )
    INSTALL( DIRECTORY data/hyph/ DESTINATION share/cr3/hyph
      FILES_MATCHING PATTERN "*.pattern" )
    IF (${DEVICE_NAME} STREQUAL all)
        INSTALL( DIRECTORY data/devices/v3/skins DESTINATION share/cr3/v3 )
        INSTALL( DIRECTORY data/devices/n516/skins DESTINATION share/cr3/n516 )
        INSTALL( DIRECTORY data/devices/v3/keymaps/ DESTINATION share/cr3/v3 )
        INSTALL( DIRECTORY data/devices/n516/keymaps/ DESTINATION share/cr3/n516 )
    ELSE()
        INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/skins DESTINATION share/cr3 )
        INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/keymaps DESTINATION share/cr3 )
    ENDIF(${DEVICE_NAME} STREQUAL all)
    INSTALL( DIRECTORY data/manual/ DESTINATION share/cr3/manual )
    INSTALL( DIRECTORY ${CMAKE_BINARY_DIR}${CMAKE_DATA_DIR}/i18n/ DESTINATION share/locale
      FILES_MATCHING PATTERN "*.mo" )

elseif ( ${GUI} STREQUAL CRGUI_QT )

    FIND_PACKAGE( Qt4 REQUIRED )
    INCLUDE(${QT_USE_FILE})
    
    SET(CR3_SOURCES ${CR3_SOURCES} src/cr3qt.cpp)
    SET (EXTRA_LIBS ${QT_LIBRARIES} ${STD_LIBS} )

    INSTALL( FILES ${CR3_STYLES} DESTINATION root/crengine)
    INSTALL( DIRECTORY data/hyph/ DESTINATION root/crengine/hyph
      FILES_MATCHING PATTERN "*.pdb" )
    INSTALL( DIRECTORY data/hyph/ DESTINATION root/crengine/hyph
      FILES_MATCHING PATTERN "*.pattern" )
    INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/skins/default/ DESTINATION root/crengine/skin )
    INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/keymaps/ DESTINATION root/crengine )
    INSTALL( DIRECTORY data/manual/ DESTINATION root/crengine/manual )

    FOREACH(LANGUAGE ${LANGUAGES})
        SET(MO_FILE "${CMAKE_BINARY_DIR}/i18n/${LANGUAGE}/LC_MESSAGES/cr3.mo")
        INSTALL(FILES ${MO_FILE} DESTINATION root/crengine/i18n RENAME ${LANGUAGE}.mo)
    ENDFOREACH(LANGUAGE ${LANGUAGES})

elseif ( ${GUI} STREQUAL CRGUI_NANOX )

    SET(CR3_SOURCES ${CR3_SOURCES} src/cr3nanox.cpp)
    SET (EXTRA_LIBS nano-X pthread ${DBUS_LIBRARIES} )
    if (ENABLE_DBUS_VIEWER_EVENTS)
        SET (EXTRA_LIBS ${EXTRA_LIBS} ${DBUS_LIBRARIES} )
    endif(ENABLE_DBUS_VIEWER_EVENTS)

    INSTALL( FILES ${CR3_STYLES} DESTINATION root/crengine)
    INSTALL( DIRECTORY data/hyph/ DESTINATION root/crengine/hyph
      FILES_MATCHING PATTERN "*.pdb" )
    INSTALL( DIRECTORY data/hyph/ DESTINATION root/crengine/hyph
      FILES_MATCHING PATTERN "*.pattern" )
    INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/skins/default/ DESTINATION root/crengine/skin )
    INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/keymaps/ DESTINATION root/crengine )
    INSTALL( DIRECTORY data/manual/ DESTINATION root/crengine/manual )

    FOREACH(LANGUAGE ${LANGUAGES})
        SET(MO_FILE "${CMAKE_BINARY_DIR}/i18n/${LANGUAGE}/LC_MESSAGES/cr3.mo")
        INSTALL(FILES ${MO_FILE} DESTINATION root/crengine/i18n RENAME ${LANGUAGE}.mo)
    ENDFOREACH(LANGUAGE ${LANGUAGES})

#    INSTALL( DIRECTORY ${CMAKE_BINARY_DIR}/cr3gui/po/ DESTINATION root/crengine/i18n
#      FILES_MATCHING PATTERN "*.mo" )
    SET (CR3_BIN_DESTINATION root)

elseif ( ${GUI} STREQUAL CRGUI_JINKE_PLUGIN )

    SET(CR3_SOURCES ${CR3_SOURCES} src/cr3jinke.cpp)

    INSTALL( FILES ${CR3_STYLES} DESTINATION root/crengine)
    INSTALL( DIRECTORY data/hyph/ DESTINATION root/crengine/hyph
      FILES_MATCHING PATTERN "*.pdb" )
    INSTALL( DIRECTORY data/hyph/ DESTINATION root/crengine/hyph
      FILES_MATCHING PATTERN "*.pattern" )
    INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/skins/default/ DESTINATION root/crengine/skin )
    INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/keymaps/ DESTINATION root/crengine )
    INSTALL( DIRECTORY data/manual/ DESTINATION root/crengine/manual )

    FOREACH(LANGUAGE ${LANGUAGES})
        SET(MO_FILE "${CMAKE_BINARY_DIR}/i18n/${LANGUAGE}/LC_MESSAGES/cr3.mo")
        INSTALL(FILES ${MO_FILE} DESTINATION root/crengine/i18n RENAME ${LANGUAGE}.mo)
#        INSTALL(FILES ${MO_FILE} DESTINATION root/crengine/i18n RENAME cr3_${LANGUAGE}.mo)
    ENDFOREACH(LANGUAGE ${LANGUAGES})

#    INSTALL( DIRECTORY ${CMAKE_BINARY_DIR}/cr3gui/po/ DESTINATION root/crengine/i18n
#      FILES_MATCHING PATTERN "*.mo" )
    SET (CR3_BIN_DESTINATION usr/lib)

elseif ( ${GUI} STREQUAL CRGUI_WIN32 )

    SET(CR3_SOURCES ${CR3_SOURCES} src/cr3win.cpp)

    INSTALL( FILES ${CR3_STYLES} DESTINATION crengine)
    INSTALL( DIRECTORY data/hyph/ DESTINATION crengine/hyph
      FILES_MATCHING PATTERN "*.pdb" )
    INSTALL( DIRECTORY data/hyph/ DESTINATION crengine/hyph
      FILES_MATCHING PATTERN "*.pattern" )
    INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/skins/default/ DESTINATION crengine/skin )
    INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/keymaps/ DESTINATION crengine )
    INSTALL( DIRECTORY data/manual/ DESTINATION crengine/manual )
    INSTALL( DIRECTORY ${CMAKE_BINARY_DIR}/cr3gui/po/ DESTINATION cr3/i18n
      FILES_MATCHING PATTERN "*.mo" )
    SET (CR3_BIN_DESTINATION .)

elseif (${GUI} STREQUAL CRGUI_PB)
#    SET(CR3_SOURCES ${CR3_SOURCES} src/cr3pocketbook.cpp)
    # ADD_DEFINITIONS(-Wno-format-y2k)
    # ADD_DEFINITIONS(-fomit-frame-pointer)
    # ADD_DEFINITIONS(-fsigned-char)
    # ADD_DEFINITIONS(-s)
    if (POCKETBOOK_PRO)

      Message("Will build for pocketbook PRO")
      ADD_DEFINITIONS(-DPOCKETBOOK_PRO=1)

      if (POCKETBOOK_PRO_PRO2)
        Message("Will build for firmware 2")
        ADD_DEFINITIONS(-DPOCKETBOOK_PRO_PRO2=1)
      else (POCKETBOOK_PRO_PRO2) 

        if (POCKETBOOK_PRO_FW5)
          Message("Will build for firmware 5")
          ADD_DEFINITIONS(-DPOCKETBOOK_PRO_FW5=1)
        else (POCKETBOOK_PRO_FW5) 
          Message("Will build for firmware 4")
        endif (POCKETBOOK_PRO_FW5) 

      endif (POCKETBOOK_PRO_PRO2) 

    endif (POCKETBOOK_PRO) 

    if (BACKGROUND_CACHE_FILE_CREATION)
      ADD_DEFINITIONS(-DBACKGROUND_CACHE_FILE_CREATION=1)
    endif (BACKGROUND_CACHE_FILE_CREATION) 
    FIND_LIBRARY(INKVIEW_LIB inkview)
    IF (ENABLE_PB_DB_STATE)
    FIND_LIBRARY (BOOKSTATE_LIB bookstate)
    IF (BOOKSTATE_LIB)
        ADD_DEFINITIONS( -DPB_DB_STATE_SUPPORTED=1 )
        message("bookstate library found!")
        SET (BOOKSTATE_LIB dl )
    ELSE (BOOKSTATE_LIB)
        message("bookstate library not found!")
        SET (BOOKSTATE_LIB "")
    ENDIF (BOOKSTATE_LIB)
    ENDIF (ENABLE_PB_DB_STATE)
    IF (INKVIEW_LIB)
	IF (DEFINED POCKETBOOK_EMU_GTK)
	    FIND_PACKAGE (GTK2 REQUIRED)
	    FIND_PACKAGE (CURL REQUIRED)
	ENDIF (DEFINED POCKETBOOK_EMU_GTK)
        SET (EXTRA_LIBS ${INKVIEW_LIB} ${BOOKSTATE_LIB} ${GTK2_LIBRARIES} ${CURL_LIBRARIES})
    ELSE (INKVIEW_LIB)
	  message("libinkview library not found!")
    ENDIF (INKVIEW_LIB)

    FIND_PROGRAM(ZIP_EXECUTABLE zip)
    IF(NOT ZIP_EXECUTABLE)
	MESSAGE(FATAL_ERROR "Unable to find 'zip' executable")
    ENDIF(NOT ZIP_EXECUTABLE)

    SET( cr3_skin_default ${CMAKE_CURRENT_BINARY_DIR}/default.cr3skin )
    ADD_CUSTOM_COMMAND(OUTPUT ${cr3_skin_default} COMMAND ${ZIP_EXECUTABLE} -j ${cr3_skin_default} ${CMAKE_CURRENT_SOURCE_DIR}/data/devices/${DEVICE_NAME}/skins/default/* )
    ADD_CUSTOM_TARGET( skin_default ALL DEPENDS ${cr3_skin_default} )
    
    SET( cr3_skin_pb62x ${CMAKE_CURRENT_BINARY_DIR}/pb62x.cr3skin )
    ADD_CUSTOM_COMMAND(OUTPUT ${cr3_skin_pb62x} COMMAND ${ZIP_EXECUTABLE} -j ${cr3_skin_pb62x} ${CMAKE_CURRENT_SOURCE_DIR}/data/devices/${DEVICE_NAME}/skins/pb62x/* )
    ADD_CUSTOM_TARGET( skin_pb62x ALL DEPENDS ${cr3_skin_pb62x} )

    SET( cr3_skin_pb626fw5 ${CMAKE_CURRENT_BINARY_DIR}/pb626fw5.cr3skin )
    ADD_CUSTOM_COMMAND(OUTPUT ${cr3_skin_pb626fw5} COMMAND ${ZIP_EXECUTABLE} -j ${cr3_skin_pb626fw5} ${CMAKE_CURRENT_SOURCE_DIR}/data/devices/${DEVICE_NAME}/skins/pb626fw5/* )
    ADD_CUSTOM_TARGET( skin_pb626fw5 ALL DEPENDS ${cr3_skin_pb626fw5} )

    INSTALL( FILES ${CR3_STYLES} DESTINATION system/share/cr3)
    INSTALL( DIRECTORY data/hyph/ DESTINATION system/share/cr3/hyph
      FILES_MATCHING PATTERN "*.pdb" )
    INSTALL( DIRECTORY data/hyph/ DESTINATION system/share/cr3/hyph
      FILES_MATCHING PATTERN "*.pattern" )
#    INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/skins/default/ DESTINATION system/share/cr3/skin )
    INSTALL( FILES ${cr3_skin_default} DESTINATION system/share/cr3/skins )
    INSTALL( FILES ${cr3_skin_pb62x} DESTINATION system/share/cr3/skins )
    INSTALL( FILES ${cr3_skin_pb626fw5} DESTINATION system/share/cr3/skins )
    INSTALL( DIRECTORY data/devices/${DEVICE_NAME}/keymaps/ DESTINATION system/share/cr3/keymaps )
    INSTALL( DIRECTORY data/manual/ DESTINATION system/share/cr3/manual )

    FOREACH(LANGUAGE ${LANGUAGES})
        SET(MO_FILE "${CMAKE_BINARY_DIR}/i18n/${LANGUAGE}/LC_MESSAGES/cr3.mo")
        INSTALL(FILES ${MO_FILE} DESTINATION system/share/cr3/i18n RENAME ${LANGUAGE}.mo)
    ENDFOREACH(LANGUAGE ${LANGUAGES})

endif ( ${GUI} STREQUAL CRGUI_XCB )


if ( ${GUI} STREQUAL CRGUI_JINKE_PLUGIN )
    ADD_LIBRARY(fb2 SHARED ${CR3_SOURCES})
    TARGET_LINK_LIBRARIES(fb2 crengine tinydict ${EXTRA_LIBS} ${STD_LIBS})
    if (DEFINED CMAKE_STRIP)
       MESSAGE("Strip command: ${CMAKE_STRIP} ${CMAKE_BINARY_DIR}/cr3gui/libfb2.so" )
       EXECUTE_PROCESS( COMMAND ${CMAKE_STRIP} ${CMAKE_BINARY_DIR}/cr3gui/libfb2.so )
    endif (DEFINED CMAKE_STRIP)
    INSTALL( TARGETS fb2 LIBRARY DESTINATION ${CR3_BIN_DESTINATION} )
elseif (${GUI} STREQUAL CRGUI_PB)
    ADD_EXECUTABLE(cr3-pb.app ${CR3_SOURCES})
    TARGET_LINK_LIBRARIES(cr3-pb.app crengine tinydict ${EXTRA_LIBS} ${STD_LIBS})
    if (DEFINED CMAKE_STRIP)
       MESSAGE("Strip command:  ${CMAKE_STRIP} ${CMAKE_BINARY_DIR}/cr3gui/cr3-pb.app" )
       EXECUTE_PROCESS( COMMAND ${CMAKE_STRIP} ${CMAKE_BINARY_DIR}/cr3gui/cr3-pb.app )
    endif (DEFINED CMAKE_STRIP)
    INSTALL( TARGETS cr3-pb.app RUNTIME DESTINATION ${CR3_BIN_DESTINATION} )
else()
    ADD_EXECUTABLE(cr3 ${CR3_SOURCES})
    TARGET_LINK_LIBRARIES(cr3 crengine tinydict ${EXTRA_LIBS} ${STD_LIBS})
    if (DEFINED CMAKE_STRIP)
       MESSAGE("Strip command:  ${CMAKE_STRIP} ${CMAKE_BINARY_DIR}/cr3gui/cr3" )
       EXECUTE_PROCESS( COMMAND ${CMAKE_STRIP} ${CMAKE_BINARY_DIR}/cr3gui/cr3 )
    endif (DEFINED CMAKE_STRIP)
    INSTALL( TARGETS cr3 RUNTIME DESTINATION ${CR3_BIN_DESTINATION} )
endif ( ${GUI} STREQUAL CRGUI_JINKE_PLUGIN )

if (${GUI} STREQUAL CRGUI_XCB)
    SET(LOGOCNV_SOURCES src/logoconv.cpp)
    ADD_EXECUTABLE(logocnv ${LOGOCNV_SOURCES})
    TARGET_LINK_LIBRARIES(logocnv crengine ${STD_LIBS} ${EXTRA_LIBS})
endif (${GUI} STREQUAL CRGUI_XCB)


