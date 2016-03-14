
# Generate module information file (one version for build time, one for install time)
# 
# This function only generates the module information file internally, to deploy
# it for actual use, the functions export_module_info() and install_module_info()
# should be used.
function(generate_module_info MODNAME)

    # Create build-time version
    set(file_build   "${PROJECT_BINARY_DIR}/ModuleInfo/${MODNAME}.modinfo.build")
    file(WRITE "${file_build}"   "")

    # Create install-time version
    set(file_install "${PROJECT_BINARY_DIR}/ModuleInfo/${MODNAME}.modinfo.install")
    file(WRITE "${file_install}" "")

    # Parse all remaining arguments
    set(mode 0)
    set(index 1)
    while(${index} LESS ${ARGC})
        set(arg ${ARGV${index}})
        math(EXPR index "${index} + 1" )

        # Switch mode
        if(arg STREQUAL "VALUES")
            set(mode 1)
        elseif(arg STREQUAL "BUILD_VALUES")
            set(mode 2)
        elseif(arg STREQUAL "INSTALL_VALUES")
            set(mode 3)
        else()
            # Get key and value
            set(key "${arg}")
            set(val ${ARGV${index}})
            math(EXPR index "${index} + 1" )

            # Write into build-time version for VALUES or BUILD_VALUES
            if(mode EQUAL 1 OR mode EQUAL 2)
                file(APPEND "${file_build}" "${key}: ${val}\n")
            endif()

            # Write into install-time version for VALUES or INSTALL_VALUES
            if(mode EQUAL 1 OR mode EQUAL 3)
                file(APPEND "${file_install}" "${key}: ${val}\n")
            endif()
        endif()

    endwhile()

endfunction(generate_module_info)


# Copy module information file to a specific location
function(copy_module_info MODNAME DEST)

    file(GENERATE
        OUTPUT "${DEST}"
        INPUT  "${PROJECT_BINARY_DIR}/ModuleInfo/${MODNAME}.modinfo.build"
    )

endfunction(copy_module_info MODNAME DEST)


# Put module information file alongside a target
function(export_module_info MODNAME)

    # Target name must be set
    
    # Workaround for cmake < 3.2 which misinterprets the "TARGET" value
    string(COMPARE EQUAL "${ARGV1}" "TARGET" compare_result)
    if(${compare_result})
        set(target "${ARGV2}")

        # Determine output name
        set(outname "${MODNAME}.modinfo")
        if("${ARGV5}" STREQUAL "RENAME")
            set(outname "${ARGV6}")
        endif()

        # Create a target that copies the module information file alongside the generated target
        add_custom_target("${target}-modinfo" ALL
            COMMAND ${CMAKE_COMMAND} -E copy
                    "${PROJECT_BINARY_DIR}/ModuleInfo/${MODNAME}.modinfo.build"
                    "$<TARGET_FILE_DIR:${target}>/${outname}"
            COMMENT "Exporting ${outname}"
            VERBATIM
        )

        # If specified, put target into IDE-folder
        if("${ARGV3}" STREQUAL "FOLDER")
            set_target_properties("${target}-modinfo"
                PROPERTIES
                FOLDER "${ARGV4}"
            )
        endif()
    endif()

endfunction(export_module_info)


# Install module information file
function(install_module_info MODNAME)

    # Set default output name
    set(outname "${MODNAME}.modinfo")

    # Parse all remaining arguments
    set(args)
    set(index 1)
    while(${index} LESS ${ARGC})
        # Get current argument
        set(arg ${ARGV${index}})
        math(EXPR index "${index} + 1" )

        if(arg STREQUAL "RENAME")
            # Modify output name, ignore argument and value in args
            set(outname "${ARGV${index}}")
            math(EXPR index "${index} + 1" )
        else()
            # Add argument to args
            list(APPEND args ${arg})
        endif()
    endwhile()

    # Install module information file into install-tree
    # manually creating install directory becuase file GENERATE doesn't do so before CMake 3.2
    file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/ModuleInfo/install")
    file(GENERATE
        OUTPUT "${PROJECT_BINARY_DIR}/ModuleInfo/install/${outname}"
        INPUT  "${PROJECT_BINARY_DIR}/ModuleInfo/${MODNAME}.modinfo.install"
    )
    install(FILES "${PROJECT_BINARY_DIR}/ModuleInfo/install/${outname}" ${args})

endfunction(install_module_info)
