#include <gtest/gtest.h>
#include "ph_linear_motion.h"
#include <memory>
// Demonstrate some basic assertions.
TEST(phLinearMotionOBj, connect) {
  // expect connect
  ph_linear_motion mover;
  EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_error, mover.connect());
}
// Demonstrate some basic assertions.
TEST(phLinearMotionPtr, connect) {
  // expect connect
  ph_linear_motion mover;
  EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_error, mover.connect());
  Iph_axis_motion* moverPtr = new ph_linear_motion();
  EXPECT_EQ(wgm_feedbacks::enum_sub_sys_feedback::sub_error, moverPtr->connect());
  delete moverPtr;
}
// Demonstrate some basic assertions.
TEST(phLinearMotionPtr, deletePtr) {
  // expect connect
 std::unique_ptr< Iph_axis_motion> moverPtr = std::make_unique< ph_linear_motion>();
   moverPtr.reset();
  EXPECT_EQ(moverPtr, nullptr);
}