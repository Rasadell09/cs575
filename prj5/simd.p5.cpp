#include "simd.p5.h"


void
SimdMul( float *a, float *b,   float *c,   int len )
{
	int limit = ( len / SSE_WIDTH ) * SSE_WIDTH;
	__asm
	(
	    ".att_syntax\n\t"
	    "movq    -24(%rbp), %rbx\n\t"		// a
	    "movq    -32(%rbp), %rcx\n\t"		// b
	    "movq    -40(%rbp), %rdx\n\t"		// c
	);

	for ( int i = 0; i < limit; i += SSE_WIDTH )
	{
		__asm
		(
		    ".att_syntax\n\t"
		    "movups	(%rbx), %xmm0\n\t"	// load the first sse register
		    "movups	(%rcx), %xmm1\n\t"	// load the second sse register
		    "mulps	%xmm1, %xmm0\n\t"	// do the multiply
		    "movups	%xmm0, (%rdx)\n\t"	// store the result
		    "addq $16, %rbx\n\t"
		    "addq $16, %rcx\n\t"
		    "addq $16, %rdx\n\t"
		);
	}

	for ( int i = limit; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}



float
SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len / SSE_WIDTH ) * SSE_WIDTH;

	__asm
	(
	    ".att_syntax\n\t"
	    "movq    -40(%rbp), %rbx\n\t"		// a
	    "movq    -48(%rbp), %rcx\n\t"		// b
	    "leaq    -32(%rbp), %rdx\n\t"		// &sum[0]
	    "movups	 (%rdx), %xmm2\n\t"		// 4 copies of 0. in xmm2
	);

	for ( int i = 0; i < limit; i += SSE_WIDTH )
	{
		__asm
		(
		    ".att_syntax\n\t"
		    "movups	(%rbx), %xmm0\n\t"	// load the first sse register
		    "movups	(%rcx), %xmm1\n\t"	// load the second sse register
		    "mulps	%xmm1, %xmm0\n\t"	// do the multiply
		    "addps	%xmm0, %xmm2\n\t"	// do the add
		    "addq $16, %rbx\n\t"
		    "addq $16, %rcx\n\t"
		);
	}

	__asm
	(
	    ".att_syntax\n\t"
	    "movups	 %xmm2, (%rdx)\n\t"	// copy the sums back to sum[ ]
	);

	for ( int i = limit; i < len; i++ )
	{
		sum[i - limit] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}
