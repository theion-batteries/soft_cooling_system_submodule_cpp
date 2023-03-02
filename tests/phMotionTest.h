#include <gtest/gtest.h>

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


TEST_F(phMotionTest, get_linear_speed) {
    EXPECT_EQ(800, motionObj->get_linear_speed());
}
TEST_F(phMotionTest, set_linear_speed) {
    EXPECT_EQ(sub_success, motionObj->set_Xspeed(100));
}
TEST_F(phMotionTest, get_linear_position) {
    EXPECT_EQ(-100, motionObj->get_linear_position());
}
TEST_F(phMotionTest, get_rotation_speed) {
    EXPECT_EQ(800, motionObj->get_rotation_speed());
}
TEST_F(phMotionTest, set_rotation_speed) {
    EXPECT_EQ(sub_success, motionObj->set_Yspeed(100));
}
TEST_F(phMotionTest, get_rotation_position) {
    EXPECT_EQ(-100, motionObj->get_rotation_position());
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


