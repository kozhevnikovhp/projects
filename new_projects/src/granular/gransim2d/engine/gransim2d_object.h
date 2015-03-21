#ifndef GRANSIM2D_OBJECT_H
#define GRANSIM2D_OBJECT_H

#include <stdio.h>
#include "widget_desc.h"
#include "portable/text_string.h"
#include "geometry2d/geometry2d.h"

// Forward declarations
class cGranSim2DDesign;

class CGranSim2DObject
{
	friend class cGranSim2DDesign;
//Constuctors/destructors
public:
    CGranSim2DObject();
    virtual ~CGranSim2DObject();

// Public methods
public:
	static cGranSim2DDesign *GetDesign() { return m_pDesign; }
    char *GetFullName();
    char *GetGivenName() { return m_GivenName.GetText(); }
    void SetGivenName(const char *pszName);
    void SetGivenName(const common::portable::CTextString &Name) { SetGivenName(Name.GetText()); }
    bool IsChecked(unsigned int uCheckTime) { return (m_uCheckTime == uCheckTime); }
    void Checked(unsigned int uCheckTime) { m_uCheckTime = uCheckTime; }

// Public overridables
public:
    virtual common::geometry2d::CGeometry2D *GetGeometry() { return NULL; }
    virtual const char *GetTypeDesc() = 0;
    virtual int GetFormCount() = 0;
    virtual SFormDesc *GetFormDesc(int iFormNumber) = 0;
    virtual bool IsSelected() { return false; }
    virtual void Select() {}
    virtual void Unselect() {}
    virtual void SaveToFile(FILE *fd) = 0;
    virtual void DeleteFromDesign() = 0;

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
	static cGranSim2DDesign *m_pDesign;
    static int m_nObjectCount;
    common::portable::CTextString m_GivenName;
    unsigned int m_uCheckTime;
    char m_szFullName[256];

// Private methods
private:

// Private overridables
private:

// Private members
private:

};


#endif // GRANSIM2D_OBJECT_H
