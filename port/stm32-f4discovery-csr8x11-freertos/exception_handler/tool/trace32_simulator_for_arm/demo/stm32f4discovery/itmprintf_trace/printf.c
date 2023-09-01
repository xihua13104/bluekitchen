#include <stdarg.h>

static volatile unsigned int  *g_itmBase = (volatile unsigned int *)0xE0000000;
static unsigned int count=0;

#define ITM_ENABLE_ACCESS  { g_itmBase[0x3EC]=0xC5ACCE55; }
#define ITM_TRACE_PRIV     g_itmBase[0x390]
#define ITM_TRACE_ENABLE   g_itmBase[0x380]

static volatile unsigned int *ITM_BASE = (volatile unsigned int *)0xE0000000;
#define ITM_TRACE_D8(_channel_,_data_) { \
	volatile unsigned int *_ch_=ITM_BASE+(_channel_); \
	while ( *_ch_ == 0); \
	(*((volatile unsigned char *)(_ch_)))=(_data_); \
}

#define ITM_TRACE_D16(_channel_,_data_) { \
	volatile unsigned int *_ch_=ITM_BASE+(_channel_); \
	while ( *_ch_ == 0); \
	(*((volatile unsigned short *)(_ch_)))=(_data_); \
}

#define ITM_TRACE_D32(_channel_,_data_) { \
	volatile unsigned int *_ch_=ITM_BASE+(_channel_); \
	while ( *_ch_ == 0); \
	*_ch_ = (_data_); \
}

extern int vsprintf(char *buf, const char *fmt, va_list args);

void ITM_printf(const char *format,...)
{
	union {
		char c[100];
		unsigned int i[25];
	} line;
	unsigned int v;
	int i,j,l;
	va_list ap;

	va_start(ap, format);
	l=vsprintf(&(line.c[0]),format,ap);
	l++;
	va_end(ap);
	i=0;j=0;
	while (i<l)
	{
		v=line.i[j];
		i+=4;
		j++;
		if (i>l)
			v&=(0xFFFFFFFF>>((i-l)*8));
		ITM_TRACE_D32(0,v);
	}
}

int wait(int k)
{
	int i;
	int j=0;

	for (i=1;i<k;i++)
		j+=i;

	return j;
}

int main()
{

	ITM_ENABLE_ACCESS;
	ITM_TRACE_PRIV=0;
	ITM_TRACE_ENABLE=0xFFFFFFFF;

	while(1)
	{
		wait(100000);
		ITM_printf("count = %08x", count);
		count++;
	}
	return 0;
}
