
file(GLOB_RECURSE CPP_SOURCES *.cpp)
file(GLOB_RECURSE C_SOURCES *.c)
file(GLOB_RECURSE QRC_SOURCES *.qrc)
file(GLOB_RECURSE UI_SOURCES *.ui)


add_library(JsonEditor
	${CPP_SOURCES}
	${C_SOURCES}
	${QRC_SOURCES}
	${UI_SOURCES}
)

target_compile_definitions(JsonEditor PUBLIC USE_JSON_EDITOR)
target_include_directories(JsonEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(JsonEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dialogs)
target_link_libraries(JsonEditor PUBLIC Qt5::Widgets)
target_compile_features(JsonEditor PUBLIC ${STUDIO_CXX_STD})