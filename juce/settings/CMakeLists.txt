# xpl_settings layer library [RQ-BLD-005, ADR-002]
add_library(xpl_settings STATIC
    src/LibraryInfo.cpp)
target_include_directories(xpl_settings PUBLIC include)
target_link_libraries(xpl_settings PUBLIC xpl_framework PRIVATE xpl::warnings)
