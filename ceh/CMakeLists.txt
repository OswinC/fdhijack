project (ceh C)

aux_source_directory (src SRC_FILES)
include_directories (inc)
include_directories (src)

add_library (ceh STATIC ${SRC_FILES})

target_include_directories (ceh PUBLIC inc)
