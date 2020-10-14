#ifndef lint
	static char SCCSID[] = "@(#)printf.c	1.2 95/03/28 1.2 07:48:35";
#endif

/* @(#)printf.c 1.0 92/07/27; by Richard "Rick" Hernandez */

/*
** This is a shell printf that handles all normal 'C' printf formatting
** instructions.
** 
** This program may be used and distributed by anyone only include 
** this header. For questions or improvements:
** E-mail:
**	 richard.hernandez@torreypinesca.ncr.com
** US MAIL:
**	AT&T/NCR Division
**	Torrey Pines Dev.Ctr.(TPDC)
**	11010 Torreyana Rd.
**	San Diego, CA 92121
**	Att: Rick Hernandez
**
** Enhancements:
**	Will handle HEX in the format string to convert to char with the
**	addition of '\0x' example printf '\0xa' prints a carriage return
**	and line feed. Will only understand upto the first two hex digits.
**
** Bugs:
**	will not handle larger than long integer for integer input
**	will not handle larger than double for float input
**	works like printf because it is printf
**
** Notes:
**	1.	Does handle exponents as arguments to the format command (i.e. 1.3e-10).
**	2.	Tested on:
**			NCR 3300 running NCR SVR4 1.0.2
**			Sun Microsystems running SunOS 4.1.1
**		No changes were required.
*/

#include <stdio.h>
#include <ctype.h>
	
#define NO 0
#define YES 1
#define OCTAL 'o'
#define HEX 'x'
#define DECIMAL 'D'

long atol();
double atof();

static char *format;

static long mkarg (arg,incr,arg_input_type,cnt) /* convert ascii to number */
char *arg;
int incr;
char arg_input_type; /* determine number parsing algorithm */
int cnt;
{
	long j,tmp=0,addon=0;
		/* decision matrix for argument type */
	if (arg_input_type == OCTAL && *arg != '\\') arg_input_type=DECIMAL;
	if (arg_input_type == DECIMAL && *arg == '\\') arg_input_type=OCTAL;
	if (*arg == '\\') arg++;
	if (*arg == '0' && (*(arg+1) == 'x' || *(arg+1) == 'X')) {
		arg_input_type=HEX;					/* addon takes into account */
		arg+=2;								/* incrementing from main loop */
		if (incr) addon=cnt=2;				/* cnt is set for hex number */
	} else if (arg_input_type != OCTAL) arg_input_type=DECIMAL;

	switch(arg_input_type) {
	case HEX:
		for (j=0; j<cnt && isxdigit(*arg); j++, arg++)
			tmp=(tmp<<4)+*arg-(isdigit(*arg)?'0':(isupper(*arg)?'A':'a')-10);
		break;
	case OCTAL:
		for (j=0;j<cnt && *arg >= '0' && *arg < '8';j++,arg++) {
			tmp=(tmp<<3) + *arg - '0';
		}
		break;
	case DECIMAL:
		tmp=atol(arg);
	}								/*If called from format parsing loop */
	if (incr)  format+=j+addon;		/* increment format pointer */

	return(tmp);
}

static void printarg(type,arg,fmt) /* print formatted output */
char type;
char *arg;
char *fmt;
{
	char c;

	switch(type) {
	case 's':
		(void) printf(fmt,arg);
		break;
	case 'c':
		if (arg[0] == '\\') c=mkarg(arg,NO,OCTAL,3);
		else c=arg[0];
		(void) printf(fmt,c);
		break;
	case 'd':
		(void) printf(fmt,mkarg(arg,NO,DECIMAL,10));
		break;
	case OCTAL:
		(void) printf(fmt,mkarg(arg,NO,OCTAL,10));
		break;
	case HEX: case 'X':
		(void) printf(fmt,mkarg(arg,NO,HEX,8));
		break;
	case 'f':
		(void) printf(fmt,atof(arg));
		break;
	}
}

int main(argc,argv)
int argc;
char **argv;
{
	int i,j;
	char type,ch;
	char buf[100];
	char **dollar;

	if(argc<2) return(1);

	for (dollar=++argv,format=*argv++; *format!='\0' ;format++)
		switch(ch=*format) {
		case '%': /* concatenate all formatting instructions */
			for (buf[0]=*format,format++,i=1; i<10 ;i++,format++) {
				switch(buf[i]=*format) {
				case 's':case 'c':case 'd':case 'u':case 'o':case 'x':case 'X':
					type=*format;
					goto PRTARG;
				case 'f': case 'e': case 'E': case 'g': case 'G':
					type='f';
					goto PRTARG;
				case '*': /* replace '*' with string argument */
					for (j=0;(buf[i]=*(*argv+j)) != '\0';j++,i++);
					--i;
					argv++;
					break;
				case '$': /* print the specified dollar arg and goto done */
					for (;*format != '%';format--);
					for (j=0,format++;*format!='$';j++,format++) buf[j]=*format;
					buf[j]='\0';
					printarg('s',*(dollar+atoi(buf)),"%s");
					if(*++format != 's') format--; /* all this for a dollar */
					argv++;
					goto DONE;
				case '%':
					(void) putchar('%');
					goto DONE;
				}
			}
PRTARG:		buf[i+1]='\0'; /* make format buffer a string */
			printarg(type,*argv++,buf);
DONE:		break;
		case '\\': /* handle escape sequences in format string */
			switch(*++format) {
			case 'n':
				ch='\n';
				break;
			case 't':
				ch='\t';
				break;
			case 'v':
				ch='\v';
				break;
			case 'b':
				ch='\b';
				break;
			case 'r':
				ch='\r';
				break;
			case 'f':
				ch='\f';
				break;
			case '\\':
				ch='\\';
				break;
			case '0': case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': /* the partial format string is the argument */
				ch=mkarg(--format,YES,DECIMAL,3);
				break;
			default:
				ch=*format;
			} /* Fall thru and print character */
		default:
			(void) putchar(ch);
		}
	return(0);
}
