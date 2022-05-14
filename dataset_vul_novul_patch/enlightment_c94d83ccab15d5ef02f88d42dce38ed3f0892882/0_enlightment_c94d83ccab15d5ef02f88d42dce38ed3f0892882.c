__imlib_Ellipse_DrawToData(int xc, int yc, int a, int b, DATA32 color,
                           DATA32 * dst, int dstw, int clx, int cly, int clw,
                           int clh, ImlibOp op, char dst_alpha, char blend)
{
   ImlibPointDrawFunction pfunc;
   int                 xx, yy, x, y, prev_x, prev_y, ty, by, lx, rx;
   DATA32              a2, b2, *tp, *bp;
   DATA64              dx, dy;

   if (A_VAL(&color) == 0xff)
      blend = 0;
   pfunc = __imlib_GetPointDrawFunction(op, dst_alpha, blend);
   if (!pfunc)
      return;

   xc -= clx;
   yc -= cly;
   dst += (dstw * cly) + clx;

   a2 = a * a;
   b2 = b * b;

   yy = b << 16;
   prev_y = b;

   dx = a2 * b;
   dy = 0;

   ty = yc - b - 1;
   by = yc + b;
   lx = xc - 1;
   rx = xc;

   tp = dst + (dstw * ty) + lx;
   bp = dst + (dstw * by) + lx;

   while (dy < dx)
     {
        int                 len;

        y = yy >> 16;
        y += ((yy - (y << 16)) >> 15);

        if (prev_y != y)
          {
             prev_y = y;
             dx -= a2;
             ty++;
             by--;
             tp += dstw;
             bp -= dstw;
          }

        len = rx - lx;

        if (IN_RANGE(lx, ty, clw, clh))
           pfunc(color, tp);
        if (IN_RANGE(rx, ty, clw, clh))
           pfunc(color, tp + len);
        if (IN_RANGE(lx, by, clw, clh))
           pfunc(color, bp);
         if (IN_RANGE(rx, by, clw, clh))
            pfunc(color, bp + len);
 
        if (dx < 1)
           dx = 1;

         dy += b2;
         yy -= ((dy << 16) / dx);
         lx--;

        if ((lx < 0) && (rx > clw))
           return;
        if ((ty > clh) || (by < 0))
           return;
     }

   xx = yy;
   prev_x = xx >> 16;

   dx = dy;

   ty++;
   by--;

   tp += dstw;
   bp -= dstw;

   while (ty < yc)
     {
        int                 len;

        x = xx >> 16;
        x += ((xx - (x << 16)) >> 15);

        if (prev_x != x)
          {
             prev_x = x;
             dy += b2;
             lx--;
             rx++;
             tp--;
             bp--;
          }

        len = rx - lx;

        if (IN_RANGE(lx, ty, clw, clh))
           pfunc(color, tp);
        if (IN_RANGE(rx, ty, clw, clh))
           pfunc(color, tp + len);
        if (IN_RANGE(lx, by, clw, clh))
           pfunc(color, bp);
        if (IN_RANGE(rx, by, clw, clh))
           pfunc(color, bp + len);

         if (IN_RANGE(rx, by, clw, clh))
            pfunc(color, bp + len);
 
        if (dy < 1)
           dy = 1;

         dx -= a2;
         xx += ((dx << 16) / dy);
         ty++;
        if ((ty > clh) || (by < 0))
           return;
     }
}
