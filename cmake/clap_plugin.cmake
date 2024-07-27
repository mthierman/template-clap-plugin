function(add_plugin)
    set(args
        ID
        NAME
        VENDOR
        URL
        MANUAL_URL
        SUPPORT_URL
        VERSION
        DESCRIPTION
        )
    set(listArgs SOURCES)
    cmake_parse_arguments(
        PLUGIN
        ""
        "${args}"
        "${listArgs}"
        ${ARGN}
        )

    message(STATUS ${PLUGIN_ID})
    message(STATUS ${PLUGIN_NAME})
    message(STATUS ${PLUGIN_VENDOR})
    message(STATUS ${PLUGIN_URL})
    message(STATUS ${PLUGIN_MANUAL_URL})
    message(STATUS ${PLUGIN_SUPPORT_URL})
    message(STATUS ${PLUGIN_VERSION})
    message(STATUS ${PLUGIN_DESCRIPTION})

    add_library(
        ${PLUGIN_NAME}
        MODULE
        )

    target_compile_definitions(
        ${PLUGIN_NAME}
        PRIVATE PLUGIN_ID="${PLUGIN_ID}"
                PLUGIN_NAME="${PLUGIN_NAME}"
                PLUGIN_VENDOR="${PLUGIN_VENDOR}"
                PLUGIN_URL="${PLUGIN_URL}"
                PLUGIN_MANUAL_URL="${PLUGIN_MANUAL_URL}"
                PLUGIN_SUPPORT_URL="${PLUGIN_SUPPORT_URL}"
                PLUGIN_VERSION="${PLUGIN_VERSION}"
                PLUGIN_DESCRIPTION="${PLUGIN_DESCRIPTION}"
        )

    target_sources(${PLUGIN_NAME} PRIVATE ${PLUGIN_SOURCES})

    if(CMAKE_SYSTEM_NAME
       STREQUAL
       "Windows"
        )
        set_target_properties(
            ${PLUGIN_NAME}
            PROPERTIES SUFFIX
                       ".clap"
            )
    endif()
endfunction()
