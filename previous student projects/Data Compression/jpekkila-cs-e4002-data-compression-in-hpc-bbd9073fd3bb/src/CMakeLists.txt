include_directories(.)
add_executable(compress main.c buffer.c morton.c solver.c grid.c compress.c benchmark.c analysis.c)
target_link_libraries(compress m zfp) # zfp nvcomp
