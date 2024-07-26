function(add_plugin)
    set(args NAME)
    set(listArgs SOURCES)

    cmake_parse_arguments(
        ADD_PLUGIN
        ""
        "${args}"
        "${listArgs}"
        ${ARGN}
        )

    add_library(
        ${ADD_PLUGIN_NAME}
        MODULE
        )

    target_sources(${ADD_PLUGIN_NAME} PRIVATE ${ADD_PLUGIN_SOURCES})

    if(CMAKE_SYSTEM_NAME
       STREQUAL
       "Windows"
        )
        set_target_properties(
            ${ADD_PLUGIN_NAME}
            PROPERTIES SUFFIX
                       ".clap"
            )
    endif()
endfunction()
