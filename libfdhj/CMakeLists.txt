project (fdhj C)

aux_source_directory (src SRC_FILES)
include_directories (inc)
include_directories (src)
include_directories (${fdhijack_SOURCE_DIR}/inc)

add_library (fdhj STATIC ${SRC_FILES})
target_link_libraries(fdhj ceh)

target_include_directories (fdhj PUBLIC inc)
