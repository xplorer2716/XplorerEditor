# xpl_framework layer library: port of MidiApp.MidiController core [RQ-FMW, ADR-002]
add_library(xpl_framework STATIC
    src/LibraryInfo.cpp
    src/AbstractParameter.cpp
    src/OrderedParameterMap.cpp
    src/AbstractTone.cpp
    src/DualDictionary.cpp
    src/AbstractController.cpp
    src/AbstractControllerDevices.cpp
    src/AbstractControllerWorker.cpp
    src/Logger.cpp
    src/SingleInstanceGuard.cpp)
target_include_directories(xpl_framework PUBLIC include)
target_link_libraries(xpl_framework PUBLIC xpl_midi PRIVATE xpl::warnings)
