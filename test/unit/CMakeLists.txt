cc_add_test_suite(unit
    SOURCES input_validation_test.cpp command_intake_test.cpp status_eval_test.cpp
            rule_engine_test.cpp controller_test.cpp
    LIBS ecl_core)
