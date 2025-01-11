
set(RSFML_NAME "RSFML")

set (RSFML_PUBLIC_SRC "Source/Public/${RSFML_NAME}/")
set (RSFML_PRIVTE_SRC "Source/Private/${RSFML_NAME}/")


# Header file are not compiled into object (or rather are just copied into objects on include), so their inclusion into the project is optional
# Only their 'target_include_directories' is needed to be usable as #include
# List of included sources
set(RSFML_SOURCES
	${RSFML_PUBLIC_SRC}Defines/RSFML.h						${RSFML_PRIVTE_SRC}Defines/RSFML.cpp
	${RSFML_PUBLIC_SRC}Tools/Textures/RGDXTPAtlasData.h		${RSFML_PRIVTE_SRC}Tools/Textures/RGDXTPAtlasData.cpp
	${RSFML_PUBLIC_SRC}Tools/Textures/RGDXTPAtlasReader.h	${RSFML_PRIVTE_SRC}Tools/Textures/RGDXTPAtlasReader.cpp
	${RSFML_PUBLIC_SRC}Tools/Textures/RGDXTPSprite.h		${RSFML_PRIVTE_SRC}Tools/Textures/RGDXTPSprite.cpp
	${RSFML_PUBLIC_SRC}Tools/RCollection.h					${RSFML_PRIVTE_SRC}Tools/RCollection.cpp
	${RSFML_PUBLIC_SRC}Tools/REnum.h						${RSFML_PRIVTE_SRC}Tools/REnum.cpp
	${RSFML_PUBLIC_SRC}Tools/RMath.h						${RSFML_PRIVTE_SRC}Tools/RMath.cpp
	${RSFML_PUBLIC_SRC}Tools/RLog.h							${RSFML_PRIVTE_SRC}Tools/RLog.cpp
	${RSFML_PUBLIC_SRC}Graphics/RVertArrHelper.h			${RSFML_PRIVTE_SRC}Graphics/RVertArrHelper.cpp
	${RSFML_PUBLIC_SRC}Graphics/R9Slice.h					${RSFML_PRIVTE_SRC}Graphics/R9Slice.cpp
	${RSFML_PUBLIC_SRC}Graphics/RRepeatable.h				${RSFML_PRIVTE_SRC}Graphics/RRepeatable.cpp
	${RSFML_PUBLIC_SRC}Graphics/RSimpleSprite.h				${RSFML_PRIVTE_SRC}Graphics/RSimpleSprite.cpp

)

create_non_existing("${RSFML_SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}" ${RSFML_NAME})

add_library (${RSFML_NAME} STATIC ${RSFML_SOURCES})

# 'source_group' Ensure than Visual Studio Solution respect File Hierarchy in filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RSFML_SOURCES})


# No differenciation between 'install(EXPORT)' and 'export()' for now
# (never been tested outside static include case)

target_link_libraries(${RSFML_NAME} PUBLIC "RCB" sfml-graphics  sfml-audio)

target_include_directories(${RSFML_NAME}
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/Public/"
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}"/Source/Private/"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${RSFML_NAME} PROPERTY CXX_STANDARD 20)
endif()

#target_link_libraries(${RSFML_NAME} PUBLIC "d3d12.lib" "dxgi.lib" "dxguid.lib")