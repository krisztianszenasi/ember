#include <gtest/gtest.h>

extern "C" {
#include <ember/core/models/object/base.h>
#include <ember/core/models/object/str.h>
#include <ember/core/models/value/array.h>
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

TEST(EmberValueArrayTest, InitStartsEmpty) {
    struct EmberValueArray array;

    ember_value_array_init(&array);

    EXPECT_EQ(array.count, 0u);
    EXPECT_EQ(array.capacity, 0u);
    EXPECT_EQ(array.values, nullptr);
}

TEST(EmberValueArrayTest, AddAppendsValuesInOrder) {
    struct EmberValueArray array;
    ember_value_array_init(&array);

    ember_value_array_add(&array, make_int(7));
    ember_value_array_add(&array, make_float(2.5));
    ember_value_array_add(&array, make_boolean(true));

    ASSERT_EQ(array.count, 3u);
    ASSERT_NE(array.values, nullptr);
    EXPECT_TRUE(EV_IS_INT(array.values[0]));
    EXPECT_EQ(EV_AS_INT(array.values[0]), 7);
    EXPECT_TRUE(EV_IS_FLOAT(array.values[1]));
    EXPECT_DOUBLE_EQ(EV_AS_FLOAT(array.values[1]), 2.5);
    EXPECT_TRUE(EV_IS_BOOLEAN(array.values[2]));
    EXPECT_TRUE(EV_AS_BOOLEAN(array.values[2]));

    ember_value_array_deinit(&array);
}

TEST(EmberValueArrayTest, AddGrowsCapacityWhenNeeded) {
    struct EmberValueArray array;
    ember_value_array_init(&array);

    ember_value_array_add(&array, make_int(1));
    size_t initial_capacity = array.capacity;

    while (array.count < initial_capacity + 1) {
        ember_value_array_add(&array, make_int((int64_t)array.count + 2));
    }

    EXPECT_GT(array.capacity, initial_capacity);
    EXPECT_EQ(array.count, initial_capacity + 1);

    ember_value_array_deinit(&array);
}

TEST(EmberValueArrayTest, FindReturnsFirstMatchingIndex) {
    struct EmberValueArray array;
    ember_value_array_init(&array);

    ember_value_array_add(&array, make_int(11));
    ember_value_array_add(&array, make_float(3.0));
    ember_value_array_add(&array, make_int(11));

    size_t index = 99;
    ASSERT_TRUE(ember_value_array_find(&array, make_int(11), &index));
    EXPECT_EQ(index, 0u);

    ASSERT_TRUE(ember_value_array_find(&array, make_float(3.0), &index));
    EXPECT_EQ(index, 1u);

    ember_value_array_deinit(&array);
}

TEST(EmberValueArrayTest, FindDoesNotOverwriteIndexWhenValueIsMissing) {
    struct EmberValueArray array;
    ember_value_array_init(&array);

    ember_value_array_add(&array, make_int(5));

    size_t index = 17;
    EXPECT_FALSE(ember_value_array_find(&array, make_float(5.0), &index));
    EXPECT_EQ(index, 17u);
    EXPECT_FALSE(ember_value_array_find(&array, make_none(), nullptr));

    ember_value_array_deinit(&array);
}

TEST(EmberValueArrayTest, DeinitResetsArrayAndReleasesOwnedValues) {
    struct EmberValueArray array;
    ember_value_array_init(&array);

    struct EmberStringObject *string = ember_string_object_new("ember");
    ASSERT_NE(string, nullptr);
    ember_value_array_add(&array, make_obj((struct EmberObject *)string));
    ember_value_array_add(&array, make_int(42));

    ASSERT_EQ(array.count, 2u);

    ember_value_array_deinit(&array);

    EXPECT_EQ(array.count, 0u);
    EXPECT_EQ(array.capacity, 0u);
    EXPECT_EQ(array.values, nullptr);
}
