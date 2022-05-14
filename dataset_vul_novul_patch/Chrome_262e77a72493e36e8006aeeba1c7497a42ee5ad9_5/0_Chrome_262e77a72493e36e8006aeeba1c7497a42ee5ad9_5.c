void VRDisplay::OnVSync(device::mojom::blink::VRPosePtr pose,
                         mojo::common::mojom::blink::TimeDeltaPtr time,
                         int16_t frame_id,
                         device::mojom::blink::VRVSyncProvider::Status error) {
  DVLOG(2) << __FUNCTION__;
   v_sync_connection_failed_ = false;
   switch (error) {
     case device::mojom::blink::VRVSyncProvider::Status::SUCCESS:
      break;
    case device::mojom::blink::VRVSyncProvider::Status::CLOSING:
      return;
  }
  pending_vsync_ = false;

  WTF::TimeDelta time_delta =
      WTF::TimeDelta::FromMicroseconds(time->microseconds);
  if (timebase_ < 0) {
    timebase_ = WTF::MonotonicallyIncreasingTime() - time_delta.InSecondsF();
  }

  frame_pose_ = std::move(pose);
  vr_frame_id_ = frame_id;

  Platform::Current()->CurrentThread()->GetWebTaskRunner()->PostTask(
      BLINK_FROM_HERE,
      WTF::Bind(&VRDisplay::ProcessScheduledAnimations,
                WrapWeakPersistent(this), timebase_ + time_delta.InSecondsF()));
}
