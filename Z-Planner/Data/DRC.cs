using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text.RegularExpressions;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.DRC
{
    class Problem
    {
        public enum Severity { Info, Warning, Error }
        public Problem(ZStackup stk, ZLayer lay, Severity level, string check, string text)
        {
            stackup = stk;
            layer = lay;
            severity = level;
            rule = check;
            Description = text;
        }

        public Severity severity { get; private set; }

        public string Description { get; private set; }

        public ZStackup stackup;
        public ZLayer layer;

        public string rule { get; private set; } 

        public string GetSeverity()
        {
            string ret = "";
            switch (severity)
            {
                case Severity.Error:
                    ret += "Error";
                    break;
                case Severity.Warning:
                    ret += "Warning";
                    break;
                case Severity.Info:
                    ret += "Info";
                    break;
            }
            return ret;
        }

        public string GetScope()
        {
            string ret = "";
            if (layer == null)
            {
                ZStackup z = ZPlannerManager.Project.Stackup;
                if (z != null)
                {
                    ret = "Stackup";// String.Format("Stackup: {0}", z.Title);
                }
            }
            else
            {
                ret = layer.Title;//String.Format("Row: {0}", layer.Order + 1);
            }
            return ret;
        }

        public string GetDescription()
        {
            return Description;
        }

        public string Format()
        {
            string ret = "";
            if (layer == null)
            {
                ret = "Stackup ";
            }
            else
            {
                ret = "Layer ";
            }
            switch (severity)
            {
                case Severity.Error : 
                    ret += "Error";
                    break;
                case Severity.Warning : 
                    ret += "Warning";
                    break;
                case Severity.Info : 
                    ret += "Info";
                    break;
            }
            ret += Environment.NewLine;
            ret += Description;
            ret += Environment.NewLine;
            return ret;
        }
    }

    class Checker
    {
        public List<Problem> report;

        public Checker()
        {
            report = new List<Problem>();
        }

        public string TextReport()
        {
            SortReport();
            return CreateText();
        }

        private string CreateText()
        {
            ZStackup z = ZPlannerManager.Project.Stackup;
            if (z == null)  return "";

            string ret = String.Format("Stackup: {0}", z.Title) + Environment.NewLine;
            ret += Environment.NewLine;
            bool b = false;
            foreach (Problem p in report)
            {
                ret += p.Format();
                ret += Environment.NewLine;
                b = true;
            }
            if (!b)
            {
                ret += "No problems found." + Environment.NewLine;
            }
            return ret;
        }

        public void CheckActive()
        {
            ZStackup zs = ZPlannerManager.Project.Stackup;
            if (zs != null)
            {
                CheckStackup(zs);
            }
        }

        private void CheckStackup(ZStackup stackup)
        {
            AllChecks(stackup);
        }


        private void SortReport()
        {
            //sort report;
        }

        //------------------------ checks
        private void AllChecks(ZStackup stackup)
        {

            LayerProperties(stackup);//

            NumberOfMetals(stackup);//
            LayerOrder(stackup);
            DielectricOrder(stackup);
            Symmetry(stackup);
            Planes(stackup);//at least one plane, GND and Power should alternate
            SolderMask(stackup);//

            NumberOfMaterials(stackup);
        }

        private bool CheckRange(double dval, double min, double max)
        {
            return dval >= min && dval <= max;
        }

        private bool CheckFunctionRange(string stringValue, double min, double max) //table function F(a) of the form a:F;a:F;...
        {
            double val = double.NaN;

            if (string.IsNullOrWhiteSpace(stringValue) || !stringValue.Contains(":") && !stringValue.Contains(";") && double.TryParse(stringValue.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out val))
            {//one value -> F(a) = Const
                return CheckRange(val, min, max);
            }
            else //sorted by argument table -> F(a)  a:F;a:F;a:F;..
            {
                Regex frequencyValuePattern = new Regex(@"(?<frequency>[^:;]+?):(?<value>[^:;]+?);", RegexOptions.Compiled | RegexOptions.Singleline);
                foreach (Match frequencyValueMatch in frequencyValuePattern.Matches(stringValue))
                {
                    if (!frequencyValueMatch.Success) continue;
                    string frequencyString = frequencyValueMatch.Groups["frequency"].Value;
                    string valueString = frequencyValueMatch.Groups["value"].Value;
                    double frequencyDouble;
                    if (double.TryParse(frequencyString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out frequencyDouble) &&
                        double.TryParse(valueString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out val))
                    {
                        if (!CheckRange(val, min, max)) return false;
                    }
                }
            }
            return true;
        }

        private void SolderMask(ZStackup stackup)
        {
            foreach (ZLayer zl in stackup.Layers)
            {
                int idx = stackup.GetLayerOfStackupIndex(zl.ID);

                if (zl.GetLayerType() == ZLayerType.SolderMask){
                    if (idx != 0 && idx != stackup.Layers.Count - 1)
                    {
                        Problem p = new Problem(stackup, zl, Problem.Severity.Error, "Solder Mask", "Solder mask can only be on outer layer");
                        report.Add(p);
                    }
                }
            }
        }

        enum PlaneType
        {
            PowerPlane,
            GroundPlane,
            UnknownPlane
        };

        private void Planes(ZStackup stackup)
        {
            bool bPlaneTypesAllRecognized = true; // report or not to report alternating planes?
            PlaneType lastPlaneType = PlaneType.UnknownPlane;
            int nPlanes = 0;
            bool bPowerGroundAlternate = true; // OK so far...
            int nMetals = 0;
            foreach (ZLayer zl in stackup.Layers)
            {
                bool bMetal = false;
                if (zl.isMetal())
                {
                    nMetals++;
                    bMetal = true;
                }

                bool bPlane = false;
                if (zl.GetLayerType() == ZLayerType.Plane)
                {
                    nPlanes++;
                    bPlane = true;
                }
                else if (zl.GetLayerType() == ZLayerType.SplitMixed)
                {
                    nPlanes++;
                }
                if (bPlane)
                {
                    if (zl.isGround())
                    {
                        if (lastPlaneType == PlaneType.GroundPlane)
                            bPowerGroundAlternate = false; // violation!
                        lastPlaneType = PlaneType.GroundPlane;
                    }
                    else if (zl.isPower())
                    {
                        if (lastPlaneType == PlaneType.PowerPlane)
                            bPowerGroundAlternate = false; // violation!
                        lastPlaneType = PlaneType.PowerPlane;
                    }
                    else
                    {
                        string layerName = zl.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName);
                        Problem p = new Problem(stackup, null, Problem.Severity.Warning, "", "Could not recognize type of plane '" + layerName + "'");
                        report.Add(p);
                        bPlaneTypesAllRecognized = false;
                    }
                }
                else
                {
                    if (bMetal)
                        lastPlaneType = PlaneType.UnknownPlane; // metal but no plane -> reset previous plane as if it does not exist, signal layer breaks plane pairing
                }
            }
            if (nPlanes == 0) //&& nMetals > 2)
            {
                Problem p = new Problem(stackup, null, Problem.Severity.Error, "Number of Planes", "Stackup should have at least one plane layer");
                report.Add(p);
            }
            if (bPlaneTypesAllRecognized && !bPowerGroundAlternate)
            {
                Problem p = new Problem(stackup, null, Problem.Severity.Error, "Power/Ground alternate", "Stackup should have alternating Power/Ground planes");
                report.Add(p);
            }
        }

        private void NumberOfMaterials(ZStackup stackup)
        {
            HashSet<string> materials;
            materials = new HashSet<string>();
            foreach (ZLayer zl in stackup.Layers){
                if (!zl.isMetal())
                {
                    switch (zl.GetLayerType()) 
                    {
                        case ZLayerType.Core:
                        case ZLayerType.Prepreg:
                            {
                                string mName = zl.GetLayerParameterValue(ZStringConstants.ParameterIDMaterial).Trim().ToLower();
                                if (mName.Length > 0)
                                {
                                    if ((mName != "dielectric") && (mName != "substrate")){
                                        materials.Add(mName);
                                    }
                                }
                            }
                            break;
                    }
                }
            }
            if (materials.Count > 1)
            {
                string  lam = "";
                int c = 0;
                foreach(string a in materials){
                    c++;
                    lam += a;
                    if (c != materials.Count) lam += ",";
                }
                string s = String.Format("This stackup includes {0} different PCB laminates [{1}], which can lead to board warpage.\nIt is generally advisable to use just one laminate/resin system within the same stackup.", materials.Count, lam);
                Problem p = new Problem(stackup, null, Problem.Severity.Warning, "Number of Laminates", s);
                report.Add(p);
            }
        }

        private void NumberOfMetals(ZStackup stackup)
        {
            int nMetals = 0;
            int nAll = stackup.Layers.Count;
            foreach (ZLayer zl in stackup.Layers)
            {                
                if (zl.isMetal())
                {
                    nMetals++;
                }
            }
            if (nMetals % 2 != 0)
            {
                Problem p = new Problem(stackup, null, Problem.Severity.Error, "Number of Metals", "Stackup must have even number of metal layers");
                report.Add(p);
            }
            //if (nAll % 2 == 0)
            //{
            //    Problem p = new Problem(stackup, null, Problem.Severity.Warning, "Number of Rows", "Stackup should have an odd number of rows");
            //    report.Add(p);
            //}
        }

        private void LayerProperties(ZStackup stackup)
        {
            foreach (ZLayer zl in stackup.Layers)
            {
                bool currMetal = zl.isMetal();
                ZLayerType? currType = zl.GetLayerType();
                int currIdx = stackup.GetLayerOfStackupIndex(zl.ID);
                string message = "";
                
                double dval = 0;
                if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref dval))
                {
                    //check range
                    if (!CheckRange(dval, 0.1, 30))
                    {
                        message = String.Format("Layer {0} {1} value should be in the range [{2}..{3}].", currIdx + 1, "thickness", 0.1, 30);
                        Problem p = new Problem(stackup, zl, Problem.Severity.Error, "Layer parameters", message);
                        report.Add(p);
                    }
                }
                else
                {
                    message = String.Format("Layer {0} must have {1} value defined", currIdx + 1, "thickness");
                    Problem p = new Problem(stackup, zl, Problem.Severity.Error, "Layer parameters", message);
                    report.Add(p);
                }
                if (currMetal)
                {
                    foreach (var single in stackup.Singles)
                    {
                        ZLayer zsl = stackup.GetLayerOfSingleImpedance(zl.ID, single.ID);
                        if (zsl != null)
                        {
                            if (ZPlannerProject.GetLayerParameterValue(zsl, ZStringConstants.ParameterIDZo_TraceWidth, ref dval))
                            {
                                //check range
                                if (!CheckRange(dval, 2, 100))
                                {
                                    message = String.Format("Layer {0} {1} value should be in the range [{2}..{3}].", currIdx + 1, "trace width", 2, 100);
                                    Problem p = new Problem(stackup, zsl, Problem.Severity.Error, "Layer parameters", message);
                                    report.Add(p);
                                }
                            }
                        }
                    }

                    foreach (var pair in stackup.Pairs)
                    {
                        ZLayer zpl = stackup.GetLayerOfPairImpedance(zl.ID, pair.ID);
                        if (zpl == null) continue;

                        if (ZPlannerProject.GetLayerParameterValue(zpl, ZStringConstants.ParameterIDZdiff_TraceWidth, ref dval))
                        {
                            //check range
                            if (!CheckRange(dval, 2, 100))
                            {
                                message = String.Format("Layer {0} {1} value should be in the range [{2}..{3}].", currIdx + 1, "diff trace width", 2, 100);
                                Problem p = new Problem(stackup, zpl, Problem.Severity.Error, "Layer parameters", message);
                                report.Add(p);
                            }
                        }
                        if (ZPlannerProject.GetLayerParameterValue(zpl, ZStringConstants.ParameterIDZdiff_TraceSpacing, ref dval))
                        {
                            //check range
                            if (!CheckRange(dval, 2, 100))
                            {
                                message = String.Format("Layer {0} {1} value should be in the range [{2}..{3}].", currIdx + 1, "diff trace spacing", 2, 100);
                                Problem p = new Problem(stackup, zpl, Problem.Severity.Error, "Layer parameters", message);
                                report.Add(p);
                            }
                        }
                    }
                }
                else
                {
                    foreach (var single in stackup.Singles)
                    { 
                        ZLayer zsl = stackup.GetLayerOfSingleImpedance(zl.ID, single.ID);
                        if (zsl == null) continue;
                    
                        string str = zsl.GetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant);

                        //--if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_DielectricConstant, ref dval))
                        if (!string.IsNullOrEmpty(str))
                        {
                            //check range
                            if (!CheckFunctionRange(str, 1, 7))
                            {
                                message = String.Format("Layer {0} {1} value should be in the range [{2}..{3}].", currIdx + 1, "dielectric constant", 1, 7);
                                Problem p = new Problem(stackup, zsl, Problem.Severity.Error, "Layer parameters", message);
                                report.Add(p);
                            }
                        }
                        else
                        {
                            message = String.Format("Layer {0} must have {1} value defined", currIdx + 1, "dielectric constant");
                            Problem p = new Problem(stackup, zsl, Problem.Severity.Error, "Layer parameters", message);
                            report.Add(p);
                        }

                        str = zsl.GetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent);
                        //--if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_LossTangent, ref dval))
                        if (!string.IsNullOrEmpty(str))
                        {
                            //check range
                            if (!CheckFunctionRange(str, 0.0001, 0.05))
                            {
                                message = String.Format("Layer {0} {1} value should be in the range [{2}..{3}].", currIdx + 1, "loss tangent", 0.0001, 0.05);
                                Problem p = new Problem(stackup, zsl, Problem.Severity.Error, "Layer parameters", message);
                                report.Add(p);
                            }
                        }
                        else
                        {
                            message = String.Format("Layer {0} must have {1} value defined", currIdx + 1, "loss tangent");
                            Problem p = new Problem(stackup, zsl, Problem.Severity.Error, "Layer parameters", message);
                            report.Add(p);
                        }
                    }

                    foreach (var pair in stackup.Pairs)
                    {
                        ZLayer zpl = stackup.GetLayerOfPairImpedance(zl.ID, pair.ID);
                        if (zpl == null) continue;

                        string str = zpl.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_DielectricConstant);

                        //--if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZdiff_DielectricConstant, ref dval))
                        if (!string.IsNullOrEmpty(str))
                        {
                            //check range
                            if (!CheckFunctionRange(str, 1, 7))
                            {
                                message = String.Format("Layer {0} {1} value should be in the range [{2}..{3}].", currIdx + 1, "diff dielectric constant", 1, 7);
                                Problem p = new Problem(stackup, zpl, Problem.Severity.Error, "Layer parameters", message);
                                report.Add(p);
                            }
                        }
                        else
                        {
                            message = String.Format("Layer {0} must have {1} value defined", currIdx + 1, "diff dielectric constant");
                            Problem p = new Problem(stackup, zpl, Problem.Severity.Error, "Layer parameters", message);
                            report.Add(p);
                        }

                        str = zpl.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_LossTangent);
                        //--if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZdiff_LossTangent, ref dval))
                        if (!string.IsNullOrEmpty(str))
                        {
                            //check range
                            if (!CheckFunctionRange(str, 0.0001, 0.05))
                            {
                                message = String.Format("Layer {0} {1} value should be in the range [{2}..{3}].", currIdx + 1, "diff loss tangent", 0.0001, 0.05);
                                Problem p = new Problem(stackup, zpl, Problem.Severity.Error, "Layer parameters", message);
                                report.Add(p);
                            }
                        }
                        else
                        {
                            message = String.Format("Layer {0} must have {1} value defined", currIdx + 1, "diff loss tangent");
                            Problem p = new Problem(stackup, zpl, Problem.Severity.Error, "Layer parameters", message);
                            report.Add(p);
                        }
                    }
                }
            }
        }

        //

        private void LayerOrder(ZStackup stackup)
        {
            // check correct sequence of dielectric and metal layers
            string message = "";

            ZLayerType? currType = null, prevType = null;
            bool currMetal = false, prevMetal = false;
            int currIdx = 0, prevIdx = 0;
            int numPlanes = 0;
            foreach (ZLayer zl in stackup.Layers)
            {
                currMetal = zl.isMetal();
                currType = zl.GetLayerType();
                if (currType == ZLayerType.Plane)
                {
                    numPlanes++;
                }
                currIdx = stackup.GetLayerOfStackupIndex(zl.ID);

                if (prevType != null)
                {
                    //no adjacent metal layers
                    if (currMetal)
                    {
                        if (prevMetal)
                        {
                            message = String.Format("Two adjacent metal layers: {0} and {1}.", currIdx + 1, prevIdx + 1);
                            Problem p = new Problem(stackup, zl, Problem.Severity.Error, "Layer order", message);
                            report.Add(p);
                        }
                    }
                    else //dielectric, depends on type
                    {
                        switch (currType)
                        {
                            case ZLayerType.Core:
                                //only metal before Core
                                if (!prevMetal)
                                {
                                    message = String.Format("Dielectric layer {0} adjacent to Core {1}. Consider adding a copper foil to the core.", prevIdx + 1, currIdx + 1);
                                    Problem p = new Problem(stackup, zl, Problem.Severity.Warning, "Layer order", message);
                                    report.Add(p);
                                }
                                break;

                            case ZLayerType.Prepreg:
                                //any layer before prepreg except core
                                if (prevType == ZLayerType.Core)
                                {
                                    message = String.Format("Prepreg layer {0} adjacent to Core {1}. Consider adding a copper foil to the core.", currIdx + 1, prevIdx + 1);
                                    Problem p = new Problem(stackup, zl, Problem.Severity.Warning, "Layer order", message);
                                    report.Add(p);
                                }
                                break;

                        }
                    }
                }

                prevType = currType;
                prevMetal = currMetal;   
                prevIdx = currIdx;
            }

        }

        private void Symmetry(ZStackup stackup)
        {
            Symmetry_1(stackup);
            Symmetry_2(stackup);
        }
        private void Symmetry_1(ZStackup stackup)
        {
            int nLayers = stackup.Layers.Count;
            if (nLayers % 2 == 0) return; //wrong number of rows - already reported
            int middle = nLayers / 2; //0-based
            int nUp = 0, nDown = 0;
            for (int i = 0; i < nLayers; i++)
            {
                ZLayer zl = stackup.Layers[i];
                if (zl.isMetal())
                {
                    if (i < middle) nUp++;
                    if (i > middle) nDown++;
                }
            }
            if (nUp != nDown)
            {
                string message = String.Format("Stackup is asymmetrical: there are {0} metal layers above the middle row {1} and {2} metal layers below it.", nUp, middle, nDown);
                Problem p = new Problem(stackup, null, Problem.Severity.Error, "Symmetry", message);
                report.Add(p);
            }
        }
        private void Symmetry_2(ZStackup stackup)
        {
            int nLayers = stackup.Layers.Count;
            if (nLayers % 2 == 0) return; //wrong number of rows - already reported
            int middle = nLayers / 2 + 1;//1-based
            //check row by row
            int iUp = middle, iDown = middle;
            for (int i = 1; i <= nLayers / 2; i++)
            {
                iUp   -= 1;
                iDown += 1;
                ZLayer zlUp = stackup.Layers[iUp-1];
                ZLayer zlDown = stackup.Layers[iDown-1];
                ZLayerType? tUp = zlUp.GetLayerType();
                ZLayerType? tDown = zlDown.GetLayerType();
                if (tUp != tDown)
                {
                    string message = String.Format("Stackup is asymmetrical: consider making layers {0} and {1} the same type.", iUp, iDown);
                    Problem p = new Problem(stackup, null, Problem.Severity.Error, "Symmetry", message);
                    report.Add(p);
                }

                double hUp = 0, hDown = 0;
                ZPlannerProject.GetLayerParameterValue(zlUp, ZStringConstants.ParameterIDThickness, ref hUp);
                ZPlannerProject.GetLayerParameterValue(zlDown, ZStringConstants.ParameterIDThickness, ref hDown);
                double eps = 0.0001;
                if (Math.Abs(hUp - hDown) > eps)
                {
                    string message = String.Format("Stackup is asymmetrical: consider making layers {0} and {1} the same thickness.", iUp, iDown);
                    Problem p = new Problem(stackup, null, Problem.Severity.Error, "Symmetry", message);
                    report.Add(p);
                }
            }
        }
        private void Symmetry_3(ZStackup stackup)
        {
            int nLayers = stackup.Layers.Count;
            bool bEven = nLayers % 2 == 0;
            int nHalf = nLayers / 2;
            double h1 = 0, h2 = 0;
            for (int i = 0; i < nLayers; i++)
            {
                ZLayer zl = stackup.Layers[i];
                double dval = 0;
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref dval);

                if (i < nHalf)
                {
                    h1 += dval;
                }
                else if (i > nHalf)
                {
                    h2 += dval;
                }
                else
                {
                    if (bEven)
                    {
                        h2 += dval;
                    }
                    else
                    {
                        h1 += dval / 2;
                        h2 += dval / 2;
                    }
                }
            }
            if (h1 != h2)
            {
                string message = String.Format("Stackup is asymmetrical: upper half thickness is {0}, lower half thickness is {1}.", h1, h2);
                Problem p = new Problem(stackup, null, Problem.Severity.Error, "Symmetry", message);
                report.Add(p);
            }
        }

        private void DielectricOrder(ZStackup stackup)
        {
            string message = "";
            ZLayerType? prev_type = null;
            ZLayer zprev = null;
            bool bErr = false;
            foreach (ZLayer zl in stackup.Layers)
            {
                ZLayerType? type = zl.GetLayerType();
                switch (type)
                {
                    case ZLayerType.Core:
                        if (prev_type != null)
                        {
                            if (type == prev_type)
                            {
                                message = String.Format("Core dielectric {0} is adjacent to another core {1} in the stackup.", stackup.GetLayerOfStackupIndex(zl.ID) + 1, stackup.GetLayerOfStackupIndex(zprev.ID) + 1);
                                bErr = true;
                            }
                        }
                        prev_type = type;
                        zprev = zl;
                        break;
                    case ZLayerType.Prepreg:
                        /*
                        if (prev_type != null)
                        {
                            if (type == prev_type)
                            {
                                message = String.Format("Prepreg type dielectric {0} next to another Prepreg {1} in stackup.", stackup.GetLayerOfStackupIndex(zl.ID) + 1, stackup.GetLayerOfStackupIndex(zprev.ID) + 1);
                                bErr = true;
                            }
                        }*/
                        prev_type = type;
                        zprev = zl;
                        break;
                }
                if (bErr) break;
            }
            if (bErr)
            {
                Problem p = new Problem(stackup, null, Problem.Severity.Error, "Order of Dielectrics", message);
                report.Add(p);
            }

        }
    }
}