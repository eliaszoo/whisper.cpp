if (WHISPER_SDL2)
    # stream
    set(TARGET zego-stream2)
    add_executable(${TARGET} stream.cpp)

    include(DefaultTargetOptions)

    target_link_libraries(${TARGET} PRIVATE common whisper ${CMAKE_THREAD_LIBS_INIT})
endif ()
