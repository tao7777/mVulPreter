 ResizeGripper::ResizeGripper(ResizeGripperDelegate* delegate)
    : delegate_(delegate), initial_position_(0) {
   ResourceBundle &rb = ResourceBundle::GetSharedInstance();
   SkBitmap* gripper_image = rb.GetBitmapNamed(IDR_RESIZE_GRIPPER);
  SetImage(gripper_image);
 }
