#include <gtest/gtest.h>

extern "C" {
#include <ember/core/models/object/base.h>
#include <ember/core/models/object/str.h>
#include <ember/core/models/value/base.h>
}

namespace {

EmberValue make_boolean(bool value) {
    struct EmberValue ember_value;
    ember_value.kind = EVK_BOOLEAN;
    ember_value.as.boolean_val = value;
    return ember_value;
}

EmberValue make_float(double value) {
    struct EmberValue ember_value;
    ember_value.kind = EVK_FLOAT;
    ember_value.as.float_val = value;
    return ember_value;
}

EmberValue make_int(int64_t value) {
    struct EmberValue ember_value;
    ember_value.kind = EVK_INT;
    ember_value.as.int_val = value;
    return ember_value;
}

EmberValue make_none() {
    struct EmberValue ember_value;
    ember_value.kind = EVK_NONE;
    return ember_value;
}

EmberValue make_obj(struct EmberObject *value) {
    struct EmberValue ember_value;
    ember_value.kind = EVK_OBJ;
    ember_value.as.obj_val = value;
    return ember_value;
}

}

TEST(EmberValueTest, ConstructorsPreserveKindAndPayload) {
    struct EmberValue boolean_value = make_boolean(true);
    struct EmberValue float_value = make_float(3.5);
    struct EmberValue int_value = make_int(-42);
    struct EmberValue none_value = make_none();

    EXPECT_TRUE(EV_IS_BOOLEAN(boolean_value));
    EXPECT_TRUE(EV_AS_BOOLEAN(boolean_value));

    EXPECT_TRUE(EV_IS_FLOAT(float_value));
    EXPECT_DOUBLE_EQ(EV_AS_FLOAT(float_value), 3.5);

    EXPECT_TRUE(EV_IS_INT(int_value));
    EXPECT_EQ(EV_AS_INT(int_value), -42);

    EXPECT_TRUE(EV_IS_NONE(none_value));
}

TEST(EmberValueTest, TruthinessMatchesValueKindSemantics) {
    EXPECT_FALSE(ember_value_is_truthy(make_boolean(false)));
    EXPECT_TRUE(ember_value_is_truthy(make_boolean(true)));

    EXPECT_FALSE(ember_value_is_truthy(make_float(0.0)));
    EXPECT_TRUE(ember_value_is_truthy(make_float(-0.25)));

    EXPECT_FALSE(ember_value_is_truthy(make_int(0)));
    EXPECT_TRUE(ember_value_is_truthy(make_int(7)));

    EXPECT_FALSE(ember_value_is_truthy(make_none()));

    struct EmberStringObject *string = ember_string_object_new("ember");
    ASSERT_NE(string, nullptr);
    EXPECT_TRUE(ember_value_is_truthy(make_obj((struct EmberObject *)string)));
    ember_object_destroy((struct EmberObject *)string);
}

TEST(EmberValueTest, EqualityRequiresMatchingKindsAndPayloads) {
    EXPECT_TRUE(ember_value_eq(make_boolean(true), make_boolean(true)));
    EXPECT_TRUE(ember_value_eq(make_float(1.5), make_float(1.5)));
    EXPECT_TRUE(ember_value_eq(make_int(12), make_int(12)));
    EXPECT_TRUE(ember_value_eq(make_none(), make_none()));

    EXPECT_FALSE(ember_value_eq(make_int(12), make_float(12.0)));
    EXPECT_FALSE(ember_value_eq(make_boolean(false), make_boolean(true)));
    EXPECT_FALSE(ember_value_eq(make_float(1.5), make_float(2.5)));
}

TEST(EmberValueTest, CompareSupportsIntegersAndFloats) {
    EXPECT_LT(ember_value_compare(make_int(2), make_int(5)), 0);
    EXPECT_GT(ember_value_compare(make_int(9), make_int(5)), 0);
    EXPECT_EQ(ember_value_compare(make_int(3), make_int(3)), 0);

    EXPECT_LT(ember_value_compare(make_float(-2.0), make_float(1.0)), 0);
    EXPECT_GT(ember_value_compare(make_float(4.5), make_float(1.0)), 0);
    EXPECT_EQ(ember_value_compare(make_float(2.25), make_float(2.25)), 0);
}

TEST(EmberValueTest, PrintFormatsPrimitiveValues) {
    testing::internal::CaptureStdout();
    ember_value_print(make_boolean(true));
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "true");

    testing::internal::CaptureStdout();
    ember_value_print(make_float(3.5));
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "3.5");

    testing::internal::CaptureStdout();
    ember_value_print(make_int(-12));
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "-12");

    testing::internal::CaptureStdout();
    ember_value_print(make_none());
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "none");
}
