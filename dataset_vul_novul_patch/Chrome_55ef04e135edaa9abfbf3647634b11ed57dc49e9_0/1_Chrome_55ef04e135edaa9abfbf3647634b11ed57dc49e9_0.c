 void PPB_Widget_Impl::Invalidate(const PP_Rect* dirty) {
   const PPP_Widget_Dev* widget = static_cast<const PPP_Widget_Dev*>(
       instance()->module()->GetPluginInterface(PPP_WIDGET_DEV_INTERFACE));
   if (!widget)
    return;
  ScopedResourceId resource(this);
  widget->Invalidate(instance()->pp_instance(), resource.id, dirty);
}
