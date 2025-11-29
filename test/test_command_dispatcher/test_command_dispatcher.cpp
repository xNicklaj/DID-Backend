#include <Arduino.h>
#include <unity.h>

#include <CommandDispatcher.h>

static String capturedArgs;

void handler_capture(String args) {
    capturedArgs = args;
}

void setUp(void) {
    // runs before each test
    capturedArgs = "";
}

void tearDown(void) {
    // runs after each test
}

void test_command_with_params_dispatches(void) {
    CommandDispatcher disp;
    disp.registerCommand("*341#", handler_capture, 3);

    // Sequence contains command + 3 parameters at the end
    String seq = "XXX*341#012"; // extra leading chars to emulate rolling buffer
    bool res = disp.execute(seq, "");

    TEST_ASSERT_TRUE(res);
    TEST_ASSERT_EQUAL_STRING("012", capturedArgs.c_str());
}

void test_command_with_params_not_dispatches_when_missing_params(void) {
    CommandDispatcher disp;
    disp.registerCommand("*341#", handler_capture, 3);

    // Sequence is missing one parameter char
    String seq = "*341#01";
    bool res = disp.execute(seq, "");

    TEST_ASSERT_FALSE(res);
    TEST_ASSERT_EQUAL_STRING("", capturedArgs.c_str());
}

void test_command_no_params_dispatches_immediately(void) {
    CommandDispatcher disp;
    disp.registerCommand("*123#", handler_capture, 0);

    String seq = "*123#";
    bool res = disp.execute(seq, "");

    TEST_ASSERT_TRUE(res);
    TEST_ASSERT_EQUAL_STRING("", capturedArgs.c_str());
}

void setup() {
    delay(200);
    UNITY_BEGIN();
    RUN_TEST(test_command_with_params_dispatches);
    RUN_TEST(test_command_with_params_not_dispatches_when_missing_params);
    RUN_TEST(test_command_no_params_dispatches_immediately);
    UNITY_END();
}

void loop() {
    // empty - tests run once in setup
}
