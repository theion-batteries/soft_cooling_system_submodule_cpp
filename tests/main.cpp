#include "phMotionTest.h"
#include "phTriggerTest.h"
#include "phControllerTest.h"
#include "phMeteorSDKTest.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}