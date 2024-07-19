if (TARGET tinyusb_device)

	add_compile_options(-Wall
	        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
	        -Wno-unused-function # we have some for the docs that aren't called
	        -Wno-maybe-uninitialized
	        )

    add_executable(pico_async_uart
            sample_main.c
            async_uart/Uart.c
            async_uart/Timer.c
            async_uart/Common.c
            )

	target_include_directories(pico_async_uart PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )

    # pull in common dependencies
    target_link_libraries(pico_async_uart 
			pico_stdlib 
			pico_multicore
			)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(pico_async_uart 1)
    pico_enable_stdio_uart(pico_async_uart 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(pico_async_uart)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building pico_async_uart because TinyUSB submodule is not initialized in the SDK")
endif()
