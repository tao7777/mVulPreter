void ImageCapture::SetMediaTrackConstraints(
    ScriptPromiseResolver* resolver,
    const HeapVector<MediaTrackConstraintSet>& constraints_vector) {
  DCHECK_GT(constraints_vector.size(), 0u);
  if (!service_) {
    resolver->Reject(DOMException::Create(kNotFoundError, kNoServiceError));
    return;
  }
  auto constraints = constraints_vector[0];

  if ((constraints.hasWhiteBalanceMode() &&
       !capabilities_.hasWhiteBalanceMode()) ||
      (constraints.hasExposureMode() && !capabilities_.hasExposureMode()) ||
      (constraints.hasFocusMode() && !capabilities_.hasFocusMode()) ||
      (constraints.hasExposureCompensation() &&
       !capabilities_.hasExposureCompensation()) ||
      (constraints.hasColorTemperature() &&
       !capabilities_.hasColorTemperature()) ||
      (constraints.hasIso() && !capabilities_.hasIso()) ||
      (constraints.hasBrightness() && !capabilities_.hasBrightness()) ||
      (constraints.hasContrast() && !capabilities_.hasContrast()) ||
      (constraints.hasSaturation() && !capabilities_.hasSaturation()) ||
      (constraints.hasSharpness() && !capabilities_.hasSharpness()) ||
      (constraints.hasZoom() && !capabilities_.hasZoom()) ||
      (constraints.hasTorch() && !capabilities_.hasTorch())) {
    resolver->Reject(
        DOMException::Create(kNotSupportedError, "Unsupported constraint(s)"));
    return;
  }

  auto settings = media::mojom::blink::PhotoSettings::New();
  MediaTrackConstraintSet temp_constraints = current_constraints_;

  settings->has_white_balance_mode = constraints.hasWhiteBalanceMode() &&
                                     constraints.whiteBalanceMode().IsString();
  if (settings->has_white_balance_mode) {
    const auto white_balance_mode =
        constraints.whiteBalanceMode().GetAsString();
    if (capabilities_.whiteBalanceMode().Find(white_balance_mode) ==
        kNotFound) {
      resolver->Reject(DOMException::Create(kNotSupportedError,
                                            "Unsupported whiteBalanceMode."));
      return;
    }
    temp_constraints.setWhiteBalanceMode(constraints.whiteBalanceMode());
    settings->white_balance_mode = ParseMeteringMode(white_balance_mode);
  }
  settings->has_exposure_mode =
      constraints.hasExposureMode() && constraints.exposureMode().IsString();
  if (settings->has_exposure_mode) {
    const auto exposure_mode = constraints.exposureMode().GetAsString();
    if (capabilities_.exposureMode().Find(exposure_mode) == kNotFound) {
      resolver->Reject(DOMException::Create(kNotSupportedError,
                                            "Unsupported exposureMode."));
      return;
    }
    temp_constraints.setExposureMode(constraints.exposureMode());
    settings->exposure_mode = ParseMeteringMode(exposure_mode);
  }

  settings->has_focus_mode =
      constraints.hasFocusMode() && constraints.focusMode().IsString();
  if (settings->has_focus_mode) {
    const auto focus_mode = constraints.focusMode().GetAsString();
    if (capabilities_.focusMode().Find(focus_mode) == kNotFound) {
      resolver->Reject(
          DOMException::Create(kNotSupportedError, "Unsupported focusMode."));
      return;
    }
    temp_constraints.setFocusMode(constraints.focusMode());
    settings->focus_mode = ParseMeteringMode(focus_mode);
  }

  if (constraints.hasPointsOfInterest() &&
      constraints.pointsOfInterest().IsPoint2DSequence()) {
    for (const auto& point :
         constraints.pointsOfInterest().GetAsPoint2DSequence()) {
      auto mojo_point = media::mojom::blink::Point2D::New();
      mojo_point->x = point.x();
      mojo_point->y = point.y();
      settings->points_of_interest.push_back(std::move(mojo_point));
    }
    temp_constraints.setPointsOfInterest(constraints.pointsOfInterest());
  }

  settings->has_exposure_compensation =
      constraints.hasExposureCompensation() &&
      constraints.exposureCompensation().IsDouble();
  if (settings->has_exposure_compensation) {
    const auto exposure_compensation =
        constraints.exposureCompensation().GetAsDouble();
    if (exposure_compensation < capabilities_.exposureCompensation()->min() ||
        exposure_compensation > capabilities_.exposureCompensation()->max()) {
      resolver->Reject(DOMException::Create(
          kNotSupportedError, "exposureCompensation setting out of range"));
      return;
    }
    temp_constraints.setExposureCompensation(
        constraints.exposureCompensation());
    settings->exposure_compensation = exposure_compensation;
  }
  settings->has_color_temperature = constraints.hasColorTemperature() &&
                                    constraints.colorTemperature().IsDouble();
  if (settings->has_color_temperature) {
    const auto color_temperature = constraints.colorTemperature().GetAsDouble();
    if (color_temperature < capabilities_.colorTemperature()->min() ||
        color_temperature > capabilities_.colorTemperature()->max()) {
      resolver->Reject(DOMException::Create(
          kNotSupportedError, "colorTemperature setting out of range"));
      return;
    }
    temp_constraints.setColorTemperature(constraints.colorTemperature());
    settings->color_temperature = color_temperature;
  }
  settings->has_iso = constraints.hasIso() && constraints.iso().IsDouble();
  if (settings->has_iso) {
    const auto iso = constraints.iso().GetAsDouble();
    if (iso < capabilities_.iso()->min() || iso > capabilities_.iso()->max()) {
      resolver->Reject(
          DOMException::Create(kNotSupportedError, "iso setting out of range"));
      return;
    }
    temp_constraints.setIso(constraints.iso());
    settings->iso = iso;
  }

  settings->has_brightness =
      constraints.hasBrightness() && constraints.brightness().IsDouble();
  if (settings->has_brightness) {
    const auto brightness = constraints.brightness().GetAsDouble();
    if (brightness < capabilities_.brightness()->min() ||
        brightness > capabilities_.brightness()->max()) {
      resolver->Reject(DOMException::Create(kNotSupportedError,
                                            "brightness setting out of range"));
      return;
    }
    temp_constraints.setBrightness(constraints.brightness());
    settings->brightness = brightness;
  }
  settings->has_contrast =
      constraints.hasContrast() && constraints.contrast().IsDouble();
  if (settings->has_contrast) {
    const auto contrast = constraints.contrast().GetAsDouble();
    if (contrast < capabilities_.contrast()->min() ||
        contrast > capabilities_.contrast()->max()) {
      resolver->Reject(DOMException::Create(kNotSupportedError,
                                            "contrast setting out of range"));
      return;
    }
    temp_constraints.setContrast(constraints.contrast());
    settings->contrast = contrast;
  }
  settings->has_saturation =
      constraints.hasSaturation() && constraints.saturation().IsDouble();
  if (settings->has_saturation) {
    const auto saturation = constraints.saturation().GetAsDouble();
    if (saturation < capabilities_.saturation()->min() ||
        saturation > capabilities_.saturation()->max()) {
      resolver->Reject(DOMException::Create(kNotSupportedError,
                                            "saturation setting out of range"));
      return;
    }
    temp_constraints.setSaturation(constraints.saturation());
    settings->saturation = saturation;
  }
  settings->has_sharpness =
      constraints.hasSharpness() && constraints.sharpness().IsDouble();
  if (settings->has_sharpness) {
    const auto sharpness = constraints.sharpness().GetAsDouble();
    if (sharpness < capabilities_.sharpness()->min() ||
        sharpness > capabilities_.sharpness()->max()) {
      resolver->Reject(DOMException::Create(kNotSupportedError,
                                            "sharpness setting out of range"));
      return;
    }
    temp_constraints.setSharpness(constraints.sharpness());
    settings->sharpness = sharpness;
  }

  settings->has_zoom = constraints.hasZoom() && constraints.zoom().IsDouble();
  if (settings->has_zoom) {
    const auto zoom = constraints.zoom().GetAsDouble();
    if (zoom < capabilities_.zoom()->min() ||
        zoom > capabilities_.zoom()->max()) {
      resolver->Reject(DOMException::Create(kNotSupportedError,
                                            "zoom setting out of range"));
      return;
    }
    temp_constraints.setZoom(constraints.zoom());
    settings->zoom = zoom;
  }

  settings->has_torch =
      constraints.hasTorch() && constraints.torch().IsBoolean();
  if (settings->has_torch) {
    const auto torch = constraints.torch().GetAsBoolean();
    if (torch && !capabilities_.torch()) {
      resolver->Reject(
          DOMException::Create(kNotSupportedError, "torch not supported"));
      return;
    }
    temp_constraints.setTorch(constraints.torch());
    settings->torch = torch;
  }

  current_constraints_ = temp_constraints;

  service_requests_.insert(resolver);
 
   MediaTrackConstraints resolver_constraints;
   resolver_constraints.setAdvanced(constraints_vector);

  // An IDLDictionaryBase cannot safely be bound into a callback so the
  // ScriptValue is created ahead of time. See https://crbug.com/759457.
  auto resolver_cb = WTF::Bind(
      &ImageCapture::ResolveWithMediaTrackConstraints, WrapPersistent(this),
      ScriptValue::From(resolver->GetScriptState(), resolver_constraints));
 
   service_->SetOptions(
       stream_track_->Component()->Source()->Id(), std::move(settings),
      ConvertToBaseCallback(WTF::Bind(
          &ImageCapture::OnMojoSetOptions, WrapPersistent(this),
          WrapPersistent(resolver), WTF::Passed(std::move(resolver_cb)),
          false /* trigger_take_photo */)));
}
