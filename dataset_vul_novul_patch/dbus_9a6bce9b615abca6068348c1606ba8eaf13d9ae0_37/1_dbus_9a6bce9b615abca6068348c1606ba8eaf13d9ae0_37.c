my_object_set_property (GObject      *object,
                        guint         prop_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
  MyObject *mobject;
  mobject = MY_OBJECT (object);
  switch (prop_id)
    {
    case PROP_THIS_IS_A_STRING:
      g_free (mobject->this_is_a_string);
      mobject->this_is_a_string = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}
