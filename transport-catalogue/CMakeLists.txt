cmake_minimum_required(VERSION 3.10)

project(TransportCatalogue CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto svg.proto map_renderer.proto transport_router.proto graph.proto)

set(DOMAIN domain.h domain.cpp)
set(GEO geo.h geo.cpp)
set(JSON json.h json.cpp json_builder.h json_builder.cpp json_reader.h json_reader.cpp)
set(MAP map_renderer.h map_renderer.cpp)
set(REQUEST request_handler.h request_handler.cpp)
set(SERIALIZATION serialization.h serialization.cpp)
set(SVG svg.h svg.cpp)
set(ROUTER router.h transport_router.h transport_router.cpp)
set(PROTO transport_catalogue.proto svg.proto map_renderer.proto transport_router.proto graph.proto)
set(TRANSPORT_CATALOQUE transport_catalogue.h transport_catalogue.cpp)
set(ALL_FILES main.cpp graph.h ranges.h ${DOMAIN} ${GEO} ${JSON} ${MAP} ${REQUEST} ${SERIALIZATION} ${SVG} ${ROUTER} ${PROTO} ${TRANSPORT_CATALOQUE})

add_executable(transport_catalogue ${PROTO_SRCS} ${PROTO_HDRS} ${ALL_FILES})
target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
#string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)