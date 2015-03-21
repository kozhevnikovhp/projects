#include "rgba.h"

namespace common {

namespace portable {

///////////////////////////////////////////////////////////////////
// SRGBA

SRGBA::SRGBA()
{
        m_uiRGBA = 0;
}

void SRGBA::InitAndHighlite(SRGBA *pRGBA, double fFactor)
{
        unsigned char r, g, b, a;

        if (pRGBA->R() * fFactor > 0xFF)
                r = 0xFF;
        else
                r = (unsigned char)(pRGBA->R()*fFactor);

        if (pRGBA->G() * fFactor > 0xFF)
                g = 0xFF;
        else
                g = (unsigned char)(pRGBA->G()*fFactor);

        if (pRGBA->B() * fFactor > 0xFF)
                b = 0xFF;
        else
                b = (unsigned char)(pRGBA->B()*fFactor);

        if (pRGBA->A() * fFactor > 0xFF)
                a = 0xFF;
        else
                a = (unsigned char)(pRGBA->A()*fFactor);

        Init(r, g, b, a);
}

void SRGBA::Init(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
        m_ucRGBA[0] = r;
        m_ucRGBA[1] = g;
        m_ucRGBA[2] = b;
        m_ucRGBA[3] = a;
}

/*unsigned char double2char(double x)
{
        if (x < 0.)
                x = 0.;
        unsigned char _x = (unsigned char)(x*256+0.49999);
        return _x;
}

void SRGBA::Init(double r, double g, double b, double a)
{
        Init(double2char(r), double2char(g), double2char(b), double2char(a));
}*/

} // namespace portable

} // namespace common


