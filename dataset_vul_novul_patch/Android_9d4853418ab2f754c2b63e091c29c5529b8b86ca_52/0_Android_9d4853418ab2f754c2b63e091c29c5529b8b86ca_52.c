image_pixel_setf(image_pixel *this, unsigned int max)
image_pixel_setf(image_pixel *this, unsigned int rMax, unsigned int gMax,
        unsigned int bMax, unsigned int aMax)
 {
   this->redf = this->red / (double)rMax;
   this->greenf = this->green / (double)gMax;
   this->bluef = this->blue / (double)bMax;
   this->alphaf = this->alpha / (double)aMax;
 
   if (this->red < rMax)
       this->rede = this->redf * DBL_EPSILON;
    else
       this->rede = 0;
   if (this->green < gMax)
       this->greene = this->greenf * DBL_EPSILON;
    else
       this->greene = 0;
   if (this->blue < bMax)
       this->bluee = this->bluef * DBL_EPSILON;
    else
       this->bluee = 0;
   if (this->alpha < aMax)
       this->alphae = this->alphaf * DBL_EPSILON;
    else
       this->alphae = 0;
}
