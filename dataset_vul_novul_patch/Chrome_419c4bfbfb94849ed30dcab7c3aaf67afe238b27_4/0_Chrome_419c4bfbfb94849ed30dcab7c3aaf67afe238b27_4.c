 void ImageBitmapFactories::ImageBitmapLoader::Trace(blink::Visitor* visitor) {
  ContextLifecycleObserver::Trace(visitor);
   visitor->Trace(factory_);
   visitor->Trace(resolver_);
   visitor->Trace(options_);
}
