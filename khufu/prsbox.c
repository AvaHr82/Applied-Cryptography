/* @(#)prsbox.c	4.1 7/13/89 14:44:16 */
/* 
 * prsbox - print the S Box tables
 *
 * usage:
 *
 *	prsbox [-b | -l | -s]     > sbox.c
 *	prsbox [-b | -l | -s][-h] > endian.h
 *
 *	-b  =>  force Big Endian output
 *	-l  =>  force Little Endian output
 *	-s  =>  switch Endian type output
 *	-h  =>  print endian.h indead of sbox.c
 *
 * Selecting -b will force output for a Big Endian machine, while -l 
 * force output for a Little Endian machine.  The -s option will
 * result in Endian style output that oppisite of the current
 * machine.  Not selecting -l, -b, or -s will result in Endian
 * style output for the current machine.
 *
 * NOTE: This program is currently lacking the RAND book of numbers.
 *	 For now, it uses bits 7 to 30 of rand(), which is NOT very
 *	 good, but is at least standard.  The function randdigits()
 *	 should be changed to select groups of 7 digits from the
 *	 RAND book of random numbers.
 *
 * Landon Curt Noll makes no representations concerning either the
 * merchantability of this software or the suitability of this software for
 * any particular purpose.  It is provided "as is" without express or 
 * implied warranty of any kind.
 *
 * This file has been put in the public domain.  Please leave it that way.
 *
 * By: Landon Curt Noll   (chongo@toad.com  -or  uunet!hoptoad!chongo)
 */

#include <stdio.h>
#include "prsbox.h"

/*
 * Standard S Boxes
 *
 * A standard S Box is an array (8 is standard) of 'sbox'.  Each
 * 'sbox' is 256 u32bit (32 bit unsigned) words.  Each sbox is
 * setup such that byte X of word Y != byte X of word Z, when Y != Z.
 * That is, the byte column of an 'sbox' contains all the values
 * between 0 and 255 inclusive, in some random order.
 */
sbox s_box[MAX_SBOX_COUNT];

/* 
 * 4 byte pattern for Big Endian test 
 *
 * We put this pattern in global name space rather than inside the
 * big_endian() function because some compilers will optimize the
 * routine into a `do nothing' routine because everything will be
 * based on a constant.
 */
u32bit byte4 = 0x00010203;	

void convert();		/* convert Endian order of an u32bit array */
void usage();		/* print ussage message and abort */
void parse_args();	/* parse args */
void print_endian_h();	/* print the endian.h file */
void mksbox();		/* load the standard S Boxes */
void check();		/* check if S Box is valid */
u32bit randbyte();	/* generate a random byte */
u32bit randdigits();	/* get 7 random decimal digits */
int big_end;		/* 1 => produce for a Big Endian machine, 0 => little */
int h_file;		/* 1 => print endian.h, 0 => print sbox.c */
int rands;		/* rands - number of random values fetched */
char *program;		/* our name */

main(argc, argv)
    int argc;		/* arg count */
    char *argv[];	/* the args */
{
    register int i;	/* index */
    register int s;	/* Sbox index */
    register int nbyte;	/* number of bytes to right rotate by */

    /* parse args */
    parse_args(argc, argv);

    /*
     * if only print endian.h, do it and exit
     */
    if (h_file) {
	print_endian_h();
	exit(0);
    }

    /*
     * setup the S Boxes
     */
    mksbox();

    /*
     * verify S Boxes
     */
    check();

    /*
     * convert Endian order if needed
     */
    if (! big_end) {
	convert(s_box, MAX_SBOX_COUNT);
    }

    /* 
     * print the front of the sbox.c file
     */
    puts("/*");
    puts(" * WARNING: DO NOT EDIT THIS FILE DIRECTLY!!!");
    puts(" *");
    puts(" * This file was produced by the prsbox program.");
    puts(" */");
    puts("");
    puts("#include \"prsbox.h\"");
    puts("");

    /*
     * print the standard S box array
     */
    puts("/*");
    puts(" * standard S boxes");
    puts(" */");
    printf("sbox s_box[MAX_SBOX_COUNT] = {\n", 0);
    for (s=0; s < MAX_SBOX_COUNT; ++s) {
	printf("    /* S box #%d */\n", s);
	for (i=0; i < SBOX_WORDS; i+= sizeof(u32bit)) {
	    printf("%s0x%08x, 0x%08x, 0x%08x, 0x%08x%s /* %d-%d */\n",
	      (i==0) ? "    {   " : "\t",
	      s_box[s][i], 
	      s_box[s][i+1],
	      s_box[s][i+2], 
	      s_box[s][i+3],
	      (i+4 < SBOX_WORDS) ? "," : " ", i, i+3);
	}
	printf("    }%s\n", (s+1 < MAX_SBOX_COUNT) ? "," : "");
    }
    puts("};");
    puts("");

    /*
     * print the S box array shifted right by 1, 2, and 3 bytes
     */
    puts("/*");
    puts(" * The shiftX_box arrays are standard S boxes that have been");
    puts(" * byte rotated X bytes to the right.");
    puts(" */");
    for (nbyte = 1; nbyte < sizeof(u32bit); ++nbyte) {
        printf("sbox shift%1d_box[MAX_SBOX_COUNT] = {\n", nbyte);
	for (s=0; s < MAX_SBOX_COUNT; ++s) {
	    printf("    /* right rotate %d byte%s S box #%d */\n", 
	      nbyte, (nbyte == 1) ? "" : "s", s);
	    for (i=0; i < SBOX_WORDS; i+= sizeof(u32bit)) {
		printf("%s0x%08x, 0x%08x, 0x%08x, 0x%08x%s /* %d-%d */\n",
		  (i==0) ? "    {   " : "\t",
		  ROTATE(s_box[s][i],nbyte*8), 
		  ROTATE(s_box[s][i+1],nbyte*8),
		  ROTATE(s_box[s][i+2],nbyte*8), 
		  ROTATE(s_box[s][i+3],nbyte*8),
		  (i+4 < SBOX_WORDS) ? "," : " ", i, i+3);
	    }
	    printf("    }%s\n", (s+1 < MAX_SBOX_COUNT) ? "," : "");
	}
        puts("};");
    }
    puts("");

    /*
     * print the end of the file
     */
    puts("/*");
    puts(" * rotated S Boxes");
    puts(" *");
    puts(" * The 32-bit word 's_box[i][j][k] is the standard S box word");
    puts(" * 's_box[j][k]' rotated right by 'i' bytes.");
    puts(" */");
    puts("sbox *sBoxes[] = {");
    puts("    s_box, shift1_box, shift2_box, shift3_box");
    puts("};");

    /* all done */
    exit(0);
}

/*
 * big_endian - return true if on a Big endian machine
 *
 * Big Endiam machines such as the 68k, Sparc, Amdahl number
 * their 4 byte values as: '1 2 3 4'.
 *
 * returns 1 if byte order is BIG ENDIAN, 0 otherwise
 */
int
big_endian()
{
	char *p4 = (char *)&byte4;	/* byte examination of byte4 */

	if (p4[0] == '\000' && p4[1] == '\001' && 
	    p4[2] == '\002' && p4[3] == '\003')
		return 1;
	return 0;
}

/*
 * convert - convert S Box Endian order
 */
void
convert(sboxset, num)
	sbox *sboxset;		/* pointer to S Box array */
	int num;		/* number of S boxes to convert */
{
    register u32bit boxEntry;	/* S Box entry */
    register u32bit *box;	/* S box entry being converted */
    register u32bit *boxEnd;	/* after end of S box being converted */
    int boxnum;			/* S Box set number to be converted */

    /* 
     * change one sbox at a time 
     */
    for (boxnum = 0; boxnum < num; ++boxnum) {

	/* 
	 * change byte order of an S Box 
	 */
	box = (u32bit *)&(sboxset[boxnum][0]); /* starting conversion point */
	boxEnd = box + SBOX_WORDS; /* after end of conversion point */
	for (boxEntry = *box; box < boxEnd; boxEntry = *(++box)) {
	    *box = SWAP(boxEntry);
	}
    }
}

/*
 * usage - print uesage message and abort
 */
void 
usage(exitval)
    int exitval;		/* how to exit */
{
    fprintf(stderr, "usage: %s [-b | -l | -s] > sbox.c\n", program);
    fprintf(stderr, "usage: %s [-b | -l | -s] -h > endian.h\n", program);
    exit(exitval);
}

/*
 * parse_args - parse command line arguments
 *
 * usage: prsbox [-l | -b | -s] [-h]
 *
 * XXX - should use getopt()
 */
void
parse_args(argc, argv)
    int argc;			/* arg count */
    char *argv[];		/* the args */
{
    /* note our name */
    program = argv[0];

    /* arg count check */
    if (argc > 3) {
	usage(1);
    }

    /* set defaults */
    big_end = big_endian();
    h_file = 0;

    /* check each arg */
    for (--argc, ++argv; argc > 0; --argc, ++argv) {

	/* check arg form: -?\0 */
	if (strlen(argv[0]) != 2 || argv[0][0] != '-') {
	    usage(2);
	}

	/* process arg char */
	switch (argv[0][1]) {
	case 'l':	/* Little Endian */
	    big_end = 0;
	    break;
	case 'b':	/* Big Endian */
	    big_end = 1;
	    break;
	case 's':	/* switch Endian type */
	    big_end = ! big_end;
	    break;
	case 'h':	/* print endian.h, not sbox.c */
	    h_file = 1;
	    break;
	default:	/* unknown arg */
	    usage(3);
	    break;
	}
    }
}

/*
 * print_endian_h - print the endian.h file
 */
void
print_endian_h()
{
    /*
     * print endian.h header
     */
    puts("/*");
    puts(" * WARNING: DO NOT EDIT THIS FILE DIRECTLY!!!");
    puts(" *");
    puts(" * This file was produced by the prsbox program, with -h");
    puts(" */");
    puts("");

    /*
     * print the LITTLE define
     */
    if (big_end) {
	puts("#undef LITTLE\t/* use on a Big Endian machine */");
    } else {
	puts("#define LITTLE\t/* use on a Little Endian machine */");
    }
}

/*
 * load the Standard S Box
 *
 * Given random numbers from the RAND book of numbers, construct
 * the `sbox' set for the standard S Box set.  See above the 's_box'
 * declaration for rules on how an `sbox' may be formed.
 */
void
mksbox()
{
    int boxnum;		/* sbox set number */
    int byte;		/* byte as bit number */
    int loc;		/* last location stored */ 
    u32bit val;		/* byte value to load */
    u32bit *box;	/* the sbox being built */

    /*
     * load each `sbox'
     */
    for (boxnum=0; boxnum < MAX_SBOX_COUNT; ++boxnum) {
	
	/*
	 * load each byte of a box
	 */
	box = &(s_box[boxnum][0]);
	for (byte=0; byte < 32; byte+=8) {
		
	    /*
	     * load each 8 bit value into an sbox word byte
	     *
	     * We know that initially, bytes will contain zeros.
	     * So we start selecting bytes to hold values from
	     * 1 to 255.  The pigeon hold theorm says that one
	     * byte will have a zero (because we loaded 255 out of
	     * 256 bytes).
	     *
	     * While selecting locations for the 1 thru 255
	     * locations, we will consider any zero byte to be
	     * available for use.  The random number generator
	     * will tell us how far to skip before we start
	     * looking for a free byte.  If the byte we come to
	     * is non-zero, we do a linear search until we
	     * find a zero byte.
	     */
	    for (loc=0, val=1; val < 256; ++val) {

		/* get the location */
		loc = (loc+randbyte())&0xff;

		/* search for an empty spot */
		while ((box[loc]&(0xff<<byte)) != 0) {
		    loc = (loc+1)&0xff;
		}

		/* plug the spot we found */
		box[loc] |= (val << byte);
	    }
	}
    }
}

/*
 * randbyte - return a random byte
 *
 * This function returns an unsigned 32 bit word with a value from
 * 0 thru 255.  We use the function randdigits() to return blocks
 * 7 decimal digits.  We convert these 7 digit blocks into 23 bits
 * and extract sets of 8 bits as needed.
 */
u32bit
randbyte()
{
    u32bit decimal;		/* random number between 0 and (10^7)-1 */
    u32bit val;			/* the value to return */
    static u32bit word=0;	/* random binary bits packed ready to use */
    static int bits=0;		/* number of bits in word */

    /*
     * add bits to word if it is low on bits
     */
    if (bits < 8) {
	/*
	 * get a random number between 0 and 2^23-1
	 */
	do {
	    decimal = randdigits();
	} while (decimal >= 0x800000);

	/*
	 * pack the 23 bits into the top of word
	 */
	word |= (decimal << bits);
	bits += 23;
    }

    /*
     * extract our 8 bit value from word
     */
    val = word & 0xff;
    word >>= 8;
    bits -= 8;
    return( val );
}

/*
 * randdigits - get 7 random digits
 *
 * This function returns 7 random digits from the RAND book of
 * numbers in the form of value between 0 and 10^7-1 inclusive.
 *
 * XXX - since we don't have the RAND book of numbers, fake
 *       them for now with the a pseudo random number generator.
 */
u32bit
randdigits()
{
    static int seeded=0;	/* 1 => random number was seeded */
    u32bit value;	/* the random value */

    /*
     * seed the generator if needed
     */
    if (seeded == 0) {
	srand(23209);
	seeded = 1;
    }

    /*
     * get a value between 0 and 10^7-1
     */
    do {
	value = (rand() >> 7)&0x7fffff;
	++rands;
    } while (value >= 10000000);

    /* return it */
    return( value );
}

/*
 * check - check to see if S Boxes are valid
 *
 * See rules above `s_box' definition.
 */
void
check()
{
    int boxnum;			/* sbox set number */
    int byte;			/* byte as bit number */
    int curcount;		/* the current tally count */
    u32bit loc;			/* location to look at */
    u32bit val;			/* byte value to look at */
    int tally[SBOX_WORDS];	/* 1 => not found, 0 => found */
    u32bit *box;		/* the sbox being built */

    /*
     * test each `sbox'
     */
    for (boxnum=0; boxnum < MAX_SBOX_COUNT; ++boxnum) {

	/*
	 * search each byte for values
	 */
	box = &(s_box[boxnum][0]);
	for (byte=0; byte < 32; byte+=8) {

	    /* clear tally */
	    for (loc=0; loc < 256; ++loc) {
		tally[loc] = 0;
	    }

	    /*
	     * tally each box entry
	     */
	    for (loc=0; loc < 256; ++loc) {
		tally[(box[loc]>>byte)&0xff]++;
	    }

	    /*
	     * check for tally problems
	     */
	    for (loc=0; loc < 256; ++loc) {
		if (tally[loc] == 0) {
		    fprintf(stderr, "%s: no val: %d for box %d byte %d\n",
		      program, loc, boxnum, byte/8);
		    exit(4);
		} else if (tally[loc] > 1) {
		    fprintf(stderr, "%s: dup val: %d for box %d byte %d\n",
		      program, loc, boxnum, byte/8);
		    exit(5);
		}
	    }
	}
    }
}
