// File TelnetSockets.cpp - Telnet sockets implementation
// Created:	February 03, 2003
// Author:	Evgeny Kozhevnikov, Zelenograd, Russia E-mail: ek538@mail.ru
 
#include "stdafx.h"
#include "TelnetSockets.h"

// Class CTelnetSocket implementation

const unsigned char BEL = (unsigned char)'\007';	//  BEL (Bell)
const unsigned char BS = (unsigned char)'\010';		//  BS (Backspace)
const unsigned char HT = (unsigned char)'\011';		//  HT (Horizontal Tab)
const unsigned char LF = (unsigned char)'\012';		//  LF (Line Feed)
const unsigned char VT = (unsigned char)'\013';		//  VT (Vertical Tab)
const unsigned char FF = (unsigned char)'\014';		//  FF (Form Feed)
const unsigned char CR = (unsigned char)'\015';		//  CR (Carriage Return)
const unsigned char ESC = (unsigned char)'\033';	//	ESC
const unsigned char CSI = (unsigned char)'\233';	//	Control Sequence Introducer ( ESC [ )

static char szCRLF[]={CR, LF, 0};

CTelnetSocket::CTelnetSocket()
{
    will_sga = do_sga = 0;
    tt_send = 0;
    naws_send = 0;
    do_echo = 0;
	m_nCurrentOutputPos = 0;
    state = TS_DATA;		// ��������� ��������� - �������� ������.
    status = 0;				//
}

//virtual
CTelnetSocket::~CTelnetSocket()
{
	TRACE("CTelnetSocket object is destructed\n");
}

//virtual
BOOL CTelnetSocket::Read(char *pBufferToFill, unsigned int nBufferSize, unsigned int &nReadBytes)
{
	m_UserSymbols = 0;
	nReadBytes = 0;
	char cThisSymbol;
	unsigned int count;
	do
	{
		BOOL bSuccess = CTcpSocket::Read(&cThisSymbol, 1, count);
		if (!bSuccess)
			return FALSE;
		OnSymbolReceived(cThisSymbol);
		nReadBytes = m_UserSymbols;
		if (nReadBytes > 0)
			pBufferToFill[nReadBytes-1] = cThisSymbol;
	} while (nReadBytes < nBufferSize);
	return TRUE;
}

// ����� �������� � �������� �����?
void CTelnetSocket::TelnetBinary()
{
    SendDO(OPT_BINARY);
    ibin_mode = REQ_BIN_MODE;
    SendWILL(OPT_BINARY);
    hbin_mode = REQ_BIN_MODE;
}

void CTelnetSocket::SendSymbol(unsigned char symbol)
{
	if (m_nCurrentOutputPos >= sizeof(m_pszOutputBuffer)/sizeof(m_pszOutputBuffer[0]))
		Flush();
	m_pszOutputBuffer[m_nCurrentOutputPos++] = symbol;
}

void CTelnetSocket::SendDO(unsigned char opt)
{
	SendSymbol(CMD_IAC);
	SendSymbol(CMD_DO);
	SendSymbol(opt);
	Flush();
}

void CTelnetSocket::SendDONT(unsigned char opt)
{
	SendSymbol(CMD_IAC);
	SendSymbol(CMD_DONT);
	SendSymbol(opt);
	Flush();
}

void CTelnetSocket::SendWILL(unsigned char opt)
{
	SendSymbol(CMD_IAC);
	SendSymbol(CMD_WILL);
	SendSymbol(opt);
	Flush();
}

void CTelnetSocket::SendWONT(unsigned char opt)
{
	SendSymbol(CMD_IAC);
	SendSymbol(CMD_WONT);
	SendSymbol(opt);
	Flush();
}

// �������� ��������� ������ ...
void CTelnetSocket::SendNaws()
{
    int col = 80, row = 25;
    unsigned char c;

/*	col = tn.display_size[0];
	row = tn.display_size[1];
    if (col < 79 || row < 24 )
		return;*/
    SendSymbol(CMD_IAC);
    SendSymbol(CMD_SB);
    SendSymbol(OPT_NAWS);
    c = (col >> 8) & 0xFF; // ������� �������� ������� ���� ...
    SendSymbol(c);
    if (c == CMD_IAC)
		SendSymbol(c);
    c = col  & 0xFF; // � ����� �������.
    SendSymbol(c);
    if (c == CMD_IAC)
		SendSymbol(c);
    c = (row >> 8) & 0xFF; // ������� �������� ������� ���� ...
	SendSymbol(c);
	if (c == CMD_IAC)
		SendSymbol(c);
    c = row  & 0xFF; // � ����� �������.
    SendSymbol(c);
    if (c == CMD_IAC)
		SendSymbol(c);
    SendSymbol(CMD_IAC);
    SendSymbol(CMD_SE);
	Flush();
}

// �������� ��� ���������.
void CTelnetSocket::SendTerminalType()
{
	static char s[80];
    unsigned char c;

	strcpy(s, GetTerminalType());
    SendSymbol(CMD_IAC);
    SendSymbol(CMD_SB);
    SendSymbol(OPT_TTTYPE);
    SendSymbol(SUB_IS);
	int i = 0;
    while ((c = s[i++]) != 0)
	{
		SendSymbol(c);
		if (c == CMD_IAC)
			SendSymbol(c);
    }
    SendSymbol(CMD_IAC);
    SendSymbol(CMD_SE);
	Flush();
}

void CTelnetSocket::Flush()
{
	unsigned int nSentBytes = 0;
	Write((char *)&m_pszOutputBuffer[0], m_nCurrentOutputPos, nSentBytes);
	m_nCurrentOutputPos = 0;
}

void CTelnetSocket::ReceivedInDataState(unsigned char c)
{
	if (c == CMD_IAC)
		state = TS_IAC;	// �� � IAC ���������.
	else if (c == '\r' && hbin_mode != BIN_MODE)
	{
		SymbolReceived('\r');
		state = TS_CR;	// �� � TS_CR ���������.
	}
	else
		SymbolReceived(c);
}

void CTelnetSocket::ReceivedInCrState(unsigned char c)
{
	if (c == 0)
	{
		// Skip 0 after \r - do nothing
	}
	else if (c == LF)
		SymbolReceived('\n');	// ��� Windows ����� CR ����� ������� ������� �� ����� ������.
	else
		SymbolReceived(c);
	state = TS_DATA;	// ������� � ����� �������� ������.
}

void CTelnetSocket::ReceivedInIacState(unsigned char c)
{
	switch (c)
	{
		case CMD_DO:
			state = TS_DO; break;	// ������� � DO ���������
		case CMD_DONT:
			state = TS_DONT; break;	// ������� � DONT ���������
		case CMD_WILL:
			state = TS_WILL; break;	// ������� � WILL ���������
		case CMD_WONT:
			state = TS_WONT; break;	// ������� � WONT ���������
		case CMD_IAC:
			state = TS_DATA; break;	// ������� � ����� �������� ������. ��� ���� ������� ������ '\255'
		case CMD_SB:
			state = TS_SB; break;	// ������� � SB ���������.
		default:					// � ��� ��������� - ������.
			state = TS_DATA; break;	// ������� � ����� �������� ������.
	} // switch (c)
}

void CTelnetSocket::ReceivedInDoState(unsigned char c)
{
	switch (c)
	{
		case OPT_BINARY:	// �������� ����;
			if (ibin_mode != BIN_MODE)
			{
				if (ibin_mode == ASCII_MODE)
				SendWILL(OPT_BINARY);		// �������� !
				ibin_mode = BIN_MODE;
			}
		break;
		case OPT_SGA:		// ����� Suppress Go-Ahead;
			if (!do_sga)
			{
				SendWILL(OPT_SGA);		// �������� ! 
				do_sga = 1;			// ���� - ������������
			}
		break;
		case OPT_TTTYPE:	// ����������� �� ���� ���� ���������?
			if (!tt_send)
			{
				SendWILL(OPT_TTTYPE);		// �������� !
				tt_send = 1;			// ���� - ������������
			}
		break;
		case OPT_NAWS:		// ��� �� ���� ���������� ������?
			if (!naws_send)
			{
				SendWILL(OPT_NAWS);	// �������� !
				naws_send = 1;		// ���� - ������������
			}
			SendNaws();			// ������� ��������� ������
		break;
		case OPT_AUTHENTICATION:
			SendWONT(OPT_AUTHENTICATION);
			break;
		case OPT_ECHO:	// ��� �� ���� ECHO ...
		default:	// � ����� �������?
			SendWONT(c); // �� ��������.
			break;
	} // switch (c)
	state = TS_DATA;	// ������� � ����� �������� ������.
}

void CTelnetSocket::ReceivedInDontState(unsigned char c)
{
	switch (c)
	{
		case OPT_BINARY:	// �������� ����.
		if (ibin_mode != ASCII_MODE)
		{
			if (ibin_mode == BIN_MODE)
				SendWONT(OPT_BINARY); // �������� �� ������������
			ibin_mode = ASCII_MODE;
		}
		break;
	}
	state = TS_DATA;	// ������� � ����� �������� ������.
}

void CTelnetSocket::ReceivedInWillState(unsigned char c)
{
	switch (c)
	{
		case OPT_BINARY:	// �������� ����;
		if (hbin_mode != BIN_MODE)
		{
			if (hbin_mode == ASCII_MODE)
				SendDO(OPT_BINARY); // ��������
			hbin_mode = BIN_MODE;
		}
		break;
		case OPT_SGA:		// ����� Suppress Go-Ahead.
			if (!do_sga)
			{
				SendDO(OPT_SGA);	// ��������
				do_sga = 1;
			}
			break;
		case OPT_ECHO:		// ECHO - ������;
			if (!do_echo)
			{
				SendDO(OPT_ECHO);	// ��������
				do_echo = 1;
			}
		break;
	} // switch (c)
	state = TS_DATA;	// ������� � ����� �������� ������.
}

void CTelnetSocket::ReceivedInWontState(unsigned char c)
{
	switch (c)
	{
		case OPT_BINARY:	// �������� ����.
			if (hbin_mode != ASCII_MODE)
			{
				if (hbin_mode == BIN_MODE)
					SendDONT(OPT_BINARY);	// �������� �� ������������
				hbin_mode = ASCII_MODE;
			}
		break;
	}
	state = TS_DATA;	// ������� � ����� �������� ������.
}

void CTelnetSocket::ReceivedInSbState(unsigned char c)
{
	switch (c)
	{
		case OPT_TTTYPE:	// �������������� �� ���� ���������
			state = TS_SB_TERM;
			break; // ��������� TS_SB_TERM
		default:		// � ����� ������ ������ 
			state = TS_WAIT_SE;
			break;	// ��������� � ��������� �������� ����� �����������
	}
}

void CTelnetSocket::ReceivedInWaitSeState(unsigned char c)
{
	switch (c)
	{
		case CMD_IAC:		// �� ������� IAC
		state = TS_WAIT_SE1;
		break; // �������� � ��������� ����� ����������� TS_WAIT_SE1
	}
}

void CTelnetSocket::ReceivedInWaitSe1State(unsigned char c)
{
	switch (c)
	{
		case CMD_SE:		// ��������� ���������� �� SE
			state = TS_DATA;
			break; // ������� � ����� �������� ������.
		case CMD_IAC:		// ���� ���������� � ��������� �� ���������
			state = TS_WAIT_SE;
			break; // ��������� TS_WAIT_SE
		default: // �� ����� ������� ..
			state = TS_IAC; // ��������� � IAC ���������.
			break;	
	}
}

void CTelnetSocket::ReceivedInSbTermState(unsigned char c)
{
	switch (c)
	{
		case CMD_IAC:
			state = TS_WAIT_SE1;
			break;
		case SUB_SEND:
			state = TS_SEND_TERM;
			SendTerminalType();
			state = TS_WAIT_SE;
			break;
		default:
			state = TS_WAIT_SE;
			break;
	}
}

void CTelnetSocket::ReceivedInSendTermState(unsigned char c)
{
}

void CTelnetSocket::ReceivedInSendNawsState(unsigned char c)
{
	SendNaws();			// �������.
	state = TS_DATA;	// ����� ���������� ������.
}

//virtual
void CTelnetSocket::OnConnected()
{
	SetReadTimeout(3000);
	SendDO(OPT_SGA);
	SendWILL(OPT_TTTYPE);	// ����������� � ���� ���������?
	SendWILL(OPT_NAWS);	// ����������� � ������� ������?
	SendDO(OPT_ECHO);	// ����� �������� ���������� ����� ECHO.
	TelnetBinary();
	unsigned int nSentBytes;
	Write(szCRLF, 2, nSentBytes);
}

//virtual
void CTelnetSocket::OnSymbolReceived(char c)
{
	switch (state)
	{
		case TS_DATA:		// ��������� ��������� ������
			ReceivedInDataState(c);
			break;
		case TS_CR:			// ��������� TS_CR
			ReceivedInCrState(c);
			break;
	    case TS_IAC:	// IAC ���������
			ReceivedInIacState(c);
			break;
	    case TS_DO:			// ������ ���������� ������������ ...
			ReceivedInDoState(c);
			break;
	    case TS_DONT:		// ������ ���������� �� ������������ ...
			ReceivedInDontState(c);
			break;
	    case TS_WILL:		// ������ ����� ������������ ...
			ReceivedInWillState(c);
			break;
	    case TS_WONT:		// ������ �������� �� ������������ ...
			ReceivedInWontState(c);
			break;
	    case TS_SB:			// Start Subnegotiation
			ReceivedInSbState(c);
			break;
	    case TS_WAIT_SE:	// �� ��������� TS_WAIT_SE
			ReceivedInWaitSeState(c);
			break;
	    case TS_WAIT_SE1:	// �� ��������� TS_WAIT_SE1 ������ 2 ����
			ReceivedInWaitSe1State(c);
			break;
	    case TS_SB_TERM:	// �������������� �� ���� ���������
			ReceivedInSbTermState(c);
			break;
	    case TS_SEND_TERM:	// ������� ���������� � ���������.
			ReceivedInSendTermState(c);
			break;
	    case TS_SEND_NAWS: // ������� ���������� �� ������.
			ReceivedInSendNawsState(c);
			break;
		default:
			ASSERT(FALSE);
			break;
	} // switch (state)
}

//virtual
void CTelnetSocket::OnSymbolToUser(char symbol)
{
}

void CTelnetSocket::SymbolReceived(char symbol)
{
	if (IsProcessedAsControl(symbol))
		return;
	TRACE("%c ", symbol);
	m_UserSymbols++;
	OnSymbolToUser(symbol);
}


// Class CAnsiTelnetSocket implementation

CAnsiTelnetSocket::CAnsiTelnetSocket()
{
	m_EscapeState = Esc_None;
	nCSI_par = 0;
	CSI_currpar = 0;
	memset(intermediate, 0, sizeof(intermediate));
}

//virtual
CAnsiTelnetSocket::~CAnsiTelnetSocket()
{
	TRACE("CAnsiTelnetSocket object is destructed\n");
}

BOOL CAnsiTelnetSocket::IsControlSymbol(char symbol)
{
	return (symbol < ' '	||
			symbol == ESC	||
			symbol == CSI);
}

BOOL CAnsiTelnetSocket::ControlSymbolInNoneState(char symbol)
{
	if (IsControlSymbol(symbol))
	{
		switch (symbol)
		{
			case BEL:			// ������ (BEL)
				OnBell();
				return TRUE;
			case BS:			// Backspace (BS)
				OnBackspace();
				return TRUE;
			case HT:			// �������������� ��������� (HT)
				OnHorizontalTab();
				return TRUE;
			case LF:			// ������� �� ����� ����� (LF)
				OnLineFeed();
				return FALSE;
			case VT:			// ������������ ��������� (VT) VT100
				OnVerticalTab();
				return TRUE;
			case FF:			// ������ �������� (FF)
				OnFormFeed();
				return TRUE;
			case CR:			// ������� ������� (CR)
				OnCarriageReturn();
				return FALSE;
			case '\016':			// SO VT100
// UNDER CONSTRUCTION !!!
				break;
			case '\017':			// SI VT100
// UNDER CONSTRUCTION !!!					
				break;
			case ESC:
				memset(intermediate, 0, sizeof(intermediate));
				m_EscapeState = Esc_code;
				return TRUE;
			case CSI:
				memset(intermediate, 0, sizeof(intermediate));
				m_EscapeState = CSI_code;
				return TRUE;
		} // end of switch (symbol)
	}
	return FALSE; // not control symbol
}

BOOL CAnsiTelnetSocket::ControlSymbolInEscapeState(char symbol)
{
	if (symbol >= 0x20 && symbol < 0x30)
	{
	    for (int i = 0; i < MAX_INTERMEDIATE; i++)
			if (intermediate[i] == 0)
				break;
		if (i < MAX_INTERMEDIATE)
			intermediate[i] = symbol;
	}
	else
	{
	    if (intermediate[0] == 0)
		{
			switch (symbol)
			{
				case '[':			// Control Sequence Introducer ( ESC [ )
					m_EscapeState = CSI_code;
					break;
				case 'P':			// Device Control String ( VT100 )
					m_EscapeState = DCS_code;
					break;
				case 'Q':			// Defines function key Fn with string
					m_EscapeState = KeyDefine;
					//cs.ansi.key_flag = 0;
					break;
				case '7':			// ��������� ������� ������� �������
					OnStoreCursorPos();
					m_EscapeState = Esc_None; // �������� ���� escstate.
					break;
				case '8':			// ������������ ����������� ������� �������.
					OnRestoreCursorPos();
					m_EscapeState = Esc_None; // �������� ���� escstate.
					break;
				case 'H':			// Horizontal Tab Set (HTS)
					OnTabstopSet();
					//BITSET(cs.ansi.tabstop, cs.curx);
					m_EscapeState = Esc_None;
					break;
				case 'E':			// Next Line (NEL)
					//cs.curx = 0;
					//cs.cury++;
					OnNextLine();
					m_EscapeState = Esc_None;
					break;
				case 'D':			// Index (IND) - ���������� VT
				/*	cs.cury++;
					if(cs.cury == cs.maxy){ 	// ����� �� ������ ������ �������.
						cs.cury = cs.maxy - 1;	// ������ - ��� ������� ������.
						NextLine();
						Invalidate(FALSE);
						CWnd::UpdateWindow();
					}*/
					m_EscapeState = Esc_None;
					break;
				case 'M':			// Reverse Index (RI)
				/*	cs.cury--;
					if(cs.cury < 0){ 	// ����� �� ������ ������� �������.
						cs.cury = 0;	// ������ - ��� ������� ������.
						PreviousLine();
						Invalidate(FALSE);
						CWnd::UpdateWindow();
					}*/
					m_EscapeState = Esc_None;
					break;
				case '~':	// vt320
					m_EscapeState = Esc_None;
					break;
				case 'n':
					m_EscapeState = Esc_None;
					break;
				case '}':
					m_EscapeState = Esc_None;
					break;
				case 'o':
					m_EscapeState = Esc_None;
					break;
				case '|':
					m_EscapeState = Esc_None;
					break;
				case 'N':
					m_EscapeState = Esc_None;
					break;
				case 'O':
					m_EscapeState = Esc_None;
					break;
				case '\\':
					m_EscapeState = Esc_None;
					break;
				case '>':	// vt100
					m_EscapeState = Esc_None;
					break;
				case '=':	// vt100
					m_EscapeState = Esc_None;
					break; 
				default:
					m_EscapeState = Esc_None;
					break;
			} // end of switch(symbol)
		}
		else
		{
			unsigned char im = intermediate[0];
			switch (im)
			{
			    case '(': 
				case ')':
				case '*':
				case '+':	// vt100
				case ',':
				case '-':
				case '.':
				case '/':
					m_EscapeState = Esc_None;
					break;
				default:
					m_EscapeState = Esc_None;
					break;
			} // end of switch (im)
		} // else
	}
	return TRUE;
}

BOOL CAnsiTelnetSocket::ControlSymbolInSpecialState(char symbol)
{
	if (symbol >= '0' && symbol <= '9')
	{
		CSI_currpar =  (CSI_currpar * 10 + symbol - '0') & 0xFFFF;
	}
	else if (symbol == ';' || symbol == ':')
	{
		CSI_par[nCSI_par] = CSI_currpar;
		if (++(nCSI_par) >= MAX_CSI_PAR)
		{
			m_EscapeState = Esc_None;
			nCSI_par = 0;
			CSI_currpar = 0;
		}
		CSI_currpar = 0;
	} else if (symbol >= '<' && symbol <= '?')
	{
	    CSI_currpar = 0;
	    if (m_EscapeState != DCS_code)
		{
			switch (symbol)
			{
				case '?':
					m_EscapeState = DEC_Escape;
					break;
				case '=':
					m_EscapeState = SCO_Escape;
					break;
				case '>':
					m_EscapeState = GANS_Escape;
					break;
				case '<':
					break;
			}
	    }
	} else if (symbol >= 0x20 && symbol < 0x30)
	{
	    for (int i = 0; i < MAX_INTERMEDIATE; i++)
			if (intermediate[i] == 0)
				break;
	    if (i < MAX_INTERMEDIATE)
			intermediate[i] = symbol;
	}
	else
	{
		CSI_par[nCSI_par++] =  CSI_currpar;
		switch (m_EscapeState)
		{
			case SCO_Escape:
				//ExecuteSCOCode(symbol);
				break;
			case GANS_Escape:
				//ExecuteGANSCode(symbol);
				break;
			case DEC_Escape:
				//ExecuteDECCode (c);
				m_EscapeState = Esc_None;
				break;
			case CSI_code:
				//ExecuteCSICode (symbol);  
				break;
			case DCS_code:
				//ExecuteDCSCode (c);
				m_EscapeState = Esc_None;
				break;
		}
		if (m_EscapeState == SCO_Escape || m_EscapeState == GANS_Escape ||
			m_EscapeState == CSI_code || m_EscapeState == DEC_Escape)
			m_EscapeState = Esc_None;
		else if (m_EscapeState == DCS_code)
			m_EscapeState = Bad_DCS;
		nCSI_par = 0;
		CSI_currpar = 0;
		memset(intermediate, 0, sizeof(intermediate));
			/*if (c == '\033')
			    goto repeat;*/
	}
	return TRUE;
}

//virtual
BOOL CAnsiTelnetSocket::IsProcessedAsControl(char symbol)
{
	switch (m_EscapeState)
	{
		case Esc_None:
			return ControlSymbolInNoneState(symbol);
		case Esc_code:
			return ControlSymbolInEscapeState(symbol);
		case CSI_code:
		case SCO_Escape:
		case GANS_Escape:
		case DEC_Escape:
		case DCS_code:
			return ControlSymbolInSpecialState(symbol);
	} // switch
	return FALSE;
}

