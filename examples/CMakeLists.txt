add_executable(write_dgCMatrix src/write_dgCMatrix.cpp)
target_link_libraries(write_dgCMatrix PRIVATE rds2cpp)
target_compile_options(write_dgCMatrix PRIVATE -Wall -Werror -Wpedantic -Wextra)

add_executable(peek_rds_file src/peek_rds_file.cpp)
target_link_libraries(peek_rds_file PRIVATE rds2cpp)
target_compile_options(peek_rds_file PRIVATE -Wall -Werror -Wpedantic -Wextra)

