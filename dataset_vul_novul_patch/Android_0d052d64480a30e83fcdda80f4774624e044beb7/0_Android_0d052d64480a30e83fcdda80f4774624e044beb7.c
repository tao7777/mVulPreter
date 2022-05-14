void silk_NLSF_stabilize(
          opus_int16            *NLSF_Q15, /* I/O   Unstable/stabilized normalized LSF vector in Q15 [L]       */
 const opus_int16            *NDeltaMin_Q15, /* I     Min distance vector, NDeltaMin_Q15[L] must be >= 1 [L+1]   */
 const opus_int              L                   /* I     Number of NLSF parameters in the input vector              */
)
{
    opus_int   i, I=0, k, loops;
    opus_int16 center_freq_Q15;
    opus_int32 diff_Q15, min_diff_Q15, min_center_Q15, max_center_Q15;

 /* This is necessary to ensure an output within range of a opus_int16 */
    silk_assert( NDeltaMin_Q15[L] >= 1 );

 for( loops = 0; loops < MAX_LOOPS; loops++ ) {
 /**************************/
 /* Find smallest distance */
 /**************************/
 /* First element */
        min_diff_Q15 = NLSF_Q15[0] - NDeltaMin_Q15[0];
        I = 0;
 /* Middle elements */
 for( i = 1; i <= L-1; i++ ) {
            diff_Q15 = NLSF_Q15[i] - ( NLSF_Q15[i-1] + NDeltaMin_Q15[i] );
 if( diff_Q15 < min_diff_Q15 ) {
                min_diff_Q15 = diff_Q15;
                I = i;
 }
 }
 /* Last element */
        diff_Q15 = ( 1 << 15 ) - ( NLSF_Q15[L-1] + NDeltaMin_Q15[L] );
 if( diff_Q15 < min_diff_Q15 ) {
            min_diff_Q15 = diff_Q15;
            I = L;
 }

 /***************************************************/
 /* Now check if the smallest distance non-negative */
 /***************************************************/
 if( min_diff_Q15 >= 0 ) {
 return;
 }

 if( I == 0 ) {
 /* Move away from lower limit */
            NLSF_Q15[0] = NDeltaMin_Q15[0];

 } else if( I == L) {
 /* Move away from higher limit */
            NLSF_Q15[L-1] = ( 1 << 15 ) - NDeltaMin_Q15[L];

 } else {
 /* Find the lower extreme for the location of the current center frequency */
            min_center_Q15 = 0;
 for( k = 0; k < I; k++ ) {
                min_center_Q15 += NDeltaMin_Q15[k];
 }
            min_center_Q15 += silk_RSHIFT( NDeltaMin_Q15[I], 1 );

 /* Find the upper extreme for the location of the current center frequency */
            max_center_Q15 = 1 << 15;
 for( k = L; k > I; k-- ) {
                max_center_Q15 -= NDeltaMin_Q15[k];
 }
            max_center_Q15 -= silk_RSHIFT( NDeltaMin_Q15[I], 1 );

 /* Move apart, sorted by value, keeping the same center frequency */
            center_freq_Q15 = (opus_int16)silk_LIMIT_32( silk_RSHIFT_ROUND( (opus_int32)NLSF_Q15[I-1] + (opus_int32)NLSF_Q15[I], 1 ),
                min_center_Q15, max_center_Q15 );
            NLSF_Q15[I-1] = center_freq_Q15 - silk_RSHIFT( NDeltaMin_Q15[I], 1 );
            NLSF_Q15[I] = NLSF_Q15[I-1] + NDeltaMin_Q15[I];
 }
 }

 /* Safe and simple fall back method, which is less ideal than the above */
 if( loops == MAX_LOOPS )
 {
 /* Insertion sort (fast for already almost sorted arrays):   */
 /* Best case:  O(n)   for an already sorted array            */
 /* Worst case: O(n^2) for an inversely sorted array          */
        silk_insertion_sort_increasing_all_values_int16( &NLSF_Q15[0], L );

 /* First NLSF should be no less than NDeltaMin[0] */
        NLSF_Q15[0] = silk_max_int( NLSF_Q15[0], NDeltaMin_Q15[0] );

 
         /* Keep delta_min distance between the NLSFs */
         for( i = 1; i < L; i++ )
            NLSF_Q15[i] = silk_max_int( NLSF_Q15[i], silk_ADD_SAT16( NLSF_Q15[i-1], NDeltaMin_Q15[i] ) );
 
         /* Last NLSF should be no higher than 1 - NDeltaMin[L] */
         NLSF_Q15[L-1] = silk_min_int( NLSF_Q15[L-1], (1<<15) - NDeltaMin_Q15[L] );

 /* Keep NDeltaMin distance between the NLSFs */
 for( i = L-2; i >= 0; i-- )
            NLSF_Q15[i] = silk_min_int( NLSF_Q15[i], NLSF_Q15[i+1] - NDeltaMin_Q15[i+1] );
 }
}
