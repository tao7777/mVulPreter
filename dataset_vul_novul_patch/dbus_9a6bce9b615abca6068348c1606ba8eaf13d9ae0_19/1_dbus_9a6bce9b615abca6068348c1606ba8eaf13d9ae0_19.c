my_object_get_property (GObject      *object,
                        guint         prop_id,
                        GValue       *value,
                        GParamSpec   *pspec)
{
  MyObject *mobject;
  mobject = MY_OBJECT (object);
  switch (prop_id)
    {
    case PROP_THIS_IS_A_STRING:
      g_value_set_string (value, mobject->this_is_a_string);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}
