# xpl_controller layer library: XpanderController port [RQ-CTL, ADR-002]
add_library(xpl_controller STATIC
    src/LibraryInfo.cpp
    src/PageSubPageHelper.cpp
    src/XpanderController.cpp
    src/XpanderControllerMidiEvents.cpp)
target_include_directories(xpl_controller PUBLIC include)
target_link_libraries(xpl_controller PUBLIC xpl_model xpl_settings PRIVATE xpl::warnings)
