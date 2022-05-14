 scoped_refptr<Image> CSSPaintValue::GetImage(
     const ImageResourceObserver& client,
     const Document& document,
    const ComputedStyle&,
     const FloatSize& target_size) {
   if (!generator_) {
     generator_ = CSSPaintImageGenerator::Create(
         GetName(), document, paint_image_generator_observer_);
  }

  if (!ParseInputArguments(document))
    return nullptr;

  return generator_->Paint(client, RoundedIntSize(target_size),
                           parsed_input_arguments_);
}
