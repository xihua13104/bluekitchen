#include <stddef.h>
#include <math.h>

#include "sieve.h"

enum { FALSE = 0, TRUE = 1 };

const char cstr1[] = "Constant String1";
char defaultstring[] = "test";
static int mstatic1, mstatic2, mcount;
static int period = 1000;
signed short int  plot1 = -15000, plot2 = 0;

float vfloat = 1.0;
double vdouble = 1.0;

typedef int tdef1;
tdef1 vtdef1;

typedef short tdef2[10];
tdef2 vtdef2;

struct vstruct1_s {
	int x, y;
}
vstruct1;

char vchar;
short vshort;
int vint = 1;
long vlong;

char * vpchar;
short * vpshort;
int * vpint;
long * vplong;

unsigned char vuchar;
unsigned short vushort;
unsigned int vuint;
unsigned long vulong;

unsigned char * vpuchar;
unsigned short * vpushort;
unsigned int * vpuint;
unsigned long * vpulong;

unsigned char * * vppuchar;
unsigned short * * vppushort;
unsigned int * * vppuint;
unsigned long * * vppulong;

unsigned long * * * vpppulong;
unsigned long * * * * vppppulong;

int varray[10];

typedef int tarray[10];
tarray vtarray;

char vdarray[5];
char vdiarray[] = "abcd";
char vdblarray[5][6];
char vtriplearray[2][3][4];

struct struct4 {
	struct struct5 * pstruct5;
	struct struct5 * * ppstruct5;
	struct struct5 * pastruct5[2];
};

struct struct5 {
	struct struct4 * pstruct4;
	struct struct4 * * ppstruct4;
	struct struct4 * pastruct4[2];
};


typedef struct struct1 {
	const char * word;
	int count;
	struct struct1 * left;
	struct struct1 * right;
	int field1:2;
	unsigned field2:3;
} strtype1 ;

strtype1 ast;
strtype1 pLinkedListBuf[5];

typedef struct struct1 structarray[10];

structarray stra1;

struct struct4 stra2[5][5];

struct union1 {
	char * word;
	int count;
	struct struct1 * left;
	struct struct1 * right;
	union {
		int ival;
		float fval;
		char * pval;
	} uval;
	int field1:2;
	unsigned field2:3;
} aun;

struct struct2 {
	char * word;
	int count;
	char name[10];
} str2;

struct struct6 {
	int x;
	struct struct7 {
		char * word;
		int count;
	} vstruct7;
	int y;
} str6;


struct abc {
	int x;
	int y;
};

struct abc def;

union tunion {
	int z1;
	short z2;
	char z3;
	struct abc z4;
} vunion;

enum enumtyp { enum1, enum2, enum4=4, enum7=7, enum8, enumx= -1 } enumvar;

int ( *funcptr ) ();

void func0()					/* empty function */
{
}

static void func1( int * intptr )		/* static function */
{
	(*intptr)++;
}

void func2()
{
	int autovar;
	register int regvar;
	static int fstatic = 44;	/* initialized static variable */
	static int fstatic2;		/* not initialized static variable */

	autovar = regvar = fstatic;
	autovar++;

	func1( &autovar );	/* to force autovar as stack-scope */
	func1( &fstatic );	/* to force fstatic as static-scope */

	for ( regvar = 0; regvar < 5 ; regvar++ )
		mstatic1 += regvar*autovar;

	fstatic += mstatic1;
	fstatic2 = 2*fstatic;

	if (mcount < 500)
		return;

	func1( &fstatic2 );

	return;
}

void func2a()
{
	char autovar;	/* char stack variable */
	register char regvar;	/* char register variable */

	autovar = regvar = (char) mstatic1;
	autovar++;

	for ( regvar = 0; regvar < (char) 5 ; regvar++ )
		vchar += regvar*autovar;
}

void func2b()
{
	long autovar;	/* long stack variable */
	register long regvar;	/* long register variable */

	autovar = regvar = mstatic1;
	autovar++;

	for ( regvar = 0; regvar < 5l ; regvar++ )
		vlong += regvar*autovar;
}

void func2d()
{
	short autovar;	/* short stack variable */
	register short regvar;	/* short register variable */

	autovar = regvar = mstatic1;
	autovar++;

	for ( regvar = 0; regvar < 5l ; regvar++ )
		vlong += regvar*autovar;
}


static int func3()			/* simple function */
{
	return 5;
}

struct struct1 func4( struct struct1 str )		/* function returning struct */
{
	str.count++;

	return str;
}

int func5( int a, char b, long c )			/* multiple arguments */
{
	return a+b*c;
}

struct bfield
{
	int a:1;
	int b:2;
	int c:3;
	int d:7;
	int e:8;
	int f:15;
	int g:1;
	int h:15;
	int i:1;
	int j:16;
	unsigned k:1;
	unsigned l:2;
	unsigned m:16;
	long n;
	char o;
	int p:15;
	char q:6;
	char r:1;
};

struct bfield vbfield;

void func8()
{
	vbfield.a = 1;
	vbfield.b = 1;
	vbfield.c = 1;
	vbfield.d = 1;
	vbfield.e = 1;
	vbfield.f = 1;
	vbfield.g = 1;
	vbfield.h = 1;
	vbfield.i = 1;
	vbfield.j = 1;
	vbfield.k = 1;
	vbfield.l = 1;
	vbfield.m = 1;
	vbfield.n = 1;
	vbfield.o = 1;
	vbfield.p = 1;
	vbfield.q = 1;
	vbfield.r = 1;

	vbfield.a = -1;
	vbfield.b = -1;
	vbfield.c = -1;
	vbfield.d = -1;
	vbfield.e = -1;
	vbfield.f = -1;
	vbfield.g = -1;
	vbfield.h = -1;
	vbfield.i = -1;
	vbfield.j = -1;
	vbfield.k = -1 & 0x01;
	vbfield.l = -1 & 0x03;
	vbfield.m = (unsigned short) -1;
	vbfield.n = -1;
	vbfield.o = -1;
	vbfield.p = -1;
	vbfield.q = -1;
	vbfield.r = -1;
}

static int * func9()			/* nested local variables */
{
	static int stat1 = 0;
	register int reg1;
	int auto1;

	auto1 = stat1;

	for ( reg1 = 0 ; reg1 < 2 ; reg1++ )
	{
		static int stat2 = 0;
		register int reg2;
		int auto2;

		auto2 = stat2;

		for ( reg2 = 0 ; reg2 < reg1 ; reg2++ )
		{
			func1( &stat1 );
			func1( &auto1 );
			func1( &stat2 );
			func1( &auto2 );
		}
	}

	return &stat1;
}

int func10()
{
	register int i;
	register int v1, v2, v3, v4, v5, v6, v7, v8;
	register int v9, v10, v11, v12, v13, v14, v15, v16, v17;

	v17 = 0;
	for ( i = 0 ; i < 3 ; i++ )
		v17 += i;

	for ( v1 = 0 ; v1 < 3 ; v1++ )
		v17 += v1;
	for ( v2 = 0 ; v2 < 3 ; v2++ )
		v17 += v2;
	for ( v3 = 0 ; v3 < 3 ; v3++ )
		v17 += v3;
	for ( v4 = 0 ; v4 < 3 ; v4++ )
		v17 += v4;
	for ( v5 = 0 ; v5 < 3 ; v5++ )
		v17 += v5;
	for ( v6 = 0 ; v6 < 3 ; v6++ )
		v17 += v6;
	for ( v7 = 0 ; v7 < 3 ; v7++ )
		v17 += v7;
	for ( v8 = 0 ; v8 < 3 ; v8++ )
		v17 += v8;
	for ( v9 = 0 ; v9 < 3 ; v9++ )
		v17 += v9;
	for ( v10 = 0 ; v10 < 3 ; v10++ )
		v17 += v10;
	for ( v11 = 0 ; v11 < 3 ; v11++ )
		v17 += v11;
	for ( v12 = 0 ; v12 < 3 ; v12++ )
		v17 += v12;
	for ( v13 = 0 ; v13 < 3 ; v13++ )
		v17 += v13;
	for ( v14 = 0 ; v14 < 3 ; v14++ )
		v17 += v14;
	for ( v15 = 0 ; v15 < 3 ; v15++ )
		v17 += v15;
	for ( v16 = 0 ; v16 < 3 ; v16++ )
		v17 += v16;

	i = v17;

	for ( v1 = 0 ; v1 < 3 ; v1++ )
		v17 += v1;
	for ( v2 = 0 ; v2 < 3 ; v2++ )
		v17 += v2;
	for ( v3 = 0 ; v3 < 3 ; v3++ )
		v17 += v3;
	for ( v4 = 0 ; v4 < 3 ; v4++ )
		v17 += v4;
	for ( v5 = 0 ; v5 < 3 ; v5++ )
		v17 += v5;
	for ( v6 = 0 ; v6 < 3 ; v6++ )
		v17 += v6;
	for ( v7 = 0 ; v7 < 3 ; v7++ )
		v17 += v7;
	for ( v8 = 0 ; v8 < 3 ; v8++ )
		v17 += v8;
	for ( v9 = 0 ; v9 < 3 ; v9++ )
		v17 += v9;
	for ( v10 = 0 ; v10 < 3 ; v10++ )
		v17 += v10;
	for ( v11 = 0 ; v11 < 3 ; v11++ )
		v17 += v11;
	for ( v12 = 0 ; v12 < 3 ; v12++ )
		v17 += v12;
	for ( v13 = 0 ; v13 < 3 ; v13++ )
		v17 += v13;
	for ( v14 = 0 ; v14 < 3 ; v14++ )
		v17 += v14;
	for ( v15 = 0 ; v15 < 3 ; v15++ )
		v17 += v15;
	for ( v16 = 0 ; v16 < 3 ; v16++ )
		v17 += v16;

	return v17;
}

int func11( int x )				/* multiple returns */
{
	switch ( x )
	{
		case 1:
			x = x+1;
			x = x*2;
			return x*x;
		case 2:
			return x+x;
		case 3:
			return x-x;
		case 4:
			x = x+1;
			x = x*2;
			return x*x;
		case 5:
			break;
		case 6:
			return x+x;
		default:
			break;
	}
	return x;
}

typedef char undefarray[];
typedef undefarray * undefptr;

void func12( undefptr ptr )		/* pointer to array of unknown size */
{
	(*ptr)[0] = 1;
}

int func13( int a, int c, int e )			/* arguments and locals stack-tracking */
{
	int b, d, f;

	b = a+c+e;
	f = b+a;
	d = f*b;

	if ( e > 0 )
		c += func13( b, f, e-1 );

	return c+e+d;
}

int func14( char x1 )		/* Parameter: 1 Byte */
{
	return 2*x1;
}

int func15( short x1 )		/* Parameter: 1 Word */
{
	return 2*x1;
}

int func16( long x1 )		/* Parameter: 1 Long */
{
	return (int) (2*x1);
}

int func17( short x1, short x2 )		/* Parameter: 2 Short */
{
	return x1*x2;
}

int func18( short x1, long x2 )		/* Parameter: Short,Long */
{
	return x1*x2;
}

int func19( long x1, short x2 )		/* Parameter: Long,Short */
{
	return x1*x2;
}

int func20( short x1, short x2, short x3 )	/* Parameter: 3 Short */
{
	return x1*x2*x3;
}

int func21( long x1, short x2, short x3 )	/* Parameter: Long,Short,Short */
{
	return x1*x2*x3;
}

int func22( short x1, long x2, short x3 )	/* Parameter: Short,Long,Short */
{
	return x1*x2*x3;
}

int func23( short x1, short x2, long x3 )	/* Parameter: Short,Short,Long */
{
	return x1*x2*x3;
}

char func24()			/* Char Return */
{
	return 55;
}

long func25()			/* Long Return */
{
	return 12345678l;
}

char * func26()			/* Pointer Return */
{
	static char x1[] = "Babel Fish";

	return x1;
}

struct struct1 func27( short x1, short x2 )		/* function returning struct */
{
	ast.count = x1*x2;

	return ast;
}

int int_sin(unsigned int x)
{
	int sign=1;
	if (x&0x20)
		sign=-1;
	x=x&0x1f;
	if ((x&0x10)&&(x&0xf))
		x=0x10-(x&0xf);

	// sin(x) -Pi/2 <= x <= Pi/2 = x-x^3/3!+x^5/5!-...
	return sign*((120*10*10*10*10*x - (20*10*10*x*x*x) + (x*x*x*x*x))>>9);
}

static short sinewave[630];

void func_sin()
{
	int x;
	
	for (x = 0; x < 628; x++) 
		sinewave[x] = int_sin(x)/(x/32+1);
}

static void initLinkedList()
{
	int i = 0;
	const int last = sizeof(pLinkedListBuf) / sizeof(pLinkedListBuf[0]) - 1;
	static const char* text[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

	ast.left = pLinkedListBuf;
	ast.right = NULL;
	ast.count = 0;
	ast.word = NULL;

	for (i = 0; i <= last; i++) {
		pLinkedListBuf[i].left  = (i==last) ? NULL : &pLinkedListBuf[i+1]; /* next element */
		pLinkedListBuf[i].right =    (i==0) ? &ast : &pLinkedListBuf[i-1]; /* previous element */
		pLinkedListBuf[i].count = i;
		pLinkedListBuf[i].word  = text[i % 7];
	}
}

static char subst( char c)
{
	switch ( c ){
		case 'a':
			return 'e';
		case 'e':
			return 'i';
		case 'i':
			return 'u';
		case 'o':
			return 'a';
		case 'u':
			return 'o';
	}
	return c;
}

static char* encode( char str[] )
{
	int i;

	for (i = 0; str[i]; i++){
		char c = str[i];
		if ('A' <= c  &&  c <= 'Z'){
			c = c + ('a'-'A');
			c = subst(c);
			c = c - ('a'-'A');
		} else {
			c = subst(c);
		}
		str[i] = c;
	}
	return str;
}


struct shortrecord
{
	short x;
	short y;
};

struct shortrecord vshortrecord;

struct shortrecord func28( struct shortrecord x )
{
	x.x++;
	x.y = (short) (x.x+1);
	return x;
}

union {
	unsigned char b[4];
	unsigned short w[2];
	unsigned long l[1];
} datas;

static int sieve(void);

void (*monHook)(void) __attribute__ ((section (".data"))) = 0;

int main(void)
{
	int        j;
	short int  inc, sign;
	char      *p;

	func_sin();

	do {
		if (monHook)
			monHook();

		mstatic1 = 12;
		mstatic2 = 34;
		mcount++;

		inc  = (4 * 15000) / period;
		sign = ( (mcount % period) > period/2 ) ?  -1 : +1;
		plot1 = plot1 + sign * inc;
		plot2 = 25000 * sign;

		vtriplearray[0][0][0] = 1;
		vtriplearray[1][0][0] = 2;
		vtriplearray[0][1][0] = 3;
		vtriplearray[0][0][1] = 4;

		func2();
		func2a();
		func2b();
		func2d();

		funcptr = (int (*) ()) 0;
		funcptr = func3;

		initLinkedList();
		ast.count = 12345;
		ast.field1 = 1 + mstatic2;
		ast.field2 = 2;

		ast = func4(ast);

		j = (*funcptr) ();

start:
		j = func5((int) j, (char) 2, (long) 3);

		if (j == 0)
			goto start;

		enumvar = enumx;

		func8();
		func9();
		func10();
		func11(5);
		func13(1, 2, 3);
		func14((char)  55);
		func15((short) 55);
		func16((long)  55);
		func17((short) 44, (short) 55);
		func18((short) 44, (long)  55);
		func19((long)  44, (short) 55);
		func20((short) 33, (short) 44, (short) 55);
		func21((long)  33, (short) 44, (short) 55);
		func22((short) 33, (long)  44, (short) 55);
		func23((short) 33, (short) 44, (long)  55);

		j = func24();
		vlong = func25();
		p = func26();
		vshortrecord = func28(vshortrecord);

		encode(p);
		sieve();

		datas.b[0] = 0x12;
		datas.b[1] = 0x12;
		datas.b[2] = 0x12;
		datas.b[3] = p[0];
		datas.w[0] = 0x1234;
		datas.w[1] = 0x1234;
		datas.l[0] = 0x12345678;
		datas.b[0] = 0x11;
		datas.b[1] = 0x11;
		datas.b[2] = 0x11;
		datas.b[3] = 0x11;
		datas.w[0] = 0x1122;
		datas.w[1] = 0x1122;
		datas.l[0] = 0x11223344;

	} while (vint);

	return 0;
}


#define SIZE 18
char flags[SIZE+1];

static int sieve(void)			/* sieve of erathostenes */
{
	register int    i, prime, k;
	int             count;

	count = 0;

	for ( i = 0 ; i <= SIZE ; flags[ i++ ] = TRUE ) ;

	for (i = 0; i <= SIZE; i++) {
		if (flags[i]) {
			prime = i + i + 3;
			k = i + prime;
			while (k <= SIZE) {
				flags[k] = FALSE;
				k += prime;
			}
			count++;
		}
	}

	return count;
}


int background(void)			/* job for background-demo */
{
	static long int bcnt1, bcnt2;

	bcnt1 = bcnt2 = 0;

	while (1) {
		bcnt1 = 10000000;
		while (bcnt1>0)
			bcnt1--;
		bcnt2++;
	}

	return 0;
}


