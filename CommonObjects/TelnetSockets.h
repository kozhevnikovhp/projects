// File TelnetSockets.h - interfaces for telnet sockets
// Created:	February 03, 2003
// Author:	Evgeny Kozhevnikov, Zelenograd, Russia E-mail: ek538@mail.ru
 
#ifndef __TELNET_SOCKETS_H_INCLUDED__
#define __TELNET_SOCKETS_H_INCLUDED__

#include "TcpSockets.h"

// Telnet command codes:
#define	CMD_EOF		(unsigned char)236		/* End of file: EOF is already used... */
#define	CMD_SUSP	(unsigned char)237		/* Suspend process */
#define	CMD_ABORT	(unsigned char)238		/* Abort process */
#define CMD_EOR     (unsigned char)239		/* end of record (transparent mode) */
#define CMD_SE		(unsigned char)240		// End of Subnegotiation
#define	CMD_NOP		(unsigned char)241		/* nop */
#define	CMD_DM		(unsigned char)242		/* data mark--for connect. cleaning */
#define	CMD_BREAK	(unsigned char)243		/* break */
#define	CMD_IP		(unsigned char)244		/* interrupt process--permanently */
#define	CMD_AO		(unsigned char)245		/* abort output--but let prog finish */
#define	CMD_AYT		(unsigned char)246		/* are you there */
#define	CMD_EC		(unsigned char)247		/* erase the current character */
#define	CMD_EL		(unsigned char)248		/* erase the current line */
#define	CMD_GA		(unsigned char)249		// you may reverse the line
#define CMD_SB		(unsigned char)250		// Start Subnegotiation
#define CMD_WILL	(unsigned char)251		// Согласен использовать
#define CMD_WONT	(unsigned char)252		// Согласен не использовать
#define CMD_DO		(unsigned char)253		// Давай будем ...
#define CMD_DONT	(unsigned char)254		// Давай не будем ...
#define CMD_IAC 	(unsigned char)255		// Interpret As Command


#define SYNCH	(unsigned char)242		/* for telfunc calls */

// Telnet option codes:
#define OPT_BINARY	(unsigned char)0		// TRANSMIT-BINARY option
#define OPT_ECHO	(unsigned char)1		// ECHO option
#define	OPT_RCP		(unsigned char)2		/* prepare to reconnect */
#define OPT_SGA		(unsigned char)3		// Suppress Go-Ahead option
#define	OPT_NAMS	(unsigned char)4		/* approximate message size */
#define	OPT_STATUS	(unsigned char)5		/* give status */
#define	OPT_TM		(unsigned char)6		/* timing mark */
#define	OPT_RCTE	(unsigned char)7		/* remote controlled transmission and echo */
#define OPT_NAOL 	(unsigned char)8		/* negotiate about output line width */
#define OPT_NAOP 	(unsigned char)9		/* negotiate about output page size */
#define OPT_NAOCRD	(unsigned char)10		/* negotiate about CR disposition */
#define OPT_NAOHTS	(unsigned char)11		/* negotiate about horizontal tabstops */
#define OPT_NAOHTD	(unsigned char)12		/* negotiate about horizontal tab disposition */
#define OPT_NAOFFD	(unsigned char)13		/* negotiate about formfeed disposition */
#define OPT_NAOVTS	(unsigned char)14		/* negotiate about vertical tab stops */
#define OPT_NAOVTD	(unsigned char)15		/* negotiate about vertical tab disposition */
#define OPT_NAOLFD	(unsigned char)16		/* negotiate about output LF disposition */
#define OPT_XASCII	(unsigned char)17		/* extended ascic character set */
#define	OPT_LOGOUT	(unsigned char)18		/* force logout */
#define	OPT_BM		(unsigned char)19		/* byte macro */
#define	OPT_DET		(unsigned char)20		/* data entry terminal */
#define	OPT_SUPDUP	(unsigned char)21		/* supdup protocol */
#define	OPT_SUPDUPOUTPUT (unsigned char)22	/* supdup output */
#define	OPT_SNDLOC	(unsigned char)23		/* send location */
#define OPT_TTTYPE	(unsigned char)24		// Terminal-Type option
#define	OPT_EOR		(unsigned char)25		/* end or record */
#define	OPT_TUID	(unsigned char)26		/* TACACS user identification */
#define	OPT_OUTMRK	(unsigned char)27		/* output marking */
#define	OPT_TTYLOC	(unsigned char)28		/* terminal location number */
#define	OPT_REGIME3270 (unsigned char)29	/* 3270 regime */
#define	OPT_X3PAD	(unsigned char)30		/* X.3 PAD */
#define OPT_NAWS	(unsigned char)31		/* window size */
#define	OPT_TSPEED	(unsigned char)32		/* terminal speed */
#define	OPT_LFLOW	(unsigned char)33		/* remote flow control */
#define OPT_LINEMODE	(unsigned char)34	/* Linemode option */
#define OPT_XDISPLOC	(unsigned char)35	/* X Display Location */
#define OPT_ENVIRON	(unsigned char)36		/* Old - Environment variables */
#define	OPT_AUTHENTICATION (unsigned char)37 /* Authenticate */
#define	OPT_ENCRYPT	(unsigned char)38		/* Encryption option */
#define OPT_NEW_ENVIRON (unsigned char)39	/* New - Environment variables */
#define	OPT_EXOPL	(unsigned char)255		/* extended-options-list */


// Option subnegotiation constants:
#define SUB_IS	 0		// IS command
#define SUB_SEND 1		// SEND command

// Telnet socket-imput FSM States:
#define TS_DATA			(unsigned char)0	// Normal data processing
#define TS_CR			(unsigned char)2
#define TS_IAC			(unsigned char)3	// Мы в IAC состоянии
#define TS_DO			CMD_DO
#define TS_DONT			CMD_DONT
#define TS_WILL			CMD_WILL
#define TS_WONT			CMD_WONT
#define TS_SB			CMD_SB
#define TS_SEND_NAWS 	(unsigned char)5
#define TS_WAIT_SE 		(unsigned char)6
#define TS_WAIT_SE1 	(unsigned char)7
#define TS_SB_TERM		(unsigned char)8
#define TS_SEND_TERM	(unsigned char)9

#define ASCII_MODE      0
#define BIN_MODE        1
#define REQ_BIN_MODE    2
#define REQ_ASCII_MODE	3

// CTelnetSocket - base socket class for Telnet clients

class CTelnetSocket : public CTcpSocket
{
//Attributes
public:
	CTelnetSocket();
	virtual ~CTelnetSocket();
// Public methods
	virtual BOOL Read(char *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes);
// Public overridables
// Public members

protected:
// Protected methods
	void SendSymbol(unsigned char symbol);
	void SendDO(unsigned char opt);
	void SendDONT(unsigned char opt);
	void SendWILL(unsigned char opt);
	void SendWONT(unsigned char opt);
	void SendNaws();
	void SendTerminalType();
	void TelnetBinary();
	void Flush();
	void SymbolReceived(char symbol);
// Protected overridables
	virtual unsigned short GetDefaultPort() { return 23; } // 23 is well-known telnet port
	virtual void OnConnected();
	virtual void OnSymbolReceived(char symbol);
	virtual void OnSymbolToUser(char symbol);
	virtual BOOL IsProcessedAsControl(char symbol) { return FALSE; }
	virtual char *GetTerminalType() { return "ttnnn"; }
// Protected members
	unsigned char m_pszOutputBuffer[2048];
	int m_nCurrentOutputPos;
	unsigned int m_UserSymbols;
	unsigned char state;	// Текущее FSM состояние
	char status;
	unsigned ibin_mode:2;
	unsigned hbin_mode:2;
	unsigned will_sga:1;
	unsigned do_sga:1;
	unsigned tt_send:1;
	unsigned naws_send:1;
	unsigned do_echo:1;
private:
// Private methods
	void ReceivedInDataState(unsigned char c);
	void ReceivedInCrState(unsigned char c);
	void ReceivedInIacState(unsigned char c);
	void ReceivedInDoState(unsigned char c);
	void ReceivedInDontState(unsigned char c);
	void ReceivedInWillState(unsigned char c);
	void ReceivedInWontState(unsigned char c);
	void ReceivedInSbState(unsigned char c);
	void ReceivedInWaitSeState(unsigned char c);
	void ReceivedInWaitSe1State(unsigned char c);
	void ReceivedInSbTermState(unsigned char c);
	void ReceivedInSendTermState(unsigned char c);
	void ReceivedInSendNawsState(unsigned char c);
// Private overridables
// Private members
};

typedef enum {
    Esc_None,
    Esc_code,
    CSI_code,
    DCS_code,
    SCO_Escape,
    DEC_Escape,
    GANS_Escape,
    Failed_CSI,
    Bad_DCS,
    KeyDefine,
    DecKeyDefine,
    Print_Controller,
    Fill_Buffer,
    GetPrintFilename,
} ESCSTATE;

#define MAX_INTERMEDIATE	2
#define MAX_CSI_PAR			5

class CAnsiTelnetSocket : public CTelnetSocket
{
//Attributes
public:
	CAnsiTelnetSocket();
	virtual ~CAnsiTelnetSocket();
// Public methods
// Public overridables
// Public members

protected:
// Protected methods
	BOOL IsControlSymbol(char symbol);
// Protected overridables
	virtual BOOL IsProcessedAsControl(char symbol);
	virtual char *GetTerminalType() { return "ansi"; }
	virtual void OnBell() {}
	virtual void OnBackspace() {}
	virtual void OnHorizontalTab() {}
	virtual void OnLineFeed() {}
	virtual void OnVerticalTab() {}
	virtual void OnFormFeed() {}
	virtual void OnCarriageReturn() {}
	virtual void OnStoreCursorPos() {}
	virtual void OnRestoreCursorPos() {}
	virtual void OnNextLine() {}
	virtual void OnTabstopSet() {}
// Protected members
	ESCSTATE m_EscapeState;
    unsigned char intermediate[MAX_INTERMEDIATE];
    unsigned int CSI_par[MAX_CSI_PAR];
    unsigned char nCSI_par;
    unsigned int CSI_currpar;
private:
// Private methods
	BOOL ControlSymbolInNoneState(char symbol);
	BOOL ControlSymbolInEscapeState(char symbol);
	BOOL ControlSymbolInSpecialState(char symbol);
// Private overridables
// Private members
};

#endif // __TELNET_SOCKETS_H_INCLUDED__