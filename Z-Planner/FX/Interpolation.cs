using System;
using System.Collections.Generic;
using System.Linq;

namespace ZZero.ZPlanner.FXInterface
{
    class PWL2
    {
        public class P
        {
            public double x, y;
            public P(double a, double b)
            {
                x = a; y = b;
            }
        }

        public class Fz
        {
            public double z;
            public List<P> data;

            public Fz(double a)
            {
                z = a;
                data = new List<P>();
            }

            public void Add(double a, double f)
            {
                data.Add(new P(a, f));
            }

            public void Sort()
            {
                data.Sort((a, b) => a.x.CompareTo(b.x));
            }

            public double Interpolate(double arg)
            {
                //extrapolation for large intervals ==> const
                double xmin = data[0].x;
                double xmax = data[data.Count - 1].x;
                if (arg <= xmin) return data[0].y;
                if (arg >= xmax) return data[data.Count - 1].y;

                P p1 = data[0];
                for (int i = 1; i < data.Count; i++)
                {
                    P p2 = data[i];
                    if (arg >= p1.x && arg < p2.x)
                    {
                        double func = p1.y + (arg - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
                        return func;
                    }
                    p1 = p2;
                }
                //shouldn't get here
                return 0;
            }
        }

        List<Fz> functionFamily;
        
        public PWL2()
        {
            functionFamily = new List<Fz>();
        }

        public Fz NewFz(double z)
        {
            functionFamily.Add(new Fz(z));
            return functionFamily[functionFamily.Count-1];
        }

        public List<P> Interpolate(double z)
        {
            //sort function family by z
            functionFamily.Sort((a, b) => a.z.CompareTo(b.z));

            //sort each function by x
            foreach (Fz f in functionFamily)
            {
                f.Sort();
            }

            //interpolate
            //1. find Fz1, Fz2 pair containing z value
            Fz Fz1 = null, Fz2 = null;
            double zmin = functionFamily[0].z;
            double zmax = functionFamily[functionFamily.Count - 1].z;
            if (z <= zmin)
            {
                Fz2 = functionFamily[0];
            }
            if (z >= zmax)
            {
                Fz1 = functionFamily[0];
            }

            //inside the range
            if (Fz1 == null && Fz2 == null)
            {
                bool bValid = false;
                Fz1 = functionFamily[0];
                Fz2 = null;
                for (int i = 1; i < functionFamily.Count; i++)
                {
                    Fz2 = functionFamily[i];
                    if (z >= Fz1.z && z < Fz2.z)
                    {
                        bValid = true;
                        break;
                    }
                    Fz1 = Fz2;
                }
                if (!bValid)
                {
                    Fz1 = null;
                    Fz2 = null;
                }
            }

            //2. get all 
            SortedSet<double> args = new SortedSet<double>();
            if (Fz1 != null){
                foreach (P p in Fz1.data)
                {
                    args.Add(p.x);
                }
            }
            if (Fz2 != null)
            {
                foreach (P p in Fz2.data)
                {
                    args.Add(p.x);
                }
            }
            List<P> pwlFunc = new List<P>();
            foreach (double x in args)
            {
                //3. interpolate by x
                if (Fz1 == null && Fz2 == null)
                {
                    //shouldn't get here
                    pwlFunc.Add(new P(x,0));
                }

                //outside the range
                if (Fz1 == null && Fz2 != null)
                {
                    pwlFunc.Add(new P(x, Fz2.Interpolate(x)));
                    //return Fz2.Interpolate(x);
                }

                if (Fz1 != null && Fz2 == null)
                {
                    pwlFunc.Add(new P(x, Fz1.Interpolate(x)));
                    //return Fz1.Interpolate(x);
                }

                if (Fz1 != null && Fz2 != null)
                {
                    //inside
                    double y1 = Fz1.Interpolate(x);
                    double y2 = Fz2.Interpolate(x);

                    //4. interpolate by z
                    pwlFunc.Add(new P(x, y1 + (y2 - y1) * (z - Fz1.z) / (Fz2.z - Fz1.z)));
                    //return y1 + (y2 - y1) * z / (Fz2.z - Fz1.z);
                }
            }

            return pwlFunc;
        }

        public double Interpolate(double x, double z)
        {
            //sort function family by z
            functionFamily.Sort((a, b) => a.z.CompareTo(b.z));

            //sort each function by x
            foreach (Fz f in functionFamily)
            {
                f.Sort();
            }

            //interpolate
            //1. find Fz1, Fz2 pair containing z value
            Fz Fz1 = null, Fz2 = null;
            double zmin = functionFamily[0].z;
            double zmax = functionFamily[functionFamily.Count - 1].z;
            if (z <= zmin)
            {
                Fz2 = functionFamily[0];
            }
            if (z >= zmax)
            {
                Fz1 = functionFamily[0];
            }

            //inside the range
            if (Fz1 == null && Fz2 == null)
            {
                bool bValid = false;
                Fz1 = functionFamily[0];
                Fz2 = null;
                for (int i = 1; i < functionFamily.Count; i++)
                {
                    Fz2 = functionFamily[i];
                    if (z >= Fz1.z && z < Fz2.z)
                    {
                        bValid = true;
                        break;
                    }
                    Fz1 = Fz2;
                }
                if (!bValid)
                {
                    Fz1 = null;
                    Fz2 = null;
                }
            }
    
            //2. interpolate by x
            if (Fz1 == null && Fz2 == null)
            {
                //shouldn't get here
                return 0;
            }

            //outside the range
            if (Fz1 == null)
            {
                return Fz2.Interpolate(x);
            }

            if (Fz2 == null)
            {
                return Fz1.Interpolate(x);
            }

            //inside
            double y1 = Fz1.Interpolate(x);
            double y2 = Fz2.Interpolate(x);

            //3. interpolate by z
            if (Fz2.z == Fz1.z) return y1;
            return y1 + (y2 - y1)*(z - Fz1.z) / (Fz2.z - Fz1.z);
        }
    }

    class PWL //piece-wise linear
    {
        class P
        {
            public double x, y;
            public P(double a, double b)
            {
                x = a; y = b;
            }
        }
        List<P> data;

        public PWL()
        {
            data = new List<P>();
        }
        public void Add(double a, double f)
        {
            data.Add(new P(a, f));
        }

        public double Interpolate(double arg)
        {
            data.Sort((a, b) => a.x.CompareTo(b.x));

            //extrapolation for large intervals ==> const
            double xmin = data[0].x;
            double xmax = data[data.Count - 1].x;
            if (arg <= xmin) return data[0].y;
            if (arg >= xmax) return data[data.Count - 1].y;

            P p1 = data[0];
            for (int i = 1; i < data.Count; i++)
            {
                P p2 = data[i];
                if (arg >= p1.x && arg < p2.x)
                {
                    double func = p1.y + (arg - p1.x)*(p2.y - p1.y) / (p2.x - p1.x);               
                    return func;
                }
                p1 = p2;
            }
            //shouldn't get here
            return 0;
        }
    }

    class NewtonPoly
    {
        class P
        {
            public double x, c;
            public P(double a, double b)
            {
                x = a; c = b;
            }
        }
        List<P> data;
        List<double> x;
        List<double> c;

        public NewtonPoly()
        {
            data = new List<P>();
            x = new List<double>();
            c = new List<double>();
        }

        public void Add(double a, double f)
        {
            data.Add(new P(a, f));
            //x.Add(a);
            //c.Add(f);
        }

        private void Init()
        {
            x.Clear();
            c.Clear();

            foreach (var point in data.OrderBy(p => p.x))
            {
                x.Add(point.x);
                c.Add(point.c);                
            }
        }
        public double F(double a)
        {
            Init();
            //check min/max values
            double xmin = x[0];
            double xmax = x[x.Count-1];
            if (a <= xmin) return c[0];
            if (a >= xmax) return c[c.Count-1];

            Coeffs();
            return Calc(a);
        }

        private void Coeffs()
        {
            int n = x.Count - 1;
            for (int j = 1; j <= n; j++)
            {
                for (int i = 0; i <= n - j; i++)
                {
                    if (x[i + j] == x[i])
                    {
                        throw new Exception("duplicate x values encountered");
                    }
                    c[i] = (c[i + 1] - c[i]) / (x[i + j] - x[i]);
                }
            }
        }

        private double Calc(double a)
        {
            int n = x.Count-1;
            double fa = c[0];
            for (int i = 1; i <= n; i++)
            {
                fa = fa * (a - x[i]) + c[i];
            }

            return fa;
        }

    }

   
//-- https://en.wikipedia.org/w/index.php?title=Spline_%28mathematics%29&oldid=288288033#Algorithm_for_computing_natural_cubic_splines
    class Spline
    {
        private List<SplineSet> coeffs;

        private struct xy
        {
            public double x;
            public double y;
        }
        private List<xy> data;

        private List<double> x;
        private List<double> y;

        private double xmin;
        private double xmax;

        public Spline()
        {
            coeffs = null;

            x = new List<double>();
            y = new List<double>();
            data = new List<xy>();
        }

        private struct SplineSet
        {
            public double a;
            public double b;
            public double c;
            public double d;
            public double x;
        }

        public void Add(double xVal, double yVal)
        {
            xy val = new xy();
            val.x = xVal;
            val.y = yVal;
            data.Add(val);
            //x.Add(xVal);
            //y.Add(yVal);
        }

        public void Define()
        {
            // sort data
            data.Sort((a, b) => a.x.CompareTo(b.x));
            foreach (xy val in data)
            {
                x.Add(val.x);
                y.Add(val.y);
            }
            xmin = x[0];
            xmax = x[x.Count - 1];
            coeffs = spline(x, y);
        }

        public double GetXMin() { return xmin; }
        public double GetXMax() { return xmax; }

        private List<SplineSet> spline(List<double> x, List<double> y)
        {
            int n = x.Count - 1; //n+1 points 0..n
            //check count > min
            //....

            List<double> a = new List<double>(y);

            double[] b = new double [n];
            double[] d = new double [n];
            double[] h = new double[n];

            for (int i = 0; i < n; i++){
                h[i] = x[i+1] - x[i];
            }

            double[] alpha = new double[n];
            for (int i = 1; i < n; i++){
                alpha[i] = 3*(a[i+1]-a[i])/h[i] - 3*(a[i]-a[i-1])/h[i-1];
            }
            double[] c = new  double [n+1];
            double[] l = new  double [n+1];
            double[] mu = new  double [n+1];
            double[] z = new  double [n+1];

            l[0] = 1;
            mu[0] = 0;
            z[0] = 0;

            for(int i = 1; i < n; ++i)
            {
                l[i] = 2 *(x[i+1]-x[i-1])-h[i-1]*mu[i-1];
                mu[i] = h[i]/l[i];
                z[i] = (alpha[i]-h[i-1]*z[i-1])/l[i];
            }

            l[n] = 1;
            z[n] = 0;
            c[n] = 0;

            for(int j = n-1; j >= 0; --j)
            {
                c[j] = z [j] - mu[j] * c[j+1];
                b[j] = (a[j+1]-a[j])/h[j]-h[j]*(c[j+1]+2*c[j])/3;
                d[j] = (c[j+1]-c[j])/3/h[j];
            }

            List<SplineSet> output_set = new List<SplineSet>();
            for(int i = 0; i < n; ++i)
            {
                SplineSet s = new SplineSet();

                s.a = a[i];
                s.b = b[i];
                s.c = c[i];
                s.d = d[i];
                s.x = x[i];

                output_set.Add(s);
            }
            //dummy for the last point
            SplineSet ss = new SplineSet();

            ss.a = y[n];
            ss.b = 0;
            ss.c = 0;
            ss.d = 0;
            ss.x = x[n];
            output_set.Add(ss);

            return output_set;
        }

        //
        public double Interpolate(double xV)
        {
            //extrapolation for large intervals ==> const
            if (xV <= xmin) return coeffs[0].a;
            if (xV >= xmax) return coeffs[coeffs.Count - 1].a;
            
            for (int i = 0; i < coeffs.Count; i++)
            {
                if (xV >= coeffs[i].x && xV < coeffs[i + 1].x)
                {
                    SplineSet s = coeffs[i];
                    double delta = xV - s.x;
                    double retval = s.a;
                    retval += s.b * delta;
                    retval += s.c * delta * delta;
                    retval += s.d * delta * delta * delta;

                    return retval;
                }
            }

            return 0; //shouldn't get here
        }

    }//spline
}