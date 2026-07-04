# xpl_model layer library: Xpander tone, parameters, SysEx I/O [RQ-MOD, ADR-002]
add_library(xpl_model STATIC
    src/LibraryInfo.cpp
    src/XpanderParameter.cpp)
target_include_directories(xpl_model PUBLIC include)
target_link_libraries(xpl_model PUBLIC xpl_framework PRIVATE xpl::warnings)
