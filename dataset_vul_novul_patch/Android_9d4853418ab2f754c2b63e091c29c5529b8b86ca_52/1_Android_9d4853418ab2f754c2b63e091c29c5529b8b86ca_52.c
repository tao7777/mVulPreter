image_pixel_setf(image_pixel *this, unsigned int max)
 {
   this->redf = this->red / (double)max;
   this->greenf = this->green / (double)max;
   this->bluef = this->blue / (double)max;
   this->alphaf = this->alpha / (double)max;
 
   if (this->red < max)
       this->rede = this->redf * DBL_EPSILON;
    else
       this->rede = 0;
   if (this->green < max)
       this->greene = this->greenf * DBL_EPSILON;
    else
       this->greene = 0;
   if (this->blue < max)
       this->bluee = this->bluef * DBL_EPSILON;
    else
       this->bluee = 0;
   if (this->alpha < max)
       this->alphae = this->alphaf * DBL_EPSILON;
    else
       this->alphae = 0;
}
