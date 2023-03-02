#include <gtest/gtest.h>

#include "ph_trigger.h"

class ph_triggerTest : public ::testing::Test {
protected:
    ph_trigger* phTrigger ;

    void SetUp() override {
       phTrigger = new ph_trigger("127.0.0.1", 8881,10);
       phTrigger->connect();
    }

    void TearDown() override {
       phTrigger->disconnect();
        delete phTrigger;
    }
};

TEST_F(ph_triggerTest, Connect) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phTrigger->connect());
}

TEST_F(ph_triggerTest, Disconnect) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phTrigger->disconnect());
}
TEST_F(ph_triggerTest, IsConnected) {
    EXPECT_TRUE(phTrigger->getStatus());
}


TEST_F(ph_triggerTest, turn_on) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phTrigger->turn_on());
}

TEST_F(ph_triggerTest, turn_off) {
    EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_success, phTrigger->turn_off());
}