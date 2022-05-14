 ResizeGripper::ResizeGripper(ResizeGripperDelegate* delegate)
    : delegate_(delegate),
      initial_position_(0),
      gripper_visible_(false) {
   ResourceBundle &rb = ResourceBundle::GetSharedInstance();
   SkBitmap* gripper_image = rb.GetBitmapNamed(IDR_RESIZE_GRIPPER);
  // Explicitly set the image size so that the preferred size is fixed to that
  // of the image. If we didn't do this the preferred size would change
  // depending upon whether the gripper was visible.
  SetImageSize(gfx::Size(gripper_image->width(), gripper_image->height()));
 }
