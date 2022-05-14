my_object_echo_variant (MyObject *obj, GValue *variant, GValue *ret, GError **error)
{
    GType t;
    t = G_VALUE_TYPE(variant);
    g_value_init (ret, t);
    g_value_copy (variant, ret);
    return TRUE;
}
