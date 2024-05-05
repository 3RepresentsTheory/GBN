
# make the file transmission program

add_executable(
        GBNFileSender
        GBNFileSender.cpp
)

add_executable(
        GBNFileReceiver
        GBNFileReceiver.cpp
)


target_link_libraries(GBNFileSender   PRIVATE GBNSocketLibrary)
target_link_libraries(GBNFileReceiver PRIVATE GBNSocketLibrary)
