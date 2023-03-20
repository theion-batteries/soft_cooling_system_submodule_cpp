#include <gtest/gtest.h>
#include <gtest/gtest_prod.h>

#include "ph_xy_motion.h"

class phMotionTest : public ::testing::Test {
protected:
    ph_xy_motion* motionObj ;

    void SetUp() override {
       motionObj = new ph_xy_motion("127.0.0.1", 8882,10);
       motionObj->connect();
    }

    void TearDown() override {
       motionObj->disconnect();
        delete motionObj;
    }
private:
};

TEST_F(phMotionTest, Connect) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, motionObj->connect());
}
TEST_F(phMotionTest, Disconnect) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, motionObj->disconnect());
}
TEST_F(phMotionTest, IsConnected) {
    EXPECT_TRUE(motionObj->getStatus());
}

TEST_F(phMotionTest, MoveHome) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, motionObj->move_home());
}


TEST_F(phMotionTest, GetLinearSpeed) {
    EXPECT_EQ(800, motionObj->get_linear_speed());
}
TEST_F(phMotionTest, SetLinearSpeed) {
    EXPECT_EQ(sub_success, motionObj->set_Xspeed(100));
}
TEST_F(phMotionTest, GetLinearPosition) {
    EXPECT_EQ(-100, motionObj->get_linear_position());
}
TEST_F(phMotionTest, GetRotationSpeed) {
    EXPECT_DOUBLE_EQ(500.000, motionObj->get_rotation_speed());
}
TEST_F(phMotionTest, SetRotationSpeed) {
    EXPECT_EQ(sub_success, motionObj->set_Yspeed(100));
}
TEST_F(phMotionTest, GetRotationPosition) {
    EXPECT_DOUBLE_EQ(50, motionObj->get_rotation_position());
}

TEST_F(phMotionTest, RotatateTo) {
    EXPECT_EQ(sub_success, motionObj->rotate_to(90));
        EXPECT_EQ(sub_success, motionObj->rotate_to(180));
}

TEST_F(phMotionTest, MoveCenter) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, motionObj->move_center());
}
TEST_F(phMotionTest, MoveDownTo) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, motionObj->move_down_to(100));
}
TEST_F(phMotionTest, MoveUpTo) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, motionObj->move_up_to(100));
}


