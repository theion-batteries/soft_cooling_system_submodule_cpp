#include <gtest/gtest.h>

#include "ph_cooling_controller.h"

class meteorAdapterTest : public ::testing::Test {
protected:
    meteorAdapter* meteorObj ;
    cooling_config_yaml_params _ph_params;

    void SetUp() override {
       meteorObj = new meteorAdapter(_ph_params);
       meteorObj->connect();
    }

    void TearDown() override {
       meteorObj->disconnect();
        delete meteorObj;
    }
};

TEST_F(meteorAdapterTest, Connect) {
    EXPECT_EQ(hw_success, meteorObj->connect());
}

TEST_F(meteorAdapterTest, Disconnect) {
    EXPECT_EQ(hw_success, meteorObj->disconnect());
}

TEST_F(meteorAdapterTest, IsConnected) {
    EXPECT_TRUE(meteorObj->getStatus());
}

TEST_F(meteorAdapterTest, turnOnPh) {
    EXPECT_EQ(hw_success, meteorObj->turnOnPh());
}

TEST_F(meteorAdapterTest, turnOffPh) {
    EXPECT_EQ(hw_success, meteorObj->turnOffPh());
}
TEST_F(meteorAdapterTest, startPrinting) {
    EXPECT_EQ(hw_success, meteorObj->startPrinting());
}
TEST_F(meteorAdapterTest, endPrinting) {
    EXPECT_EQ(hw_success, meteorObj->endPrinting());
}


