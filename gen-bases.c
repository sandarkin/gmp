/* Generate mp_bases data.

Copyright 1991, 1993, 1994, 1996, 2000, 2002, 2004, 2011, 2012, 2015
Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#include "bootstrap.c"


int    chars_per_limb;
mpz_t  big_base;
int    normalization_steps;
mpz_t  big_base_inverted;

mpz_t  t;

#define POW2_P(n)  (((n) & ((n) - 1)) == 0)

unsigned int
ulog2 (unsigned int x)
{
  unsigned int i;
  for (i = 0;  x != 0;  i++)
    x >>= 1;
  return i;
}

void
generate (int limb_bits, int nail_bits, int base)
{
  int  numb_bits = limb_bits - nail_bits;

  mpz_set_ui (t, 1L);
  mpz_mul_2exp (t, t, numb_bits);
  mpz_set_ui (big_base, (long) base);
  chars_per_limb = 0;
  while (mpz_cmp (big_base, t) <= 0)
    {
      mpz_mul_ui (big_base, big_base, (long) base);
      chars_per_limb++;
    }

  mpz_ui_pow_ui (big_base, (long) base, (long) chars_per_limb);

  normalization_steps = limb_bits - mpz_sizeinbase (big_base, 2);

  mpz_set_ui (t, 1L);
  mpz_mul_2exp (t, t, 2*limb_bits - normalization_steps);
  mpz_tdiv_q (big_base_inverted, t, big_base);
  mpz_clrbit (big_base_inverted, limb_bits);
}

void
header (int limb_bits, int nail_bits)
{
  int  numb_bits = limb_bits - nail_bits;

  generate (limb_bits, nail_bits, 10);

  printf ("/* This file generated by gen-bases.c - DO NOT EDIT. */\n");
  printf ("\n");
  printf ("#if GMP_NUMB_BITS != %d\n", numb_bits);
  printf ("Error, error, this data is for %d bits\n", numb_bits);
  printf ("#endif\n");
  printf ("\n");
  printf ("/* mp_bases[10] data, as literal values */\n");
  printf ("#define MP_BASES_CHARS_PER_LIMB_10      %d\n", chars_per_limb);
  printf ("#define MP_BASES_BIG_BASE_10            CNST_LIMB(0x");
  mpz_out_str (stdout, 16, big_base);
  printf (")\n");
  printf ("#define MP_BASES_BIG_BASE_INVERTED_10   CNST_LIMB(0x");
  mpz_out_str (stdout, 16, big_base_inverted);
  printf (")\n");
  printf ("#define MP_BASES_NORMALIZATION_STEPS_10 %d\n", normalization_steps);
}


#define EXTRA 16

/* Compute log(2)/log(b) as a fixnum. */
void
mp_2logb (mpz_t r, int bi, int prec)
{
  mpz_t t, t2, two, b;
  int i;

  mpz_init_set_ui (t, 1);
  mpz_mul_2exp (t, t, prec+EXTRA);

  mpz_init (t2);

  mpz_init_set_ui (two, 2);
  mpz_mul_2exp (two, two, prec+EXTRA);

  mpz_set_ui (r, 0);

  mpz_init_set_ui (b, bi);
  mpz_mul_2exp (b, b, prec+EXTRA);

  for (i = prec-1; i >= 0; i--)
    {
      mpz_mul_2exp (b, b, prec+EXTRA);
      mpz_sqrt (b, b);

      mpz_mul (t2, t, b);
      mpz_tdiv_q_2exp (t2, t2, prec+EXTRA);

      if (mpz_cmp (t2, two) < 0)	/* not too large? */
	{
	  mpz_setbit (r, i);		/* set next less significant bit */
	  mpz_swap (t, t2);		/* new value acceptable */
	}
    }

  mpz_clear (t);
  mpz_clear (t2);
  mpz_clear (two);
  mpz_clear (b);
}

void
table (int limb_bits, int nail_bits)
{
  int  numb_bits = limb_bits - nail_bits;
  int  base;
  mpz_t r, t, logb2, log2b;

  mpz_init (r);
  mpz_init (t);
  mpz_init (logb2);
  mpz_init (log2b);

  printf ("/* This file generated by gen-bases.c - DO NOT EDIT. */\n");
  printf ("\n");
  printf ("#include \"gmp.h\"\n");
  printf ("#include \"gmp-impl.h\"\n");
  printf ("\n");
  printf ("#if GMP_NUMB_BITS != %d\n", numb_bits);
  printf ("Error, error, this data is for %d bits\n", numb_bits);
  printf ("#endif\n");
  printf ("\n");
  puts ("const struct bases mp_bases[257] =\n{");
  puts ("  /*   0 */ { 0, 0, 0, 0, 0 },");
  puts ("  /*   1 */ { 0, 0, 0, 0, 0 },");
  for (base = 2; base <= 256; base++)
    {
      generate (limb_bits, nail_bits, base);
      mp_2logb (r, base, limb_bits + 8);
      mpz_tdiv_q_2exp (logb2, r, 8);
      mpz_set_ui (t, 1);
      mpz_mul_2exp (t, t, 2*limb_bits + 5);
      mpz_sub_ui (t, t, 1);
      mpz_add_ui (r, r, 1);
      mpz_tdiv_q (log2b, t, r);

      printf ("  /* %3u */ { ", base);
      if (POW2_P (base))
	{
          mpz_set_ui (big_base, ulog2 (base) - 1);
	  mpz_set_ui (big_base_inverted, 0);
	}

      printf ("%u,", chars_per_limb);
      printf (" CNST_LIMB(0x");
      mpz_out_str (stdout, 16, logb2);
      printf ("), CNST_LIMB(0x");
      mpz_out_str (stdout, 16, log2b);
      printf ("), CNST_LIMB(0x");
      mpz_out_str (stdout, 16, big_base);
      printf ("), CNST_LIMB(0x");
      mpz_out_str (stdout, 16, big_base_inverted);
      printf (") },\n");
    }

  puts ("};");

  mpz_clear (r);
  mpz_clear (t);
  mpz_clear (logb2);
  mpz_clear (log2b);

}

int
main (int argc, char **argv)
{
  int  limb_bits, nail_bits;

  mpz_init (big_base);
  mpz_init (big_base_inverted);
  mpz_init (t);

  if (argc != 4)
    {
      fprintf (stderr, "Usage: gen-bases <header|table> <limbbits> <nailbits>\n");
      exit (1);
    }

  limb_bits = atoi (argv[2]);
  nail_bits = atoi (argv[3]);

  if (limb_bits <= 0
      || nail_bits < 0
      || nail_bits >= limb_bits)
    {
      fprintf (stderr, "Invalid limb/nail bits: %d %d\n",
               limb_bits, nail_bits);
      exit (1);
    }

  if (strcmp (argv[1], "header") == 0)
    header (limb_bits, nail_bits);
  else if (strcmp (argv[1], "table") == 0)
    table (limb_bits, nail_bits);
  else
    {
      fprintf (stderr, "Invalid header/table choice: %s\n", argv[1]);
      exit (1);
    }

  return 0;
}
