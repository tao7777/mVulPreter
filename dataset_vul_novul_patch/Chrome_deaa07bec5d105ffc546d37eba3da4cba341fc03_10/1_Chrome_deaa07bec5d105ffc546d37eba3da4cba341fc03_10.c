  static base::Callback<void(const gfx::Image&)> Wrap(
      const base::Callback<void(const SkBitmap&)>& image_decoded_callback) {
    auto* handler = new ImageDecodedHandlerWithTimeout(image_decoded_callback);
    base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
        FROM_HERE,
        base::Bind(&ImageDecodedHandlerWithTimeout::OnImageDecoded,
                   handler->weak_ptr_factory_.GetWeakPtr(), gfx::Image()),
        base::TimeDelta::FromSeconds(kDecodeLogoTimeoutSeconds));
    return base::Bind(&ImageDecodedHandlerWithTimeout::OnImageDecoded,
                      handler->weak_ptr_factory_.GetWeakPtr());
  }
