PartialMagnificationController::PartialMagnificationController()
  void OnDelegatedFrameDamage(const gfx::Rect& damage_rect_in_dip) override {}

  void OnDeviceScaleFactorChanged(float device_scale_factor) override {
    // Redrawing will take care of scale factor change.
  }

  base::Closure PrepareForLayerBoundsChange() override {
    return base::Closure();
  }

  ui::Layer layer_;
  bool stroke_;

  DISALLOW_COPY_AND_ASSIGN(ContentMask);
};

PartialMagnificationController::PartialMagnificationController() {
   Shell::GetInstance()->AddPreTargetHandler(this);
 }
