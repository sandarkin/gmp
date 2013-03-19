dnl  SPARC T1 32-bit mpn_addmul_1.

dnl  Contributed to the GNU project by David Miller.

dnl  Copyright 2010, 2013 Free Software Foundation, Inc.

dnl  This file is part of the GNU MP Library.

dnl  The GNU MP Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 3 of the License, or (at
dnl  your option) any later version.

dnl  The GNU MP Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.

include(`../config.m4')

C		   cycles/limb
C UltraSPARC T1:       24
C UltraSPARC T2:       19
C UltraSPARC T3:       19
C UltraSPARC T4:       5

C INPUT PARAMETERS
define(`rp',	`%i0')
define(`up',	`%i1')
define(`n',	`%i2')
define(`v0',	`%i3')

ASM_START()
PROLOGUE(mpn_addmul_1)
	save	%sp, -96, %sp
	srl	n, 0, %o4
	srl	v0, 0, %g1
	subcc	%o4, 1, %o4
	be	L(final_one)
	 clr	%o5

L(top):	lduw	[up+0], %l0
	lduw	[rp+0], %l2
	lduw	[up+4], %l1
	lduw	[rp+4], %l3
	mulx	%l0, %g1, %g3
	add	up, 8, up
	mulx	%l1, %g1, %o3
	sub	%o4, 2, %o4
	add	rp, 8, rp
	add	%l2, %g3, %g3
	add	%o5, %g3, %g3
	stw	%g3, [rp-8]
	srlx	%g3, 32, %o5
	add	%l3, %o3, %o3
	add	%o5, %o3, %o3
	stw	%o3, [rp-4]
	brgz	%o4, L(top)
	 srlx	%o3, 32, %o5

	brlz,pt	%o4, L(done)
	 nop

L(final_one):
	lduw	[up+0], %l0
	lduw	[rp+0], %l2
	mulx	%l0, %g1, %g3
	add	%l2, %g3, %g3
	add	%o5, %g3, %g3
	stw	%g3, [rp+0]
	srlx	%g3, 32, %o5

L(done):
	ret
	 restore %o5, 0, %o0
EPILOGUE()
