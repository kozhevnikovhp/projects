// SerialStatistics.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <vector>

///////////////////////////////////////////
// Serial

typedef struct
{
	__int64 InputPackets;
	__int64 InputBytes;
	__int64 InputErrors;
	__int64 InputDrops;
	__int64 OutputPackets;
	__int64 OutputBytes;
	__int64 OutputErrors;
	__int64 OutputDrops;
} SInterfaceStatus;

typedef struct : public SInterfaceStatus
{
	time_t Time;
} STimedStatus;

typedef std::vector<STimedStatus> CVectorOfSerialStatus;

static CVectorOfSerialStatus SerialStatusVector;

int ProcessSerialFile(char *pszFileName)
{
	printf("Processing %s file...", pszFileName);

	FILE *fd = fopen(pszFileName, "r");
	if (!fd)
		return 0;
	tm t;
	int nLines = 0;

	while (1)
	{
		STimedStatus status;
		int rc = fscanf(fd, "%02d.%02d.%d\t%02d:%02d:%02d\t%I64d\t%I64d\t%I64d\t%I64d\t\t%I64d\t%I64d\t%I64d\t%I64d",
							&t.tm_mday, &t.tm_mon, &t.tm_year,
							&t.tm_hour, &t.tm_min, &t.tm_sec,
							&status.InputPackets, &status.InputBytes, &status.InputErrors, &status.InputDrops,
							&status.OutputPackets, &status.OutputBytes, &status.OutputErrors, &status.OutputDrops);
		if (rc == EOF)
			break;
		else if (rc == 14)
		{
			++nLines;
			t.tm_mday--; // 0-based
			t.tm_mon--; // Jan is 0
			t.tm_year -= 1900;
			status.Time = mktime(&t) + 3*3600;
			SerialStatusVector.push_back(status);
		}
		else
		{
			printf("Error\n");
		}

		// Skip USZ info
		rc = fscanf(fd, "\t%I64d\t%I64d\t%I64d\t%I64d\t\t%I64d\t%I64d\t%I64d\t%I64d\n",
							&status.InputPackets, &status.InputBytes, &status.InputErrors, &status.InputDrops,
							&status.OutputPackets, &status.OutputBytes, &status.OutputErrors, &status.OutputDrops);
		if (rc == EOF)
			break;
	}
	if (fd)
		fclose(fd);
	printf("%d lines\n", nLines);

	return 1;
}

int LookAtDirSerial(char *pszDirName)
{
	int nProcessedFile = 0;
	char pszTemplate[1024];
	strcpy(pszTemplate, pszDirName);
	strcat(pszTemplate, "\\*.ser");
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile(pszTemplate, &fd);
	BOOL bFound = (hFile != INVALID_HANDLE_VALUE);
	char pszBuffer[1024];
	
	while (bFound)
	{
		if (strcmp(fd.cFileName, ".") &&
			strcmp(fd.cFileName, ".."))
		{
			strcpy(pszBuffer, pszDirName);
			strcat(pszBuffer, "\\");
			strcat(pszBuffer, fd.cFileName);
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				nProcessedFile += LookAtDirSerial(pszBuffer);
			}
			else
			{
				nProcessedFile += ProcessSerialFile(pszBuffer);
			}
		}
		bFound = FindNextFile(hFile, &fd);
	}
	FindClose(hFile);
	return nProcessedFile;
}

int CompareSerialProc(const void *pElem1, const void *pElem2)
{
	STimedStatus *p1 = *(STimedStatus **)pElem1;
	STimedStatus *p2 = *(STimedStatus **)pElem2;
	time_t Time1 = p1->Time;
	time_t Time2 = p2->Time;
	return (Time1 - Time2);
}

void ProcessSerialFiles(char *pszDir)
{
	int nProcessedFiles = LookAtDirSerial(pszDir);
	printf("%d files processed                           \n", nProcessedFiles);

	int nRecords = SerialStatusVector.size();
	printf("%d records\n", nRecords);

	STimedStatus **ppStatus = (STimedStatus **)calloc(nRecords, sizeof(STimedStatus *));
	if (!ppStatus)
	{
		printf("Out of memory\n");
	}

	for (int i = 0; i < nRecords; i++)
	{
		ppStatus[i] = &SerialStatusVector[i];
	}
	::qsort(ppStatus, nRecords, sizeof(STimedStatus *), CompareSerialProc);

	FILE *fd_long = fopen("Serial_long.txt", "w");
	FILE *fd_short = fopen("Serial_short.txt", "w");
	time_t PrevTime;
	__int64 PrevPackets, PrevBytes;
	int bPrevInit = 0;
	for (i = 0; i < nRecords; i++)
	{
		STimedStatus *pStatus = ppStatus[i];
		if (fd_long)
		{
			tm *pTM = gmtime(&pStatus->Time);
			fprintf(fd_long, "%02d.%02d.%d %02d:%02d:%02d\t%I64d\t%I64d\t%I64d\t%I64d\t\t%I64d\t%I64d\t%I64d\t%I64d\n",
						pTM->tm_mday+1, pTM->tm_mon+1, pTM->tm_year+1900,
						pTM->tm_hour, pTM->tm_min, pTM->tm_sec,
						pStatus->InputPackets, pStatus->InputBytes, pStatus->InputErrors, pStatus->InputDrops,
						pStatus->OutputPackets, pStatus->OutputBytes, pStatus->OutputErrors, pStatus->OutputDrops);
		}
		if (fd_short)
		{
			if (pStatus->OutputBytes > -1 && pStatus->OutputPackets > -1)
			{
				if (bPrevInit && pStatus->OutputPackets > PrevPackets && pStatus->OutputBytes > PrevBytes)
				{
					int DeltaTime = pStatus->Time - PrevTime;
					tm PrevTM, ThisTM;
					tm *pTM = gmtime(&pStatus->Time);
					memcpy(&ThisTM, pTM, sizeof(ThisTM));
					pTM = gmtime(&PrevTime);
					memcpy(&PrevTM, pTM, sizeof(PrevTM));
					__int64 Packets = pStatus->OutputPackets - PrevPackets;
					double PacketsPerSecond = (double)Packets/DeltaTime;
					__int64 Bytes = pStatus->OutputBytes - PrevBytes;
					double BitsPerSecond = Bytes*8./DeltaTime;					
					
					fprintf(fd_short, "%02d.%02d.%d %02d:%02d:%02d\t%.2f\t%.2f\n",
							PrevTM.tm_mday+1, PrevTM.tm_mon+1, PrevTM.tm_year+1900,
							PrevTM.tm_hour, PrevTM.tm_min, PrevTM.tm_sec,
							PacketsPerSecond, BitsPerSecond);
					fprintf(fd_short, "%02d.%02d.%d %02d:%02d:%02d\t%.2f\t%.2f\n",
							ThisTM.tm_mday+1, ThisTM.tm_mon+1, ThisTM.tm_year+1900,
							ThisTM.tm_hour, ThisTM.tm_min, ThisTM.tm_sec,
							PacketsPerSecond, BitsPerSecond);
				}
				bPrevInit = 1;
				PrevPackets = pStatus->OutputPackets;
				PrevBytes = pStatus->OutputBytes;
				PrevTime = pStatus->Time;
			}
			else
				bPrevInit = 0;
		}
	}
	if (fd_short)
		fclose(fd_short);
	if (fd_long)
		fclose(fd_long);

	free(ppStatus);
}

///////////////////////////////////////////////////////////////////////
// UDP

typedef struct
{
	__int64 TotalInputPackets;
	__int64 PacketsShorterThanHeader;
	__int64 ChecksumErrors;
	__int64 DataLengthLargerThanPacket;
	__int64 NoSocketOnPort;
	__int64 OfAboveArrivedAsBroadcast;
	__int64 NotDeliveredInputSocketFull;
	__int64 InputPacketsMissingPcbCache;
	__int64 InputPacketsNotForHashedPcb;
	__int64 TotalOutputPackets;
} SUdpStatistics;

typedef struct : public SUdpStatistics
{
	time_t Time;
} STimedUdpStatistics;

typedef std::vector<STimedUdpStatistics> CVectorOfUdpStat;

static CVectorOfUdpStat UdpStatVector;

int ProcessUdpFile(char *pszFileName)
{
	printf("Processing %s file...", pszFileName);

	FILE *fd = fopen(pszFileName, "r");
	if (!fd)
		return 0;
	tm t;
	int nLines = 0;

	while (1)
	{
		STimedUdpStatistics stat;
		int rc = fscanf(fd, "%02d.%02d.%d\t%02d:%02d:%02d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							&t.tm_mday, &t.tm_mon, &t.tm_year,
							&t.tm_hour, &t.tm_min, &t.tm_sec,
							&stat.TotalInputPackets,
							&stat.PacketsShorterThanHeader,
							&stat.ChecksumErrors,
							&stat.DataLengthLargerThanPacket,
							&stat.NoSocketOnPort,
							&stat.OfAboveArrivedAsBroadcast,
							&stat.NotDeliveredInputSocketFull,
							&stat.InputPacketsMissingPcbCache,
							&stat.InputPacketsNotForHashedPcb,
							&stat.TotalOutputPackets);
		if (rc == EOF)
			break;
		else if (rc == 16)
		{
			++nLines;
			t.tm_mday--; // 0-based
			t.tm_mon--; // Jan is 0
			t.tm_year -= 1900;
			stat.Time = mktime(&t) + 3*3600;
			UdpStatVector.push_back(stat);
		}
		else
		{
			printf("Error\n");
		}

		// Skip USZ info
		rc = fscanf(fd, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\n",
							&stat.TotalInputPackets,
							&stat.PacketsShorterThanHeader,
							&stat.ChecksumErrors,
							&stat.DataLengthLargerThanPacket,
							&stat.NoSocketOnPort,
							&stat.OfAboveArrivedAsBroadcast,
							&stat.NotDeliveredInputSocketFull,
							&stat.InputPacketsMissingPcbCache,
							&stat.InputPacketsNotForHashedPcb,
							&stat.TotalOutputPackets);
		if (rc == EOF)
			break;
	}
	if (fd)
		fclose(fd);
	printf("%d lines\n", nLines);

	return 1;
}

int LookAtDirUdp(char *pszDirName)
{
	int nProcessedFile = 0;
	char pszTemplate[1024];
	strcpy(pszTemplate, pszDirName);
	strcat(pszTemplate, "\\*.udp");
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile(pszTemplate, &fd);
	BOOL bFound = (hFile != INVALID_HANDLE_VALUE);
	char pszBuffer[1024];
	
	while (bFound)
	{
		if (strcmp(fd.cFileName, ".") &&
			strcmp(fd.cFileName, ".."))
		{
			strcpy(pszBuffer, pszDirName);
			strcat(pszBuffer, "\\");
			strcat(pszBuffer, fd.cFileName);
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				nProcessedFile += LookAtDirUdp(pszBuffer);
			}
			else
			{
				nProcessedFile += ProcessUdpFile(pszBuffer);
			}
		}
		bFound = FindNextFile(hFile, &fd);
	}
	FindClose(hFile);
	return nProcessedFile;
}

int CompareUdpProc(const void *pElem1, const void *pElem2)
{
	STimedUdpStatistics *p1 = *(STimedUdpStatistics **)pElem1;
	STimedUdpStatistics *p2 = *(STimedUdpStatistics **)pElem2;
	time_t Time1 = p1->Time;
	time_t Time2 = p2->Time;
	return (Time1 - Time2);
}

void ProcessUdpFiles(char *pszDir)
{
	int nProcessedFiles = LookAtDirUdp(pszDir);
	printf("%d files processed                           \n", nProcessedFiles);

	int nRecords = UdpStatVector.size();
	printf("%d records\n", nRecords);

	STimedUdpStatistics **ppStat = (STimedUdpStatistics **)calloc(nRecords, sizeof(STimedUdpStatistics *));
	if (!ppStat)
	{
		printf("Out of memory\n");
	}

	for (int i = 0; i < nRecords; i++)
	{
		ppStat[i] = &UdpStatVector[i];
	}
	::qsort(ppStat, nRecords, sizeof(STimedUdpStatistics *), CompareUdpProc);

	FILE *fd_long = fopen("Udp_long.txt", "w");
	FILE *fd_short = fopen("Udp_short.txt", "w");

	time_t PrevTime;
	__int64 PrevPackets;
	int bPrevInit = 0;

	for (i = 0; i < nRecords; i++)
	{
		STimedUdpStatistics *pStat = ppStat[i];
		if (fd_short)
		{
			if (pStat->TotalOutputPackets > -1)
			{
				if (bPrevInit && pStat->TotalOutputPackets > PrevPackets)
				{
					int DeltaTime = pStat->Time - PrevTime;
					tm PrevTM, ThisTM;
					tm *pTM = gmtime(&pStat->Time);
					memcpy(&ThisTM, pTM, sizeof(ThisTM));
					pTM = gmtime(&PrevTime);
					memcpy(&PrevTM, pTM, sizeof(PrevTM));
					__int64 Packets = pStat->TotalOutputPackets - PrevPackets;
					double PacketsPerSecond = (double)Packets/DeltaTime;
					fprintf(fd_short, "%02d.%02d.%d %02d:%02d:%02d\t%.2f\n",
							PrevTM.tm_mday+1, PrevTM.tm_mon+1, PrevTM.tm_year+1900,
							PrevTM.tm_hour, PrevTM.tm_min, PrevTM.tm_sec,
							PacketsPerSecond);
					fprintf(fd_short, "%02d.%02d.%d %02d:%02d:%02d\t%.2f\n",
							ThisTM.tm_mday+1, ThisTM.tm_mon+1, ThisTM.tm_year+1900,
							ThisTM.tm_hour, ThisTM.tm_min, ThisTM.tm_sec,
							PacketsPerSecond);
				}
				bPrevInit = 1;
				PrevPackets = pStat->TotalOutputPackets;
				PrevTime = pStat->Time;
			}
		}
		if (fd_long)
		{
			tm *pTM = gmtime(&pStat->Time);
			fprintf(fd_long, "%02d.%02d.%d %02d:%02d:%02d\t%I64d\t%I64d\t%I64d\t%I64d\t\t%I64d\t%I64d\t%I64d\t%I64d\n",
						pTM->tm_mday+1, pTM->tm_mon+1, pTM->tm_year+1900,
						pTM->tm_hour, pTM->tm_min, pTM->tm_sec,
						pStat->TotalInputPackets,
						pStat->PacketsShorterThanHeader,
						pStat->ChecksumErrors,
						pStat->DataLengthLargerThanPacket,
						pStat->NoSocketOnPort,
						pStat->OfAboveArrivedAsBroadcast,
						pStat->NotDeliveredInputSocketFull,
						pStat->InputPacketsMissingPcbCache,
						pStat->InputPacketsNotForHashedPcb,
						pStat->TotalOutputPackets);
		}
	}
	
	if (fd_short)
		fclose(fd_short);
	if (fd_long)
		fclose(fd_long);

	free(ppStat);
}

/////////////////////////////////////////////////////////
// TCP

typedef struct
{
	__int64 ConnectionsInitiated;
	__int64 ConnectionsAccepted;
	__int64 ConnectionsEstablished;
	__int64 ConnectionsDropped;
	__int64 EmbryonicConnectionsDropped;
	__int64 ConnectionsClosed;
	__int64 SegsWhereWeTriedToGetRTT;
	__int64 TimesWeSucceeded;
	__int64 DelayedAcksSent;
	__int64 ConnectionsDroppedInRxmtTimeout;
	__int64 RetransmitTimeouts;
	__int64 PersistTimeouts;
	__int64 KeepaliveTimeouts;
	__int64 KeepaliveProbesSent;
	__int64 ConnectionsDroppedInKeepalive;
	__int64 TotalPacketsSent;
	__int64 DataPacketsSent;
	__int64 DataBytesSent;
	__int64 DataPacketsRetransmitted;
	__int64 DataBytesRetransmitted;
	__int64 AckOnlyPacketsSent;
	__int64 WindowProbesSent;
	__int64 PacketsSentWithUrgOnly;
	__int64 WindowUpdateOnlyPacketsSent;
	__int64 ControlPacketsSent;
	__int64 TotalPacketsReceived;
	__int64 PacketsReceivedInSequence;
	__int64 BytesReceivedInSequence;
	__int64 PacketsReceivedWithChecksumError;
	__int64 PacketsReceivedWithBadOffset;
	__int64 PacketsReceivedTooShort;
	__int64 DuplicateOnlyPacketsReceived;
	__int64 DuplicateOnlyBytesReceived;
	__int64 PacketsWithSomeDuplicateData;
	__int64 DupBytesInPartDupPackets;
	__int64 OutOfOrderPacketsReceived;
	__int64 OutOfOrderBytesReceived;
	__int64 PacketsWithDataAfterWindow;
	__int64 BytesRcvdAfterWindow;
	__int64 PacketsRcvdAfterClose;
	__int64 RcvdWindowProbePackets;
	__int64 RcvdDuplicateAcks;
	__int64 RcvdAcksForUnsentData;
	__int64 RcvdAckPackets;
	__int64 BytesAckedByRcvdAcks;
	__int64 RcvdWindowUpdatePackets;
	__int64 SegmentsDroppedDueToPAWS;
	__int64 TimesHdrPredictOkForAcks;
	__int64 TimesHdrPredictOkForDataPkts;
	__int64 PcbCacheMisses;
	__int64 TimesCachedRTTInRouteUpdated;
	__int64 TimesCachedRTTVarUpdated;
	__int64 TimesCachedSsThreshUpdated;
	__int64 TimesRTTIninitializedFromRoute;
	__int64 TimesRTTVarInitializedFromRT;
	__int64 TimesSsThreshInitializedFromRT;
	__int64 TimeoutInPersistState;
	__int64 BogusSYNPrematureACK;
	__int64 ResendsDueToMTUDiscovery;
	__int64 ListenQueueOverflows;
} STcpStatistics;

typedef struct : public STcpStatistics
{
	time_t Time;
} STimedTcpStatistics;

typedef std::vector<STimedTcpStatistics> CVectorOfTcpStat;

static CVectorOfTcpStat TcpStatVector;

int ProcessTcpFile(char *pszFileName)
{
	printf("Processing %s file...", pszFileName);

	FILE *fd = fopen(pszFileName, "r");
	if (!fd)
		return 0;
	tm t;
	int nLines = 0;

	while (1)
	{
		STimedTcpStatistics stat;
		int rc = fscanf(fd, "%02d.%02d.%d\t%02d:%02d:%02d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d",
							&t.tm_mday, &t.tm_mon, &t.tm_year,
							&t.tm_hour, &t.tm_min, &t.tm_sec,
							&stat.ConnectionsInitiated,
							&stat.ConnectionsAccepted,
							&stat.ConnectionsEstablished,
							&stat.ConnectionsDropped,
							&stat.EmbryonicConnectionsDropped,
							&stat.ConnectionsClosed,
							&stat.SegsWhereWeTriedToGetRTT,
							&stat.TimesWeSucceeded,
							&stat.DelayedAcksSent,
							&stat.ConnectionsDroppedInRxmtTimeout,
							&stat.RetransmitTimeouts,
							&stat.PersistTimeouts,
							&stat.KeepaliveTimeouts,
							&stat.KeepaliveProbesSent,
							&stat.ConnectionsDroppedInKeepalive,
							&stat.TotalPacketsSent,
							&stat.DataPacketsSent,
							&stat.DataBytesSent,
							&stat.DataPacketsRetransmitted,
							&stat.DataBytesRetransmitted,
							&stat.AckOnlyPacketsSent,
							&stat.WindowProbesSent,
							&stat.PacketsSentWithUrgOnly,
							&stat.WindowUpdateOnlyPacketsSent,
							&stat.ControlPacketsSent,
							&stat.TotalPacketsReceived,
							&stat.PacketsReceivedInSequence,
							&stat.BytesReceivedInSequence,
							&stat.PacketsReceivedWithChecksumError,
							&stat.PacketsReceivedWithBadOffset,
							&stat.PacketsReceivedTooShort,
							&stat.DuplicateOnlyPacketsReceived,
							&stat.DuplicateOnlyBytesReceived,
							&stat.PacketsWithSomeDuplicateData,
							&stat.DupBytesInPartDupPackets,
							&stat.OutOfOrderPacketsReceived,
							&stat.OutOfOrderBytesReceived,
							&stat.PacketsWithDataAfterWindow,
							&stat.BytesRcvdAfterWindow,
							&stat.PacketsRcvdAfterClose,
							&stat.RcvdWindowProbePackets,
							&stat.RcvdDuplicateAcks,
							&stat.RcvdAcksForUnsentData,
							&stat.RcvdAckPackets,
							&stat.BytesAckedByRcvdAcks,
							&stat.RcvdWindowUpdatePackets,
							&stat.SegmentsDroppedDueToPAWS,
							&stat.TimesHdrPredictOkForAcks,
							&stat.TimesHdrPredictOkForDataPkts,
							&stat.PcbCacheMisses,
							&stat.TimesCachedRTTInRouteUpdated,
							&stat.TimesCachedRTTVarUpdated,
							&stat.TimesCachedSsThreshUpdated,
							&stat.TimesRTTIninitializedFromRoute,
							&stat.TimesRTTVarInitializedFromRT,
							&stat.TimesSsThreshInitializedFromRT,
							&stat.TimeoutInPersistState,
							&stat.BogusSYNPrematureACK,
							&stat.ResendsDueToMTUDiscovery,
							&stat.ListenQueueOverflows);
		if (rc == EOF)
			break;
		else if (rc == 66)
		{
			++nLines;
			t.tm_mday--; // 0-based
			t.tm_mon--; // Jan is 0
			t.tm_year -= 1900;
			stat.Time = mktime(&t) + 3*3600;
			TcpStatVector.push_back(stat);
		}
		else
		{
			printf("Error\n");
		}

		// Skip USZ info
		rc = fscanf(fd, "\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\n",
							&stat.ConnectionsInitiated,
							&stat.ConnectionsAccepted,
							&stat.ConnectionsEstablished,
							&stat.ConnectionsDropped,
							&stat.EmbryonicConnectionsDropped,
							&stat.ConnectionsClosed,
							&stat.SegsWhereWeTriedToGetRTT,
							&stat.TimesWeSucceeded,
							&stat.DelayedAcksSent,
							&stat.ConnectionsDroppedInRxmtTimeout,
							&stat.RetransmitTimeouts,
							&stat.PersistTimeouts,
							&stat.KeepaliveTimeouts,
							&stat.KeepaliveProbesSent,
							&stat.ConnectionsDroppedInKeepalive,
							&stat.TotalPacketsSent,
							&stat.DataPacketsSent,
							&stat.DataBytesSent,
							&stat.DataPacketsRetransmitted,
							&stat.DataBytesRetransmitted,
							&stat.AckOnlyPacketsSent,
							&stat.WindowProbesSent,
							&stat.PacketsSentWithUrgOnly,
							&stat.WindowUpdateOnlyPacketsSent,
							&stat.ControlPacketsSent,
							&stat.TotalPacketsReceived,
							&stat.PacketsReceivedInSequence,
							&stat.BytesReceivedInSequence,
							&stat.PacketsReceivedWithChecksumError,
							&stat.PacketsReceivedWithBadOffset,
							&stat.PacketsReceivedTooShort,
							&stat.DuplicateOnlyPacketsReceived,
							&stat.DuplicateOnlyBytesReceived,
							&stat.PacketsWithSomeDuplicateData,
							&stat.DupBytesInPartDupPackets,
							&stat.OutOfOrderPacketsReceived,
							&stat.OutOfOrderBytesReceived,
							&stat.PacketsWithDataAfterWindow,
							&stat.BytesRcvdAfterWindow,
							&stat.PacketsRcvdAfterClose,
							&stat.RcvdWindowProbePackets,
							&stat.RcvdDuplicateAcks,
							&stat.RcvdAcksForUnsentData,
							&stat.RcvdAckPackets,
							&stat.BytesAckedByRcvdAcks,
							&stat.RcvdWindowUpdatePackets,
							&stat.SegmentsDroppedDueToPAWS,
							&stat.TimesHdrPredictOkForAcks,
							&stat.TimesHdrPredictOkForDataPkts,
							&stat.PcbCacheMisses,
							&stat.TimesCachedRTTInRouteUpdated,
							&stat.TimesCachedRTTVarUpdated,
							&stat.TimesCachedSsThreshUpdated,
							&stat.TimesRTTIninitializedFromRoute,
							&stat.TimesRTTVarInitializedFromRT,
							&stat.TimesSsThreshInitializedFromRT,
							&stat.TimeoutInPersistState,
							&stat.BogusSYNPrematureACK,
							&stat.ResendsDueToMTUDiscovery,
							&stat.ListenQueueOverflows);
		if (rc == EOF)
			break;
	}
	if (fd)
		fclose(fd);
	printf("%d lines\n", nLines);

	return 1;
}

int LookAtDirTcp(char *pszDirName)
{
	int nProcessedFile = 0;
	char pszTemplate[1024];
	strcpy(pszTemplate, pszDirName);
	strcat(pszTemplate, "\\*.tcp");
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile(pszTemplate, &fd);
	BOOL bFound = (hFile != INVALID_HANDLE_VALUE);
	char pszBuffer[1024];
	
	while (bFound)
	{
		if (strcmp(fd.cFileName, ".") &&
			strcmp(fd.cFileName, ".."))
		{
			strcpy(pszBuffer, pszDirName);
			strcat(pszBuffer, "\\");
			strcat(pszBuffer, fd.cFileName);
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				nProcessedFile += LookAtDirTcp(pszBuffer);
			}
			else
			{
				nProcessedFile += ProcessTcpFile(pszBuffer);
			}
		}
		bFound = FindNextFile(hFile, &fd);
	}
	FindClose(hFile);
	return nProcessedFile;
}

int CompareTcpProc(const void *pElem1, const void *pElem2)
{
	STimedTcpStatistics *p1 = *(STimedTcpStatistics **)pElem1;
	STimedTcpStatistics *p2 = *(STimedTcpStatistics **)pElem2;
	time_t Time1 = p1->Time;
	time_t Time2 = p2->Time;
	return (Time1 - Time2);
}

void ProcessTcpFiles(char *pszDir)
{
	int nProcessedFiles = LookAtDirTcp(pszDir);
	printf("%d files processed                           \n", nProcessedFiles);

	int nRecords = TcpStatVector.size();
	printf("%d records\n", nRecords);

	STimedTcpStatistics **ppStat = (STimedTcpStatistics **)calloc(nRecords, sizeof(STimedTcpStatistics *));
	if (!ppStat)
	{
		printf("Out of memory\n");
	}

	for (int i = 0; i < nRecords; i++)
	{
		ppStat[i] = &TcpStatVector[i];
	}
	::qsort(ppStat, nRecords, sizeof(STimedTcpStatistics *), CompareTcpProc);

	FILE *fd_long = fopen("Tcp_long.txt", "w");
	FILE *fd_short = fopen("Tcp_short.txt", "w");

	time_t PrevTime;
	__int64 PrevPackets, PrevDataPackets, PrevDataBytes;
	int bPrevInit = 0;

	for (i = 0; i < nRecords; i++)
	{
		STimedTcpStatistics *pStat = ppStat[i];
		if (fd_short)
		{
			if (pStat->TotalPacketsSent > -1 && pStat->DataPacketsSent > -1 && pStat->DataBytesSent > -1)
			{
				if (bPrevInit && pStat->TotalPacketsSent > PrevPackets && pStat->DataPacketsSent > PrevDataPackets && pStat->DataBytesSent > PrevDataBytes)
				{
					int DeltaTime = pStat->Time - PrevTime;
					tm PrevTM, ThisTM;
					tm *pTM = gmtime(&pStat->Time);
					memcpy(&ThisTM, pTM, sizeof(ThisTM));
					pTM = gmtime(&PrevTime);
					memcpy(&PrevTM, pTM, sizeof(PrevTM));
					__int64 Packets = pStat->TotalPacketsSent - PrevPackets;
					double PacketsPerSecond = (double)Packets/DeltaTime;
					__int64 DataPackets = pStat->DataPacketsSent - PrevDataPackets;
					double DataPacketsPerSecond = (double)DataPackets/DeltaTime;
					__int64 DataBytes = pStat->DataBytesSent - PrevDataBytes;
					double DataBytesPerSecond = (double)DataBytes/DeltaTime;
					double DataBitsPerSecond = DataBytesPerSecond*8;
					fprintf(fd_short, "%02d.%02d.%d %02d:%02d:%02d\t%.3f\t%.3f\t%.3f\n",
						PrevTM.tm_mday+1, PrevTM.tm_mon+1, PrevTM.tm_year+1900,
						PrevTM.tm_hour, PrevTM.tm_min, PrevTM.tm_sec,
						PacketsPerSecond,
						DataPacketsPerSecond,
						DataBitsPerSecond);
					fprintf(fd_short, "%02d.%02d.%d %02d:%02d:%02d\t%.3f\t%.3f\t%.3f\n",
						ThisTM.tm_mday+1, ThisTM.tm_mon+1, ThisTM.tm_year+1900,
						ThisTM.tm_hour, ThisTM.tm_min, ThisTM.tm_sec,
						PacketsPerSecond,
						DataPacketsPerSecond,
						DataBitsPerSecond);
				}
				bPrevInit = 1;
				PrevPackets = pStat->TotalPacketsSent;
				PrevDataPackets = pStat->DataPacketsSent;
				PrevDataBytes = pStat->DataBytesSent;
				PrevTime = pStat->Time;
			}
		}
		if (fd_long)
		{
			tm *pTM = gmtime(&pStat->Time);
			fprintf(fd_long, "%02d.%02d.%d %02d:%02d:%02d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\t%I64d\n",
						pTM->tm_mday+1, pTM->tm_mon+1, pTM->tm_year+1900,
						pTM->tm_hour, pTM->tm_min, pTM->tm_sec,
						pStat->ConnectionsInitiated,
						pStat->ConnectionsAccepted,
						pStat->ConnectionsEstablished,
						pStat->ConnectionsDropped,
						pStat->EmbryonicConnectionsDropped,
						pStat->ConnectionsClosed,
						pStat->SegsWhereWeTriedToGetRTT,
						pStat->TimesWeSucceeded,
						pStat->DelayedAcksSent,
						pStat->ConnectionsDroppedInRxmtTimeout,
						pStat->RetransmitTimeouts,
						pStat->PersistTimeouts,
						pStat->KeepaliveTimeouts,
						pStat->KeepaliveProbesSent,
						pStat->ConnectionsDroppedInKeepalive,
						pStat->TotalPacketsSent,
						pStat->DataPacketsSent,
						pStat->DataBytesSent,
						pStat->DataPacketsRetransmitted,
						pStat->DataBytesRetransmitted,
						pStat->AckOnlyPacketsSent,
						pStat->WindowProbesSent,
						pStat->PacketsSentWithUrgOnly,
						pStat->WindowUpdateOnlyPacketsSent,
						pStat->ControlPacketsSent,
						pStat->TotalPacketsReceived,
						pStat->PacketsReceivedInSequence,
						pStat->BytesReceivedInSequence,
						pStat->PacketsReceivedWithChecksumError,
						pStat->PacketsReceivedWithBadOffset,
						pStat->PacketsReceivedTooShort,
						pStat->DuplicateOnlyPacketsReceived,
						pStat->DuplicateOnlyBytesReceived,
						pStat->PacketsWithSomeDuplicateData,
						pStat->DupBytesInPartDupPackets,
						pStat->OutOfOrderPacketsReceived,
						pStat->OutOfOrderBytesReceived,
						pStat->PacketsWithDataAfterWindow,
						pStat->BytesRcvdAfterWindow,
						pStat->PacketsRcvdAfterClose,
						pStat->RcvdWindowProbePackets,
						pStat->RcvdDuplicateAcks,
						pStat->RcvdAcksForUnsentData,
						pStat->RcvdAckPackets,
						pStat->BytesAckedByRcvdAcks,
						pStat->RcvdWindowUpdatePackets,
						pStat->SegmentsDroppedDueToPAWS,
						pStat->TimesHdrPredictOkForAcks,
						pStat->TimesHdrPredictOkForDataPkts,
						pStat->PcbCacheMisses,
						pStat->TimesCachedRTTInRouteUpdated,
						pStat->TimesCachedRTTVarUpdated,
						pStat->TimesCachedSsThreshUpdated,
						pStat->TimesRTTIninitializedFromRoute,
						pStat->TimesRTTVarInitializedFromRT,
						pStat->TimesSsThreshInitializedFromRT,
						pStat->TimeoutInPersistState,
						pStat->BogusSYNPrematureACK,
						pStat->ResendsDueToMTUDiscovery,
						pStat->ListenQueueOverflows);
		}
	}
	if (fd_short)
		fclose(fd_short);
	if (fd_long)
		fclose(fd_long);

	free(ppStat);
}


int main(int argc, char* argv[])
{
	char pszBuffer[1024];
	GetCurrentDirectory(sizeof(pszBuffer), pszBuffer);

	ProcessSerialFiles(pszBuffer);
	ProcessUdpFiles(pszBuffer);
	ProcessTcpFiles(pszBuffer);

	
	getch();
	return 0;
}
