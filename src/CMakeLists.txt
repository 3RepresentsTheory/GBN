# the socket library target

file(GLOB GBNLIB_SOURCES "*.cpp")
file(GLOB GBNLIB_HEADERS "*.h")
add_library(GBNSocketLibrary ${GBNLIB_SOURCES} ${GBNLIB_HEADERS})
target_link_libraries(GBNSocketLibrary  nlohmann_json::nlohmann_json)
