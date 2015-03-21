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
Функция Бесселя нулевого порядка.

Относительная погрешность вычисления:
arithmetic   domain     # trials      peak         rms
   IEEE      0, 30       60000       4.2e-16     1.1e-16

Входные параметры:
    Х

Допустимые значения:
    -inf < X < +inf

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
*************************************************************************/
double besselj0(double x);


/*************************************************************************
Функция Бесселя первого порядка.

Абсолютная погрешность:
arithmetic   domain      # trials      peak         rms
   IEEE      0, 30       30000       2.6e-16     1.1e-16

Входные параметры:
    Х

Допустимые значения:
    -inf < X < inf

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
*************************************************************************/
double besselj1(double x);


/*************************************************************************
Функция Бесселя N-ого порядка первого рода

Входные параметры:
    N   -   порядок функции
    Х   -   аргумент

Допустимые значения
    -inf < X < +inf

Абсолютная погрешность
arithmetic   range      # trials      peak         rms
   IEEE      0, 30        5000       4.4e-16     7.9e-17

Функция недостаточно точна в области крупных n или x. В таких случаях
предпочтительным является использование алгоритма вычисления функции
Бесселя дробного порядка с теми же параметрами.

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besseljn(int n, double x);


/*************************************************************************
Функция Бесселя нулевого порядка второго рода.

Погрешность (абсолютная, если |Y0|<1, иначе относительная):
arithmetic   domain     # trials      peak         rms
   IEEE      0, 30       30000       1.3e-15     1.6e-16

Входные параметры:
    Х

Допустимые значения
    X>0

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
*************************************************************************/
double bessely0(double x);


/*************************************************************************
Функция Бесселя первого порядка второго рода.

Абсолютная погрешность вычисления:
arithmetic   domain      # trials      peak         rms
   IEEE      0, 30       30000       1.0e-15     1.3e-16

Входные параметры:
    Х

Допустимые значения:
    X>0

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
*************************************************************************/
double bessely1(double x);


/*************************************************************************
Функция Бесселя N-ого порядка второго рода.

Входные параметры:
    N   -   порядок функции
    Х   -   аргумент

Допустимые значения
    X<>0

Если |YN|<=1, то цифры в таблице ниже обозначают абсолютную
погрешность, иначе они обозначают относительную погрешность.

ОБЛАСТЬ     #ТЕСТОВ         МАКС.       СРЕДН.
0,30        30000           3.4e-15     4.3e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besselyn(int n, double x);


/*************************************************************************
Функция Бесселя I0

Входные параметры:
    X   -   аргумент

Результат:
    значение модифицированной функции бесселя I0(X)

Относительная погрешность:
arithmetic   domain     # trials      peak         rms
   IEEE      0,30        30000       5.8e-16     1.4e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besseli0(double x);


/*************************************************************************
Функция Бесселя I1

Входные параметры:
    X   -   аргумент

Результат:
    значение модифицированной функции бесселя I1(X)

Относительная погрешность:
ОБЛАСТЬ     #ТЕСТОВ         МАКС.       СРЕДН.КВ.
0, 30       30000           1.9e-15     2.1e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1985, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besseli1(double x);


/*************************************************************************
Функция Бесселя K0

Входные параметры:
    X   -   аргумент

Результат:
    значение модифицированной функции бесселя K0(X)

Относительная погрешность:
ОБЛАСТЬ     #ТЕСТОВ         МАКС.       СРЕДН.
0, 30       30000           1.2e-15     1.6e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besselk0(double x);


/*************************************************************************
Функция Бесселя K1

Входные параметры:
    X   -   аргумент

Результат:
    значение модифицированной функции бесселя K1(X)

Относительная погрешность:
ОБЛАСТЬ     #ТЕСТОВ         МАКС.       СРЕДН.КВ.
0, 30       30000           1.2e-15     1.6e-16

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double besselk1(double x);


/*************************************************************************
Функция Бесселя KN

Входные параметры:
    nn  -   порядок функции
    X   -   аргумент

Результат:
    значение модифицированной функции бесселя KN(X)

Относительная погрешность:
ОБЛАСТЬ     #ТЕСТОВ         МАКС.       СРЕДН.КВ.
0,30          90000         1.8e-8      3.0e-10

Ошибка наиболее высока в окрестностях x = 9.55, в точке, где происходит
переключение между двумя используемыми разложениями.

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1988, 2000 by Stephen L. Moshier
*************************************************************************/
double besselkn(int nn, double x);


#endif
