# CMakeList.txt : CMake project for App, include source and define
# project specific logic here.
#
#project("App" VERSION 1.0)

set(APP_NAME "App")

set (APP_PUBLIC_SRC "Source/Public/${APP_NAME}/")
set (APP_PRIVTE_SRC "Source/Private/${APP_NAME}/")
set (APP_CMAKE_SRC "CMake_Source/Public/Gen_${APP_NAME}/")

# List of included CMake sources
set(APP_SOURCES_CMake
  ${APP_CMAKE_SRC}Config/${APP_NAME}Config.h.in
)

# List of included sources
set(APP_SOURCES_CPP
  ${APP_PRIVTE_SRC}main.cpp
  ${APP_PUBLIC_SRC}Defines/pch.h
  ${APP_PUBLIC_SRC}Defines/BBSFML.h		                ${APP_PRIVTE_SRC}Defines/BBSFML.cpp
  ${APP_PUBLIC_SRC}Defines/BBGameBoardDef.h				${APP_PRIVTE_SRC}Defines/BBGameBoardDef.cpp
  ${APP_PUBLIC_SRC}Editor/BBGameBoardEditor.h			${APP_PRIVTE_SRC}Editor/BBGameBoardEditor.cpp
  ${APP_PUBLIC_SRC}Editor/BBDrawableGrid.h				${APP_PRIVTE_SRC}Editor/BBDrawableGrid.cpp
  ${APP_PUBLIC_SRC}Memory/IBBProgramObject.h			${APP_PRIVTE_SRC}Memory/IBBProgramObject.cpp
  ${APP_PUBLIC_SRC}Memory/IBBSceneObject.h				${APP_PRIVTE_SRC}Memory/IBBSceneObject.cpp
  ${APP_PUBLIC_SRC}GameSystem/BBProgramInstance.h		${APP_PRIVTE_SRC}GameSystem/BBProgramInstance.cpp
  ${APP_PUBLIC_SRC}GameSystem/BBSoundPlayer.h			${APP_PRIVTE_SRC}GameSystem/BBSoundPlayer.cpp
  ${APP_PUBLIC_SRC}GameSystem/BBMusicPlayer.h			${APP_PRIVTE_SRC}GameSystem/BBMusicPlayer.cpp
  ${APP_PUBLIC_SRC}GameSystem/BBMusicPlayerData.h		${APP_PRIVTE_SRC}GameSystem/BBMusicPlayerData.cpp
  ${APP_PUBLIC_SRC}GameSystem/BBSoundPlayerData.h		${APP_PRIVTE_SRC}GameSystem/BBSoundPlayerData.cpp
  ${APP_PUBLIC_SRC}GameSystem/BBScene.h					${APP_PRIVTE_SRC}GameSystem/BBScene.cpp
  ${APP_PUBLIC_SRC}GameSystem/BBPaths.h					${APP_PRIVTE_SRC}GameSystem/BBPaths.cpp
  ${APP_PUBLIC_SRC}GameSystem/BBTextureLibrary.h		${APP_PRIVTE_SRC}GameSystem/BBTextureLibrary.cpp
  ${APP_PUBLIC_SRC}Game/Kajamight/BBKajamightScene.h	${APP_PRIVTE_SRC}Game/Kajamight/BBKajamightScene.cpp
  ${APP_PUBLIC_SRC}Game/Kajamight/BBKajamightUI.h		${APP_PRIVTE_SRC}Game/Kajamight/BBKajamightUI.cpp
  ${APP_PUBLIC_SRC}Game/BBGameBoardScene.h				${APP_PRIVTE_SRC}Game/BBGameBoardScene.cpp
  ${APP_PUBLIC_SRC}Game/BBGameBoard.h					${APP_PRIVTE_SRC}Game/BBGameBoard.cpp
  ${APP_PUBLIC_SRC}Game/BBGameBoard_Debug.h				${APP_PRIVTE_SRC}Game/BBGameBoard_Debug.cpp
  ${APP_PUBLIC_SRC}Game/BBGameUI.h						${APP_PRIVTE_SRC}Game/BBGameUI.cpp
  ${APP_PUBLIC_SRC}Game/BBGameProgress.h				${APP_PRIVTE_SRC}Game/BBGameProgress.cpp
  ${APP_PUBLIC_SRC}Game/BBBall.h						${APP_PRIVTE_SRC}Game/BBBall.cpp
  ${APP_PUBLIC_SRC}Game/BBBorders.h						${APP_PRIVTE_SRC}Game/BBBorders.cpp
  ${APP_PUBLIC_SRC}Game/BBBricks.h						${APP_PRIVTE_SRC}Game/BBBricks.cpp
  ${APP_PUBLIC_SRC}Game/BBBrickData.h					${APP_PRIVTE_SRC}Game/BBBrickData.cpp
  ${APP_PUBLIC_SRC}Game/BBBricksDrawable.h				${APP_PRIVTE_SRC}Game/BBBricksDrawable.cpp
  ${APP_PUBLIC_SRC}Game/BBPaddle.h						${APP_PRIVTE_SRC}Game/BBPaddle.cpp
  ${APP_PUBLIC_SRC}Game/BBUserInputs.h					${APP_PRIVTE_SRC}Game/BBUserInputs.cpp
  ${APP_PUBLIC_SRC}Game/IBBGameBoardObject.h			${APP_PRIVTE_SRC}Game/IBBGameBoardObject.cpp
  ${APP_PUBLIC_SRC}Game/Level/BBLevel0.h				${APP_PRIVTE_SRC}Game/Level/BBLevel0.cpp
  ${APP_PUBLIC_SRC}Game/Level/BBLevelJsonReader.h		${APP_PRIVTE_SRC}Game/Level/BBLevelJsonReader.cpp
  
  ${APP_PUBLIC_SRC}Game/Physics/BBDynamics.h			${APP_PRIVTE_SRC}Game/Physics/BBDynamics.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBColBehavior.h			${APP_PRIVTE_SRC}Game/Physics/BBColBehavior.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBCollisionType.h		${APP_PRIVTE_SRC}Game/Physics/BBCollisionType.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBCollider.h			${APP_PRIVTE_SRC}Game/Physics/BBCollider.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBCollidersBody.h		${APP_PRIVTE_SRC}Game/Physics/BBCollidersBody.cpp
  ${APP_PUBLIC_SRC}Game/Physics/IBBProjectile.h			${APP_PRIVTE_SRC}Game/Physics/IBBProjectile.cpp
  ${APP_PUBLIC_SRC}Game/Physics/IBBProjectileSolver.h	${APP_PRIVTE_SRC}Game/Physics/IBBProjectileSolver.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBSolver.h				${APP_PRIVTE_SRC}Game/Physics/BBSolver.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBCollisionResult.h		${APP_PRIVTE_SRC}Game/Physics/BBCollisionResult.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBPhysicContext.h		${APP_PRIVTE_SRC}Game/Physics/BBPhysicContext.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBPhysicBody.h			${APP_PRIVTE_SRC}Game/Physics/BBPhysicBody.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBPhysicBodyID.h
  ${APP_PUBLIC_SRC}Game/Physics/IBBPhysicBodyListener.h
  ${APP_PUBLIC_SRC}Game/Physics/BBPhysicManager.h		${APP_PRIVTE_SRC}Game/Physics/BBPhysicManager.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBPhysicManager_Dynamics.hpp
  ${APP_PUBLIC_SRC}Game/Physics/BBPhysicManager_Collisions.hpp
  ${APP_PUBLIC_SRC}Game/Physics/Solvers/BBImpulseSolver.h	${APP_PRIVTE_SRC}Game/Physics/Solvers/BBImpulseSolver.cpp
  ${APP_PUBLIC_SRC}Game/Physics/Solvers/BBPositionSolver.h	${APP_PRIVTE_SRC}Game/Physics/Solvers/BBPositionSolver.cpp
  ${APP_PUBLIC_SRC}Debugs/Physics/BBDebugSAT.h			${APP_PRIVTE_SRC}Debugs/Physics/BBDebugSAT.cpp
  ${APP_PUBLIC_SRC}Game/Physics/BBCollisionFuncs.hpp
  ${APP_PUBLIC_SRC}Game/Physics/BBOverlapFuncs.hpp
  ${APP_PUBLIC_SRC}Game/Physics/BBBallProjectileSolver.h	${APP_PRIVTE_SRC}Game/Physics/BBBallProjectileSolver.cpp
)


create_non_existing("${APP_SOURCES_CPP}" "${CMAKE_CURRENT_SOURCE_DIR}" ${APP_NAME})


# Add source to this project's executable.
add_executable ( ${APP_NAME} ${APP_SOURCES_CMake} ${APP_SOURCES_CPP} )

target_precompile_headers(${APP_NAME}
  PRIVATE
  ${APP_PUBLIC_SRC}Defines/pch.h
)

# 'source_group' Ensure than Visual Studio Solution respect File Hierarchy in filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${APP_SOURCES_CPP})

configure_file("CMake_Source/Public/Gen_App/Config/AppConfig.h.in" "Gen_SRC/Public/Gen_App/Config/AppConfig.h")


#add_compile_definitions("SOME_DEFINITION" 0)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 20)
endif()

if (USE_WIN32)
  set_property(TARGET ${APP_NAME} PROPERTY WIN32_EXECUTABLE $<CONFIG:Release>)
endif()

#Load GameInput Library from GameInput external library
#get_property(GAMEINPUT_LIB_PATH GLOBAL PROPERTY GAMEINPUT_LIB_PATH)
#get_property(GAMEINPUT_INC_PATH GLOBAL PROPERTY GAMEINPUT_INC_PATH)
#get_property(GAMEINPUT_LIB GLOBAL PROPERTY GAMEINPUT_LIB)


get_property(TRACY_INC_PATH GLOBAL PROPERTY TRACY_INC_PATH)
#get_property(ASSIMP_INC_PATH GLOBAL PROPERTY ASSIMP_INC_PATH)


target_link_libraries(${APP_NAME} PUBLIC "RCB" "RSFML" sfml-system sfml-window sfml-graphics sfml-audio Tracy::TracyClient nlohmann_json::nlohmann_json)

if (USE_WIN32)
  target_link_libraries(${APP_NAME} PUBLIC sfml-main )
endif()


target_include_directories(${APP_NAME} 
 PUBLIC
  "${PROJECT_BINARY_DIR}/${APP_NAME}/Gen_SRC/Public/"
  "${CMAKE_CURRENT_SOURCE_DIR}/Source/Public/"
  ${TRACY_INC_PATH}
 PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Source/Private/"
)

# Specify the directory containing the assets
set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/App/Assets")

# Set the runtime output directory based on configuration 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>) 
# Add a custom command to copy the assets directory 
add_custom_command( 
  TARGET ${APP_NAME} POST_BUILD 
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
  ${ASSETS_SOURCE_DIR} 
  $<TARGET_FILE_DIR:${APP_NAME}>/Assets 
)