#include <gtest/gtest.h>

#include "ph_cooling_controller.h"

class phControllerTest : public ::testing::Test {
protected:
    ph_cooling_controller phControl= ph_cooling_controller() ;

    void SetUp() override {
       phControl.ph_controller_connect();
    }

    void TearDown() override {
       phControl.ph_controller_disconnect();
    }
};

TEST_F(phControllerTest, Connect) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phControl.ph_controller_connect());
}
TEST_F(phControllerTest, Disconnect) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phControl.ph_controller_disconnect());
}
TEST_F(phControllerTest, get_ph_status) {
    EXPECT_TRUE( phControl.get_ph_status());
}

TEST_F(phControllerTest, ph_motion_move_home) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phControl.ph_motion_move_home());
}

TEST_F(phControllerTest, ph_motion_move_offset) {
//    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phControl.ph_motion_move_offset());
}
TEST_F(phControllerTest, ph_motion_home_all) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phControl.ph_motion_home_all());
}

TEST_F(phControllerTest, ph_motion_rotate_home) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phControl.ph_motion_rotate_home());
}
TEST_F(phControllerTest, ph_rotate_center) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phControl.ph_rotate_center());
}


TEST_F(phControllerTest, ph_move_center) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phControl.ph_move_center());
}
TEST_F(phControllerTest, ph_rotate_and_print) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phControl.ph_rotate_and_print());
}



