#include "stdafx.h"
#include "../Common/SDMS_OIDs.h"
#include "PozharskyUpConverter.h"

MC_ErrorCode processPozharskyUpRequest(CPozharskyUpDownConverter *pConverter, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC = MC_DEVICE_NOT_RESPONDING;

	printf("\tup converter\n");
	if (var.m_OID.isPartOfOID(OidPozharskyUpOnOff, OidPozharskyUpOnOffLen))
	{
		printf("\t\tenabled ");
		BOOL bOn;
		if (bGet)
		{ // get
			EC = pConverter->IsUpTurnedOn(bOn);
			if (EC == MC_OK)
			{
				if (bOn)
				{
					var.setInteger32Value(2);
					printf("ON");
				}
				else
				{
					var.setInteger32Value(1);
					printf("OFF");
				}
			}
		}
		else
		{ // set
			bOn = (var.m_iIntegerValue == 2);
			if (bOn)
				printf("ON");
			else
				printf("OFF");
			EC = pConverter->TurnUpOn(bOn);
			if (EC == MC_OK)
			{
				printf(", result = ");
				if (bOn)
					printf("ON");
				else
					printf("OFF");
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidPozharskyUpFrequency, OidPozharskyUpFrequencyLen))
	{
		printf("\t\tfrequency ");
		unsigned int frequency;
		if (bGet)
		{ // get
			EC = pConverter->GetUpOutputFrequency(frequency);
			if (EC == MC_OK)
			{
				var.setInteger32Value(frequency);
				printf("%d MHz", frequency);
			}
		}
		else
		{ // set
			frequency = var.m_uiIntegerValue;
			printf("%d MHz", frequency);
			EC = pConverter->SetUpOutputFrequency(frequency);
			if (EC == MC_OK)
				printf(" result = %d MHz", frequency);
		}
	}
	else if (var.m_OID.isPartOfOID(OidPozharskyUpAttenuator, OidPozharskyUpAttenuatorLen))
	{
		printf("\t\tattenuator ");
		double att;
		if (bGet)
		{ // get
			EC = pConverter->GetUpInputAtt(att);
			if (EC == MC_OK)
			{
				var.setInteger32Value((unsigned int)(att*10));
				printf("%f db", att);
			}
		}
		else
		{ // set
			att = var.m_iIntegerValue/10.;
			printf("%f db", att);
			EC = pConverter->SetUpInputAtt(att);
			if (EC == MC_OK)
				printf(", result = %f db", att);
		}
	}

	if (EC == MC_DEVICE_NOT_RESPONDING)
	{
		printf("NO REPLY FROM UP CONVERTER");
	}
	printf("\n");

	return EC;
}
