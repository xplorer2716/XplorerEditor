# xpl_framework layer library [RQ-BLD-005, ADR-002]
add_library(xpl_framework STATIC
    src/LibraryInfo.cpp)
target_include_directories(xpl_framework PUBLIC include)
target_link_libraries(xpl_framework PUBLIC xpl_midi PRIVATE xpl::warnings)
