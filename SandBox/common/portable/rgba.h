#ifndef RGBA_H
#define RGBA_H

namespace common {

namespace portable {

typedef union SRGBA
{
//Constuctors/destructors
public:
        SRGBA();

// Public methods
public:
        void Init(SRGBA *pRGBA) { m_uiRGBA = pRGBA->m_uiRGBA; }
        void Init(SRGBA &RGBA) { Init(&RGBA); }
        void InitAndHighlite(SRGBA *pRGBA, double fFactor);
        void InitAndHighlite(SRGBA &RGBA, double fFactor) { InitAndHighlite(&RGBA, fFactor); }
        void Init(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
//	void Init(double r, double g, double b, double a = 1.);
        void Init(unsigned int RGBA) { m_uiRGBA = RGBA; }
        unsigned int GetRGBA() { return m_uiRGBA; }
        unsigned char *GetRGBAptr() { return m_ucRGBA; }
        unsigned char R() { return m_ucRGBA[0]; }
        unsigned char G() { return m_ucRGBA[1]; }
        unsigned char B() { return m_ucRGBA[2]; }
        unsigned char A() { return m_ucRGBA[3]; }

// Public members
public:

// Protected methods
protected:

// Protected overridables
protected:

// Protected members
protected:
        unsigned char m_ucRGBA[4];
        unsigned int m_uiRGBA;

// Private methods
private:

// Private overridables
private:

// Private members
private:
} SRGBA;

} // namespace portable

} // namespace common

#endif // RGBA_H
