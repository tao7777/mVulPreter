image_transform_default_ini(PNG_CONST image_transform *this,
extern void image_transform_default_ini(const image_transform *this,
   transform_display *that); /* silence GCC warnings */

void /* private, but almost always needed */
image_transform_default_ini(const image_transform *this,
     transform_display *that)
 {
    this->next->ini(this->next, that);
}
