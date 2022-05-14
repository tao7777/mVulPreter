my_object_class_init (MyObjectClass *mobject_class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (mobject_class);
  gobject_class->finalize = my_object_finalize;
  gobject_class->set_property = my_object_set_property;
  gobject_class->get_property = my_object_get_property;
  g_object_class_install_property (gobject_class,
				   PROP_THIS_IS_A_STRING,
				   g_param_spec_string ("this_is_a_string",
                                                        _("Sample string"),
                                                        _("Example of a string property"),
                                                        "default value",
                                                        G_PARAM_READWRITE));
  signals[FROBNICATE] =
    g_signal_new ("frobnicate",
		  G_OBJECT_CLASS_TYPE (mobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT,
                  G_TYPE_NONE, 1, G_TYPE_INT);
  signals[SIG0] =
    g_signal_new ("sig0",
		  G_OBJECT_CLASS_TYPE (mobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  0,
                  NULL, NULL,
                  my_object_marshal_VOID__STRING_INT_STRING,
                  G_TYPE_NONE, 3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);
  signals[SIG1] =
    g_signal_new ("sig1",
		  G_OBJECT_CLASS_TYPE (mobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  0,
                  NULL, NULL,
                  my_object_marshal_VOID__STRING_BOXED,
                  G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_VALUE);
  signals[SIG2] =
    g_signal_new ("sig2",
		  G_OBJECT_CLASS_TYPE (mobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__BOXED,
                  G_TYPE_NONE, 1, DBUS_TYPE_G_STRING_STRING_HASHTABLE);
}
