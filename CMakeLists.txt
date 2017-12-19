cmake_minimum_required(VERSION 2.8) 

PROJECT (Server)

set( MAIN_SRC
	src/main.cpp
)

set ( SERVER_SRC
	src/server/ListSensors.cpp
	src/server/ResponseAlarm.cpp
	src/server/RequestAlarm.cpp
	src/server/ResponseSensors.cpp
	src/server/ServerAlarm.cpp
	src/server/ServerSensors.cpp
)

set ( SERVER_INCLUDE
	src/server/ExceptionServer.h
	src/server/ListSensors.h
	src/server/RequestAlarm.h
	src/server/ResponseAlarm.h
	src/server/ResponseSensors.h
	src/server/ServerAlarm.h
	src/server/ServerSensors.h
)

set( UTILS_SRC
	src/utils/communication/ResponsePacket.cpp
	src/utils/logger/Logger.cpp
	src/utils/network/ClientTCP.cpp
	src/utils/network/ServerTCP.cpp
	src/utils/network/TcpComm.cpp
	src/utils/network/UdpComm.cpp
	src/utils/platform/SystemCall.cpp
	src/utils/security/Cryptography.cpp
)

set( UTILS_INCLUDE
	src/utils/communication/ExceptionCommunication.h
	src/utils/communication/PacketComm.h
	src/utils/communication/ResponsePacket.h
	src/utils/communication/ResponsePacketClient.h
	src/utils/communication/ResponsePacketServer.h
	src/utils/concurrency/Concurrency.h
	src/utils/concurrency/ExceptionConcurrency.h
	src/utils/concurrency/Runnable.h
	src/utils/logger/ExceptionLogger.h
	src/utils/logger/Logger.h
	src/utils/network/ClientTCP.h
	src/utils/network/ExceptionNetwork.h
	src/utils/network/ServerTCP.h
	src/utils/network/TcpComm.h
	src/utils/network/UdpComm.h
	src/utils/network/ExceptionNetwork.h
	src/utils/platform/ExceptionPlatform.h
	src/utils/platform/SystemCall.h
	src/utils/security/Cryptography.h
	src/utils/security/ExceptionSecurity.h
	src/utils/ClientID.h
	src/utils/Exception.h
	src/utils/Queue.h
	src/utils/util.h
)

if("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/x64)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/x64)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/x64)
else("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/x86)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/x86)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/x86)
endif("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release")

find_package (Threads REQUIRED)

add_library( LibUtils ${UTILS_SRC} ${UTILS_INCLUDE} )
if(WIN32)
	# Include winmm for timeGetTime function, wsock32 and ws2_32 for socket windows.
	target_link_libraries( LibUtils wsock32 ws2_32 winmm )
else()
	target_link_libraries( LibUtils )
endif()


add_library( LibServer ${SERVER_SRC} ${SERVER_INCLUDE} ${UTILS_INCLUDE} )
if(WIN32)
	# Include winmm for timeGetTime function, wsock32 and ws2_32 for socket windows.
	target_link_libraries( LibServer wsock32 ws2_32 winmm )
else()
	target_link_libraries( LibServer LibUtils )
endif()

add_executable ( ServerTerremoto ${MAIN_SRC} ${SERVER_INCLUDE} ${UTILS_INCLUDE} )
target_link_libraries( ServerTerremoto LibUtils LibServer )
