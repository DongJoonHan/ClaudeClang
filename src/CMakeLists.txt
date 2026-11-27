cc_add_library(ecl_core
    SOURCES ecl/input_validation.c ecl/command_intake.c ecl/status_eval.c
            ecl/rule_engine.c ecl/controller.c
    PUBLIC_INCLUDE ${PROJECT_SOURCE_DIR}/include)
