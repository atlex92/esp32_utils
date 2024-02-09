file(GLOB_RECURSE SOURCES  src/*.cpp src/*.c)
set(srcs ${SOURCES})

set(includedirs
inc
)

set(requires
    cpp_utils
    esp_http_client
    mbedtls
    esp-tls
    nvs_flash
    esp_http_server
    spiffs
    driver
    spi_flash
    esp_hw_support
    esp_timer
    freertos_utils
)

function(maybe_add_component component_name)
    idf_build_get_property(components BUILD_COMPONENTS)
    if (${component_name} IN_LIST components)
        idf_component_get_property(lib_name ${component_name} COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${lib_name})
    endif()
endfunction()

idf_component_register(
    INCLUDE_DIRS ${includedirs}
    SRCS ${srcs}
    REQUIRES ${requires}
)

if(IDF_TARGET MATCHES "esp32" AND CONFIG_ENABLE_ARDUINO_SPIFFS_DRIVER)
    maybe_add_component(arduino)
endif()