
if(EXISTS ${COPY_SRC})
    message("Importing module ${COPY_SRC}")
    file(COPY ${COPY_SRC} DESTINATION ${COPY_DST})
endif()
