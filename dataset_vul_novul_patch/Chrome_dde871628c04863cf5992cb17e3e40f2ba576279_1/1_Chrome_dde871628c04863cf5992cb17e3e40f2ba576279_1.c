PepperVideoRenderer2D::PepperVideoRenderer2D()
    : instance_(nullptr),
      event_handler_(nullptr),
      merge_buffer_(nullptr),
      dips_to_device_scale_(1.0f),
       dips_to_view_scale_(1.0f),
       flush_pending_(false),
       frame_received_(false),
       callback_factory_(this),
       weak_factory_(this) {
 }
