# xpl_model layer library [RQ-BLD-005, ADR-002]
add_library(xpl_model STATIC
    src/LibraryInfo.cpp)
target_include_directories(xpl_model PUBLIC include)
target_link_libraries(xpl_model PUBLIC xpl_framework PRIVATE xpl::warnings)
