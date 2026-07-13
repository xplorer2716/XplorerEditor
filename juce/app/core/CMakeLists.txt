# xpl_app_core: UI-framework-free application logic (control table, binding
# registry, page-family logic) — headless-testable. [ADR-006]
add_library(xpl_app_core STATIC
    src/ControlTable.cpp
    src/ParameterBindingRegistry.cpp
    src/ControlMetadata.cpp
    src/PageFamilyModel.cpp
    src/ModulationHighlight.cpp
    src/MidiAutomationTable.cpp)
target_include_directories(xpl_app_core PUBLIC include)
target_link_libraries(xpl_app_core PUBLIC xpl_controller PRIVATE xpl::warnings)
