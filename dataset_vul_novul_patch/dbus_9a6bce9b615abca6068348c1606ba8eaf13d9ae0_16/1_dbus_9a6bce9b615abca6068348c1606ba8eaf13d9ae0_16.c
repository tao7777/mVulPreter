my_object_finalize (GObject *object)
{
  MyObject *mobject = MY_OBJECT (object);
  g_free (mobject->this_is_a_string);
  (G_OBJECT_CLASS (my_object_parent_class)->finalize) (object);
}
