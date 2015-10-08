#include "stdafx.h"
#include "DeviceFromName.h"

CDevice *DeviceFromName(CString &DeviceName)
{
	CDevice *pDevice = NULL;
	
	if (!DeviceName.Compare(CDatumPsm4900_70MHz::m_pszName))
		pDevice = new CDatumPsm4900_70MHz;
	else if (!DeviceName.Compare(CDatumPsm4900_140MHz::m_pszName))
		pDevice = new CDatumPsm4900_140MHz;
	else if (!DeviceName.Compare(CDatumPsm4900_Hybrid::m_pszName))
		pDevice = new CDatumPsm4900_Hybrid;
	else if (!DeviceName.Compare(CDatumPsm4900_LBand::m_pszName))
		pDevice = new CDatumPsm4900_LBand;
	else if (!DeviceName.Compare(CDatumPsm500_LBand::m_pszName))
		pDevice = new CDatumPsm500_LBand;
	else if (!DeviceName.Compare(CDatum_M7_LMod_LDem::m_pszName))
		pDevice = new CDatum_M7_LMod_LDem;
	//else if (!DeviceName.Compare(CSdmsVsat4900L::m_pszName))
	//	pDevice = new CSdmsVsat4900L;
	//else if (!DeviceName.Compare(CSdmsVsatM500L::m_pszName))
	//	pDevice = new CSdmsVsatM500L;
	else if (!DeviceName.Compare(CNps3e::m_pszName))
		pDevice = new CNps3e;
	else if (!DeviceName.Compare(CWaveSatWtr70::m_pszName))
		pDevice = new CWaveSatWtr70;
	else if (!DeviceName.Compare(CWaveSatSubstitutor::m_pszName))
		pDevice = new CWaveSatSubstitutor;
	else if (!DeviceName.Compare(CAdvantechWty1::m_pszName))
		pDevice = new CAdvantechWty1;
	else if (!DeviceName.Compare(CPozharskyUpConverterKvv140Ver2002::m_pszName))
		pDevice = new CPozharskyUpConverterKvv140Ver2002;
	else if (!DeviceName.Compare(CPozharskyUpConverterKvv140Ver2004::m_pszName))
		pDevice = new CPozharskyUpConverterKvv140Ver2004;
	else if (!DeviceName.Compare(CPozharskyUpConverterKvv140Ver2011::m_pszName))
		pDevice = new CPozharskyUpConverterKvv140Ver2011;
	else if (!DeviceName.Compare(CPozharskyDownConverterKvn140Ver2002::m_pszName))
		pDevice = new CPozharskyDownConverterKvn140Ver2002;
	else if (!DeviceName.Compare(CPozharskyDownConverterKvn140Ver2004::m_pszName))
		pDevice = new CPozharskyDownConverterKvn140Ver2004;
	else if (!DeviceName.Compare(CPozharskyDownConverterKvn140Ver2011::m_pszName))
		pDevice = new CPozharskyDownConverterKvn140Ver2011;
	else if (!DeviceName.Compare(CNx300::m_pszName))
		pDevice = new CNx300;
	else if (!DeviceName.Compare(CAgilentE4418B::m_pszName))
		pDevice = new CAgilentE4418B;
	else if (!DeviceName.Compare(CAgilent53150A::m_pszName))
		pDevice = new CAgilent53150A;
	else if (!DeviceName.Compare(CVertex50W::m_pszName))
		pDevice = new CVertex50W;
	else if (!DeviceName.Compare(CVertexSubstitutor::m_pszName))
		pDevice = new CVertexSubstitutor;

	return pDevice;
}
