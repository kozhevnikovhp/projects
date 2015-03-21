/*************************************************************************
Cephes Math Library Release 2.8:  June, 2000
Copyright by Stephen L. Moshier

Contributors:
    * Sergey Bochkanov (ALGLIB project). Translation from C to
      pseudocode.

See subroutines comments for additional copyrights.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.

- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************/

#ifndef _bessel_h
#define _bessel_h

//#include "ap.h"

/*************************************************************************
������� ������� �������� �������.

������������� ����������� ����������:
arithmetic   domain     # trials      peak         rms
   IEEE      0, 30       60000       4.2e-16     1.1e-16

������� ���������:
    �

���������� ��������:
    -inf < X < +inf

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
*************************************************************************/
double besselj0(double x);


/*************************************************************************
������� ������� ������� �������.

���������� �����������:
arithmetic   domain      # trials      peak         rms
   IEEE      0, 30       30000       2.6e-16     1.1e-16

������� ���������:
    �

���������� ��������:
    -inf < X < inf

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
*************************************************************************/
double besselj1(double x);


/*************************************************************************
������� ������� N-��� ������� ������� ����

������� ���������:
    N   -   ������� �������
    �   -   ��������

���������� ��������
    -inf < X < +inf

���������� �����������
arithmetic   range      # trials      peak         rms
   IEEE      0, 30        5000       4.4e-16     7.9e-17

������� ������������ ����� � ������� ������� n ��� x. � ����� �������
���������������� �������� ������������� ��������� ���������� �������
������� �������� ������� � ���� �� �����������.

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besseljn(int n, double x);


/*************************************************************************
������� ������� �������� ������� ������� ����.

����������� (����������, ���� |Y0|<1, ����� �������������):
arithmetic   domain     # trials      peak         rms
   IEEE      0, 30       30000       1.3e-15     1.6e-16

������� ���������:
    �

���������� ��������
    X>0

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
*************************************************************************/
double bessely0(double x);


/*************************************************************************
������� ������� ������� ������� ������� ����.

���������� ����������� ����������:
arithmetic   domain      # trials      peak         rms
   IEEE      0, 30       30000       1.0e-15     1.3e-16

������� ���������:
    �

���������� ��������:
    X>0

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
*************************************************************************/
double bessely1(double x);


/*************************************************************************
������� ������� N-��� ������� ������� ����.

������� ���������:
    N   -   ������� �������
    �   -   ��������

���������� ��������
    X<>0

���� |YN|<=1, �� ����� � ������� ���� ���������� ����������
�����������, ����� ��� ���������� ������������� �����������.

�������     #������         ����.       �����.
0,30        30000           3.4e-15     4.3e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besselyn(int n, double x);


/*************************************************************************
������� ������� I0

������� ���������:
    X   -   ��������

���������:
    �������� ���������������� ������� ������� I0(X)

������������� �����������:
arithmetic   domain     # trials      peak         rms
   IEEE      0,30        30000       5.8e-16     1.4e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besseli0(double x);


/*************************************************************************
������� ������� I1

������� ���������:
    X   -   ��������

���������:
    �������� ���������������� ������� ������� I1(X)

������������� �����������:
�������     #������         ����.       �����.��.
0, 30       30000           1.9e-15     2.1e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1985, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besseli1(double x);


/*************************************************************************
������� ������� K0

������� ���������:
    X   -   ��������

���������:
    �������� ���������������� ������� ������� K0(X)

������������� �����������:
�������     #������         ����.       �����.
0, 30       30000           1.2e-15     1.6e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besselk0(double x);


/*************************************************************************
������� ������� K1

������� ���������:
    X   -   ��������

���������:
    �������� ���������������� ������� ������� K1(X)

������������� �����������:
�������     #������         ����.       �����.��.
0, 30       30000           1.2e-15     1.6e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besselk1(double x);


/*************************************************************************
������� ������� KN

������� ���������:
    nn  -   ������� �������
    X   -   ��������

���������:
    �������� ���������������� ������� ������� KN(X)

������������� �����������:
�������     #������         ����.       �����.��.
0,30          90000         1.8e-8      3.0e-10

������ �������� ������ � ������������ x = 9.55, � �����, ��� ����������
������������ ����� ����� ������������� ������������.

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1988, 2000 by Stephen L. Moshier
*************************************************************************/
double besselkn(int nn, double x);


#endif
