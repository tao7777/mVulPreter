my_object_error_get_type (void)
{
	static GType etype = 0;
	if (etype == 0)
	{
		static const GEnumValue values[] =
		{
			ENUM_ENTRY (MY_OBJECT_ERROR_FOO, "Foo"),
			ENUM_ENTRY (MY_OBJECT_ERROR_BAR, "Bar"),
			{ 0, 0, 0 }
		};
		etype = g_enum_register_static ("MyObjectError", values);
	}
	return etype;
}
