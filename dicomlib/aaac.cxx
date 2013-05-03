/*
MvH 19980327: Removed evaluation of Count without initialization in ReadDynamic 
mvh 20001106: Use memcpy instead of ByteCopy
ljz 20030122: Fixed initialization of AAssociateAC
mvh 20050108: Fixed for linux compile
mvh 20080203: Added experimental ConfigPadAEWithZeros
ljz 20080313: Removed some warnings
bcb 20100619: Fix gcc4 warnings and improve speed
mvh 20100717: Merged
mvh 20111010: Fix reading of accepted presentation contexts: array was ot cleared and eventually overflowed
bcb 20120820: Replaced strlen with the safer strnlen versions
bcb 20130226: Replaced gpps with IniValue class. Version to 1.4.18a.
bcb 20130328: Removed ConfigPadAEWithZeros and IniValue class references.
*/

/****************************************************************************
          Copyright (C) 1995, University of California, Davis

          THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND THE UNIVERSITY
          OF CALIFORNIA DOES NOT MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
          PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
          USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
          SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
          THE SOFTWARE IS WITH THE USER.

          Copyright of the software and supporting documentation is
          owned by the University of California, and free access
          is hereby granted as a license to use this software, copy this
          software and prepare derivative works based upon this software.
          However, any distribution of this software source code or
          supporting documentation or derivative works (source code and
          supporting documentation) must include this copyright notice.
****************************************************************************/

/***************************************************************************
 *
 * University of California, Davis
 * UCDMC DICOM Network Transport Libraries
 * Version 0.1 Beta
 *
 * Technical Contact: mhoskin@ucdavis.edu
 *
 ***************************************************************************/

#	include	"dicom.hpp"
#	include	"util.h"
#	include	<stdlib.h>

//#ifndef min
//#define min(a, b) ((a)<(b)?(a):(b))
//#endif

/************************************************************************
 *
 * Presentation Context Accept
 *
 ************************************************************************/

PresentationContextAccept	::	PresentationContextAccept()
#ifdef __GNUC__ //Faster with member initialization.
:ItemType(0x21),
Reserved1(0),
Length(0),
PresentationContextID(uniq8()),
Reserved2(0),
Result(2),
Reserved4(0),
TrnSyntax() {}
#else
	{
	ItemType = 0x21;
	Reserved1 = 0;
	PresentationContextID = uniq8();
	Reserved2 = 0;
	Result = 2;
	Reserved4 = 0;
	}
#endif

PresentationContextAccept	::	PresentationContextAccept(
	//AbstractSyntax	&Abs, 
	TransferSyntax	&Tran)
#ifdef __GNUC__ //Faster with member initialization.
:ItemType(0x21),
Reserved1(0),
Length(0),
PresentationContextID(uniq8()),
Reserved2(0),
Result(2),
Reserved4(0),
TrnSyntax(Tran) {}
#else
	{
	//AbsSyntax = Abs;
	TrnSyntax = Tran;
	ItemType = 0x21;
	Reserved1 = 0;
	PresentationContextID = uniq8();
	Reserved2 = 0;
	Result = 2;
	Reserved4 = 0;
	}
#endif

PresentationContextAccept	::	~PresentationContextAccept()
	{
	//
	}
/*
void	PresentationContextAccept	::	SetAbstractSyntax(AbstractSyntax	&Abs)
	{
	AbsSyntax = Abs;
	}
*/
void	PresentationContextAccept	::	SetTransferSyntax(TransferSyntax	&Tran)
	{
	TrnSyntax = Tran;
	}

BOOL	PresentationContextAccept	::	Write ( Buffer	&Link )
	{
	Link.Write((BYTE *) &ItemType, sizeof(BYTE));
	Link.Write((BYTE *) &Reserved1, sizeof(BYTE));	
	Link << Length;	//Link.Write((BYTE *) &Length, sizeof(UINT16));
	Link.Write((BYTE *) &PresentationContextID, sizeof(BYTE));
	Link.Write((BYTE *) &Reserved2, sizeof(BYTE));
	Link.Write((BYTE *) &Result, sizeof(BYTE));
	Link.Write((BYTE *) &Reserved4, sizeof(BYTE));
//	fprintf(stderr, "Writing Presentation Contex Accept: %d bytes\n", Length);
//	AbsSyntax.Write(Link);
	TrnSyntax.Write(Link);
//	fprintf(stderr, "ABS: %d TRN: %d\n", AbsSyntax.Size(), TrnSyntax.Size());


	Link.Flush();
	return ( TRUE );
	}

BOOL	PresentationContextAccept	::	Read (Buffer	&Link)
	{
	Link.Read((BYTE *) &ItemType, sizeof(BYTE));
	return ( this->ReadDynamic(Link) );
	}

BOOL	PresentationContextAccept	::	ReadDynamic (Buffer	&Link)
	{
//	INT32			Count;
	TransferSyntax	Tran;

	Link.Read((BYTE *) &Reserved1, sizeof(BYTE));
	Link >> Length;	//Link.Read((BYTE *) &Length, sizeof(UINT16));
	Link.Read((BYTE *) &PresentationContextID, sizeof(BYTE));
	Link.Read((BYTE *) &Reserved2, sizeof(BYTE));
	Link.Read((BYTE *) &Result, sizeof(BYTE));
	Link.Read((BYTE *) &Reserved4, sizeof(BYTE));

//	Count = Length - sizeof(BYTE) - sizeof(BYTE) - sizeof(BYTE) - sizeof(BYTE);
//	AbsSyntax.Read(Link);
//	Count = Count - AbsSyntax.Size();
	TrnSyntax.Read(Link);
//	Count = Count - TrnSyntax.Size();

//	if ( !Count)
		return ( TRUE );
//	return ( FALSE );
	}

UINT32	PresentationContextAccept	::	Size()
	{
	Length = sizeof(BYTE) + sizeof(BYTE) + sizeof(BYTE) + sizeof(BYTE);
//	Length += AbsSyntax.Size();
	Length += TrnSyntax.Size();
	return ( Length + sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT16));
	}

/************************************************************************
 *
 * AAssociateAC Packet
 *
 ************************************************************************/

AAssociateAC	::	AAssociateAC()//bcb? Called before IniValue inited
#ifdef __GNUC__ //Faster with member initialization.
:ItemType(0x02),
Reserved1(0),
Length(0),
ProtocolVersion(0x0001),
Reserved2(0),
AppContext(),
PresContextAccepts(),
UserInfo()
	{
#else
	{
	ItemType = 0x02;
	Reserved1 = 0;
	ProtocolVersion = 0x0001;
	Reserved2 = 0;
#endif
//            IniValue *iniValuePtr = IniValue::GetInstance();
	SpaceMem(CalledApTitle, 16);
//	if (iniValuePtr != NULL && iniValuePtr->sscscpPtr != NULL &&
//                iniValuePtr->sscscpPtr->ConfigPadAEWithZeros) memset(CalledApTitle, 0, 16);
	CalledApTitle[16] = 0;
	SpaceMem(CallingApTitle, 16);
//	if (iniValuePtr != NULL && iniValuePtr->sscscpPtr != NULL &&
//                iniValuePtr->sscscpPtr->ConfigPadAEWithZeros) memset(CalledApTitle, 0, 16);
	CallingApTitle[16] = 0;
	ZeroMem(Reserved3, 32);
	}

AAssociateAC	::	AAssociateAC(BYTE	*CallingAp, BYTE	*CalledAp)
#ifdef __GNUC__ //Faster with member initialization.
:ItemType(0x02),
Reserved1(0),
Length(0),
ProtocolVersion(0x0001),
Reserved2(0),
AppContext(),
PresContextAccepts(),
UserInfo()
	{
#else
	{
	ItemType = 0x02;
	Reserved1 = 0;
	ProtocolVersion = 0x0001;
	Reserved2 = 0;
#endif
//            IniValue *iniValuePtr = IniValue::GetInstance();
	SpaceMem(CalledApTitle, 16);
//	if (iniValuePtr->sscscpPtr->ConfigPadAEWithZeros) memset(CalledApTitle, 0, 16);
	CalledApTitle[16] = 0;
	SpaceMem(CallingApTitle, 16);
//	if (iniValuePtr->sscscpPtr->ConfigPadAEWithZeros) memset(CalledApTitle, 0, 16);
	CallingApTitle[16] = 0;
	ZeroMem(Reserved3, 32);
	memcpy(CallingApTitle, CallingAp, strnlen((char *)CallingAp, 16));// min(strlen((char *)CallingAp), 16u));
	memcpy(CalledApTitle, CalledAp, strnlen((char *)CalledAp, 16));// min(strlen((char *)CalledAp), 16u));
	}

AAssociateAC	::	~AAssociateAC()
	{
	// nothing, everything should self-destruct nicely
	}

void	AAssociateAC	::	SetCalledApTitle(BYTE	*CalledAp)
	{
	SpaceMem(CalledApTitle, 16);
//	if (IniValue::GetInstance()->sscscpPtr->ConfigPadAEWithZeros) memset(CalledApTitle, 0, 16);
	memcpy(CalledApTitle, CalledAp, strnlen((char *)CalledAp, 16));// min(strlen((char *)CalledAp), 16u));
	}

void	AAssociateAC	::	SetCallingApTitle(BYTE	*CallingAp)
	{
	SpaceMem(CallingApTitle, 16);
//	if (IniValue::GetInstance()->sscscpPtr->ConfigPadAEWithZeros) memset(CalledApTitle, 0, 16);
	memcpy(CallingApTitle, CallingAp, strnlen((char *)CallingAp, 16));// min(strlen((char *)CallingAp), 16u));
	}

void	AAssociateAC	::	SetApplicationContext(ApplicationContext	&AppC)
	{
	AppContext = AppC;
	}

void	AAssociateAC	::	AddPresentationContextAccept(PresentationContextAccept	&PresContextAccept)
	{
	PresContextAccepts.Add(PresContextAccept);
	}

void	AAssociateAC	::	SetUserInformation(UserInformation	&User)
	{
	UserInfo = User;
	}

BOOL	AAssociateAC	::	Write(Buffer	&Link)
	{
	UINT	Index;

//	fprintf(stderr, "AAssociateAC :: Write ()\n");fflush(stderr);
	Size();
	Link << ItemType; //Link.Write((BYTE *) &ItemType, sizeof(BYTE));
	Link.Write((BYTE *) &Reserved1, sizeof(BYTE));
	Link << Length;	//Link.Write((BYTE *) &Length, sizeof(UINT32));
	Link << ProtocolVersion;	//Link.Write((BYTE *) &ProtocolVersion, sizeof(UINT16));
	Link << Reserved2;			//Link.Write((BYTE *) &Reserved2, sizeof(UINT16));
	Link.Write((BYTE *) CalledApTitle, 16);
	Link.Write((BYTE *) CallingApTitle, 16);
	Link.Write((BYTE *) Reserved3, 32);
	Link.Flush();
//	fprintf(stderr, "AAssociateAC (writting App/Pre Contexts)\n");
	AppContext.Write(Link);
	Index = 0;
	while(Index < PresContextAccepts.GetSize())
		{
		PresContextAccepts[Index].Write(Link);
		++Index;
		}
//	fprintf(stderr, "AAssociateAC ( writting User info)\n");
	UserInfo.Write(Link);
	return ( TRUE );
	}

BOOL	AAssociateAC	::	Read(Buffer	&Link)
	{
	Link.Read((BYTE *) &ItemType, sizeof(BYTE));
	return(this->ReadDynamic(Link));
	}

BOOL	AAssociateAC	::	ReadDynamic(Buffer	&Link)
	{
	INT	Count;
	BYTE	TempByte;
	PresentationContextAccept	PresContextAccept;

	// mvh 20111010: Fix reading of accepted presentation contexts: array was ot cleared and eventually overflowed
	while(PresContextAccepts.GetSize()) PresContextAccepts.RemoveAt(0);

	Link.Read((BYTE *) &Reserved1, sizeof(BYTE));
	Link >> Length;	//Link.Read((BYTE *) &Length, sizeof(UINT32));
	Link >> ProtocolVersion;	//Link.Read((BYTE *) &ProtocolVersion, sizeof(UINT16));
	Link >>	Reserved2;			//Link.Read((BYTE *) &Reserved2, sizeof(UINT16));
	Link.Read((BYTE *) CalledApTitle, 16);
	Link.Read((BYTE *) CallingApTitle, 16);
	Link.Read((BYTE *) Reserved3, 32);

	CalledApTitle[16] = '\0';
	CallingApTitle[16] = '\0';

	Count = Length - sizeof(UINT16) - sizeof(UINT16) - 16 - 16 - 32;
	while(Count > 0)
		{
		Link.Read((BYTE *) &TempByte, sizeof(BYTE));
		switch(TempByte)
			{
			case	0x50:	// user information
				UserInfo.ReadDynamic(Link);
				Count = Count - UserInfo.Size();
				break;
			case	0x21:
				PresContextAccept.ReadDynamic(Link);
				Count = Count - PresContextAccept.Size();
				PresContextAccepts.Add(PresContextAccept);
				break;
			case	0x10:
				AppContext.ReadDynamic(Link);
				Count = Count - AppContext.Size();
				break;
			default:
				Link.Kill(Count-1);
				Count = -1;
			}
		}
	if(!Count)
		return ( TRUE );
	return ( FALSE);
	}

UINT32	AAssociateAC	::	Size()
	{
	UINT	Index;

	Length = sizeof(UINT16) + sizeof(UINT16) + 16 + 16 + 32;
	Length += AppContext.Size();
	Index = 0;

	Index = 0;
	while(Index < PresContextAccepts.GetSize())
		{
		Length += PresContextAccepts[Index].Size();
		++Index;
		}
	Length += UserInfo.Size();
	return ( Length + sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT32) );
	}

