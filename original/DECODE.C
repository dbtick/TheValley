#include <stdio.h>
#include <ctype.h>
#include <string.h>

unsigned char Basic[4096];				/* 4k of Level 2 BASIC ROM */
unsigned char Valley[20480];			/* Valley program, Binary */
unsigned char DefList[32767];			/* Buffer for definitions etc. */
unsigned char *DefEnd;					/* End of Defs List */

void Comment(char *p);					/* Prototyping */
void Decrypt(int Tab,unsigned char *Code);
void ExtractElement(unsigned char **pp,int *Type,unsigned char *Buffer);
int RipToken(unsigned char Token,char *Buffer);
unsigned int LineNumber(char *p);
void AddDefinition(char *Line);
char *FindDefinition(char *Buffer);

#define ET_TOKEN	(1)					/* Basic object types */
#define ET_CHAR		(2)
#define ET_LABEL	(3)
#define ET_VARIABLE	(4)
#define ET_STRING	(5)
#define ET_END		(6)

void main()
{
	FILE *f;
	int i,n,Line,Step;
	char *p,Buffer[192];
	f = fopen("BASIC.ROM","rb");		/* Read in BASIC ROM */
	n = fread(Basic,1,sizeof(Basic),f);	/* Only the first bit */
	fclose(f);
	fprintf(stderr,"Read %d bytes of BASIC ROM.\n",n);
	f = fopen("VALLEY","rb");   		/* Read in the Valley program */
	n = fread(Valley,1,20480,f);
	fclose(f);
	fprintf(stderr,"Read %d bytes of \"The Valley\".\n",n);
	DefEnd = DefList;					/* Erase the definitions list */
	f = fopen("VALLEY.DEF","r");		/* Open defs file */
	while (fgets(Buffer,sizeof(Buffer),f) != NULL)
	{
		for (i = 0;i < strlen(Buffer);i++)/* Remove controls */
			if (Buffer[i] < ' ') Buffer[i] = ' ';
		while (*Buffer == ' ') strcpy(Buffer,Buffer+1);
		if (*Buffer != '\0' && *Buffer != ';')
									AddDefinition(Buffer);
	}
	fclose(f);							/* Close file */
	fprintf(stderr,"Read %d bytes of definitions.\n",DefEnd-DefList);
	n = 1;								/* Ignore initial $0D */
	do
	{
		Line = Valley[n+1]+				/* Get line number */
						Valley[n] * 256;
		Step = Valley[n+2];				/* Offset to next */
		if (Step != 0)					/* If not at end */
		{
			sprintf(Buffer,"%d",Line); 	/* Check for comment */
			p = FindDefinition(Buffer);
			if (p != NULL) Comment(p);
			printf("%-8d",Line);		/* Line number */
			Decrypt(8,Valley+n+3);		/* List program */
			printf("\n");
		}
		n = n + Step;   				/* Go to next line */
	} while (Step !=0);					/* Until all done */
}

void Decrypt(int Tab,unsigned char *Code)
{
	int First = 1,Type;
	char *p;
	unsigned char Buffer[128];
	while (*Code == ' ') Code++;		/* Skip leading spaces */
	while (*Code != 0x0D)				/* until EOL */
	{
		ExtractElement(&Code,			/* Rip a BASIC element */
						&Type,Buffer);
		switch(Type)					/* Display it */
		{
			case ET_TOKEN:				/* Token may have leading space */
				if (First == 0) printf(" ");
				printf("%s ",Buffer);
				break;
			case ET_STRING:				/* Quoted string */
				printf("%s",Buffer);
				break;
			case ET_VARIABLE:			/* Variable, may have substitution */
				p = FindDefinition(Buffer);
				printf("%s",(p == NULL) ? Buffer:p);
				break;
			case ET_LABEL:				/* Label, may have name */
				p = FindDefinition(Buffer);
				if (p != NULL)
					if (*p == '[')
					{
						p++;
						while (*p != ']') printf("%c",*p++);
						printf("@");
					}
				printf("%s",Buffer);
				break;
			case ET_CHAR:
				printf("%c",*Buffer);
				break;
			case ET_END:
				printf("\n%*s",Tab,"");
				break;
		}
	First = (Type == ET_END);			/* On new line, no leading space */
	}
}

void ExtractElement(unsigned char **pp,int *Type,unsigned char *Buffer)
{
	int n,c;
	if (**pp == 0x8D)					/* Special GOTO token */
	{
		sprintf(Buffer,"%u",			/* Extract it */
						LineNumber(*pp));
		*Type = ET_LABEL;
		(*pp) += 4;						/* Skip over it */
		return;
	}
	if (**pp & 0x80)					/* All other tokens */
	{
		*Type = ET_TOKEN;
		c = *(*pp)++;          			/* Get token number */
		n = RipToken(c,Buffer);			/* Get text from BASIC ROM */
		return;
	}

	if (**pp == ':')					/* Command seperator */
	{
		(*pp)++;
		*Type = ET_END;
		return;
	}

	if (isalpha(**pp))					/* Variable or something similar */
	{
		*Type = ET_VARIABLE;
		n = 0;
		while (isalnum(**pp) ||			/* Rip out the body */
					**pp == '$' || **pp == '%')
			Buffer[n++] = *(*pp)++;
		if (**pp == '(')				/* Array marker ? */
			Buffer[n++] = *(*pp)++;
		Buffer[n] ='\0';
		strlwr(Buffer);
		return;
	}

	if (**pp == '"')					/* Quoted string */
	{
		*Type = ET_STRING;
		n = 0;
		do
			Buffer[n++] = *(*pp)++;
		while (**pp != '"');
		Buffer[n++] = *(*pp)++;
		Buffer[n] ='\0';
		return;
	}

	*Type = ET_CHAR;					/* Char for everything else */
	*Buffer = *(*pp)++;
}

int RipToken(unsigned char Token,char *Buffer)
{
	int n = 0x71;						/* Where tokens start */
	int Len;

	while (n != 0x36D)					/* Where they end ! */
	{
		Len = 0;						/* How long is it */
		while (Basic[n+Len] < 0x80) Len++;
		if (Token == Basic[n+Len])		/* Tokens match ? Return string */
		{
			strncpy(Buffer,Basic+n,Len);
			Buffer[Len] = '\0';
			strlwr(Buffer);
			return 1;
		}
		n = n + Len + 2;				/* Next token */
	}
	return 0;							/* Failed */
}

unsigned int LineNumber(char *p)
{
	int n1,n2,n3,n;
	n1 = p[1];n2 = p[2];n3 = p[3];		/* I know it's bizarre..... */
	n = n2 - 0x40;
	switch(n1)
	{
		case 0x44: n += 64;break;
		case 0x74: n += 128;break;
		case 0x64: n += 192;break;
		case 0x60: n += 16576;break;
		case 0x40: n += 16448;break;
		case 0x70: n += 16512;break;
		case 0x50: n += 16384;break;
	}
	n = (n3 - 0x40) * 256 + n;
	return n;
}

void AddDefinition(char *Line)
{
	while (*Line != ' ' )				/* Copy the key */
			*DefEnd++ = *Line++;
	*DefEnd++ = '\0';					/* End of string */
	while (*Line == ' ') Line++;		/* Skip seperating spaces */
	strcpy(DefEnd,Line);				/* Copy the body */
	DefEnd = DefEnd+strlen(DefEnd)+1;	/* Reposition end pointer */
}

char *FindDefinition(char *Buffer)
{
	char *p = DefList;
	while (p != DefEnd)					/* While not at the end */
	{
		if (stricmp(Buffer,p) == 0)		/* Keys match, return string */
			return p + strlen(p) + 1;
		p = p + strlen(p)+1;			/* Skip key and body */
		p = p + strlen(p)+1;
	}
	return NULL;
}

void Comment(char *p)
{
	int n,Star = 0;
	if (*p == '[')						/* Remove label name */
			while (*p++ != ']') {};
	if (*p == '*') p++,Star++;			/* Check for highlight comment */
	n = 38-strlen(p)/2;
	if (Star) printf("// ***************************************************************************\n");
	printf("//\n//%*s%s\n//\n",n,"",p);
	if (Star) printf("// ***************************************************************************\n");
}