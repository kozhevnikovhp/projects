Program modell;
Uses Graph,CRT;
Const rad=4;Const XMAX=600;const YMAX=470;const XMIN=1;const YMIN=1;
Const temp1=1.2;const temp2=1;
const m=2;const m2=1;
const alpha=0.97;const dt=1;
Var Device,Mode,i,j,left,right,schet:Integer;vsredlev,vsredpr,JL,JP:real ;
X:array[1..m] of integer;Y:array[1..m] of integer;VX:array[1..m]of real;VY:array[1..m] of real;VLEV:array[1..m] of real;
VPRAV:array[1..m]of real;
Procedure collide;
begin
{Vold:=sqrt((sqr(vx[i])+sqr(vy[i])));
Vnew:=sqrt(alpha*(sqr(vx[i])+sqr(vy[i]))); }
   vx[i]:=-vx[i]*alpha;
   vy[i]:=-vy[i]*alpha;
   vx[j]:=-vx[j]*alpha;
   vy[j]:=-vy[j]*alpha;

{vx[i]:=-sqrt(sqr(vx[i])*alpha);vx[j]:=-sqrt(sqr(vx[j])*alpha);
vy[i]:=-sqrt(sqr(vy[i])*alpha);vy[j]:=-sqrt(sqr(vy[j])*alpha);}
end;
Begin
Device:=0;
InitGraph(Device,Mode,'C:\TP\BGI');
DirectVideo:=false;
SetColor(3);Rectangle(XMIN,YMIN,XMAX,YMAX);SetLineStyle(0, 0,ThickWidth);setcolor(3);line(300,1,300,80);
line(300,120,300,470);
SetFillStyle(1, black);
randomize;
For i:=1 to m2 do x[i]:=4+random(250);
For i:=m2+1 to m do x[i]:=309+random(250);
for i:=1 to m do y[i]:=10+random(450);

for i:=1 to m do vx[i]:=-12+random(24);
for i:=1 to m do vy[i]:=-12+random(24);
for i:=1 to m do for j:=i+1 to m do begin
if (x[j]<=x[i]+rad)and (x[j]>=x[i]-rad)then begin x[j]:=x[j]+rad;end;
if (y[j]<=y[i]+rad)and(y[j]>=y[i]-rad)then begin y[j]:=y[j]-rad;end;
if (x[i]<=310)and(y[i]<=80)and(x[i]>=293) then  x[i]:=x[i]+rad;
if (x[i]<=310)and(y[i]>=120)and(x[i]>=293) then  x[i]:=x[i]-rad;
if x[i]>600 then x[i]:=x[i]-4*rad;
end;
repeat
  right:=0;left:=0;vsredlev:=0;vsredpr:=0;
  for i:=1 to m do begin if x[i]<=300 then vlev[i]:=sqrt(sqr(vx[i])+sqr(vy[i])) else vlev[i]:=0;end;
  for i:=1 to m do begin if x[i]>=300 then vprav[i]:=sqrt(sqr(vx[i])+sqr(vy[i])) else vprav[i]:=0;end;
 For i:=1 to m do begin SetlineStyle(0,0,NormWidth);
 SetColor(White);Circle(x[i],y[i],rad);
 delay(20000);
{ SetColor(black);Circle(x[i],y[i],rad);}
 x[i]:=x[i]+round(vx[i]*dt);
 y[i]:=y[i]+round(vy[i]*dt);
 if i=m then
{ begin FloodFill(30,30,3); FloodFill(500,30,3);end;}
 end;
  For i:=1 to m do If x[i]>=301 then right:=right+1;
  For i:=1 to m do If x[i]<=300   then left:=left+1;
  for i:=1 to left do begin  vsredlev:= vsredlev+vlev[i];end;
  for i:=1 to m do   begin  vsredpr:= vsredpr+vprav[i];end;
  JL:=((left/(300*470))*vsredlev*40)/4;
   JP:=((right/(300*470))*vsredpr*40)/4;
  for i:=1 to m do
  If x[i]>=XMAX-3*rad then  vx[i]:=-abs(vx[i]);
  for i:=1 to m do
  If x[i]<=XMIN+3*rad then  vx[i]:=abs(vx[i]);
  for i:=1 to m do
  If y[i]<=YMIN-3*rad then vy[i]:=abs((temp1*(vy[i])));
  for i:=1 to m do
  If y[i]>=YMAX+3*rad then vy[i]:=-abs((temp2*(vy[i])));
   for i:=1 to m do if (x[i]>=295)and (x[i]<=305)and(y[i]>=120)then   vx[i]:=-vx[i];
   for i:=1 to m do if  (x[i]>=295)and (x[i]<=305) and (y[i]<=80) then vx[i]:=-vx[i];
   { for i:=1 to m do if  (x[i]>=295)and (x[i]<=310) and (y[i]>=120)and (y[i]<=126) then  vy[i]:=-vy[i];}

   For i:=1 to m do
 for j:=i+1 to m do begin
 If (abs(x[i]-x[j])<=2*rad) and (abs(y[i]-y[j])<=2*rad)
 then begin collide;schet:=schet+1;end;
 {read(x[i]);}
  begin
  GOTOXY(60,2);write(x[i],'  ',y[i]); GOTOXY(60,3);write(x[j],'  ',y[j]);
  GOTOXY(60,4);write((2*rad));
  GOTOXY(60,6);write('stolknovenei ',schet);GOTOXY(60,7);write('left=',left:2); GOTOXY(60,8);write('right=',right:2);
{  GOTOXY(55,9);write('sred skorost slevo  ',vsredlev/left:2:2);
  GOTOXY(55,10);write('sred skorost sprava  ',vsredpr/right:2:2);} GOTOXY(51,11);write('potok s levo na prava  ',JL:2:4);
  GOTOXY(51,12);write('potok s prava na levo  ',JP:2:4)
  end;
 end;

until 2>3;



Readln;
end.

