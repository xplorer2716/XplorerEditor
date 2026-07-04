# xpl_controller layer library [RQ-BLD-005, ADR-002]
add_library(xpl_controller STATIC
    src/LibraryInfo.cpp)
target_include_directories(xpl_controller PUBLIC include)
target_link_libraries(xpl_controller PUBLIC xpl_model PRIVATE xpl::warnings)
