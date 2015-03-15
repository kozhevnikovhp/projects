#include "stdafx.h"
#include "../Common/SDMS_OIDs.h"
#include "PozharskyDownConverter.h"

MC_ErrorCode processPozharskyDownRequest(CPozharskyUpDownConverter *pConverter, cSnmpVariable &var, bool bGet)
{
	MC_ErrorCode EC = MC_DEVICE_NOT_RESPONDING;

	printf("\tdown converter\n");
	if (var.m_OID.isPartOfOID(OidPozharskyDownOnOff, OidPozharskyDownOnOffLen))
	{
		printf("\t\tenabled ");
		BOOL bOn;
		if (bGet)
		{ // get
			EC = pConverter->IsDownTurnedOn(bOn);
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
			EC = pConverter->TurnDownOn(bOn);
			if (EC == MC_OK)
			{
				printf(" result = ");
				if (bOn)
					printf("ON");
				else
					printf("OFF");
			}
		}
	}
	else if (var.m_OID.isPartOfOID(OidPozharskyDownFrequency, OidPozharskyDownFrequencyLen))
	{
		printf("\t\tfrequency ");
		unsigned int frequency;
		if (bGet)
		{ // get
			EC = pConverter->GetDownInputFrequency(frequency);
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
			EC = pConverter->SetDownInputFrequency(frequency);
			if (EC == MC_OK)
				printf(" result = %d MHz", frequency);
		}
	}
	else if (var.m_OID.isPartOfOID(OidPozharskyDownAttenuator, OidPozharskyDownAttenuatorLen))
	{
		printf("\t\tattenuator ");
		double att;
		if (bGet)
		{ // get
			EC = pConverter->GetDownInputAtt(att);
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
			EC = pConverter->SetDownInputAtt(att);
			if (EC == MC_OK)
				printf(", result = %f db", att);
		}
	}

	if (EC == MC_DEVICE_NOT_RESPONDING)
	{
		printf("NO REPLY FROM DOWN CONVERTER");
	}
	printf("\n");

	return EC;
}
