using System;
using System.IO;
using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Spreadsheet;
using System.Text;
//
using ClosedXML.Excel;
using Microsoft.Office.Interop.Excel;
//
using ZZero.ZPlanner;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;
using System.Collections.Generic;
using ZZero.ZPlanner.UI;
using System.Windows.Forms;
using ZZero.ZPlanner.Settings;

using System.Linq;
using ZZero.ZPlanner.UI.Dialogs;
using System.Text.RegularExpressions;
using System.Collections.Specialized;

namespace ZZero.ZPlanner.Translators
{
    class DMLMatch
    {
        private ZLibraryCategory[] libraryPriorityArray;
        private bool bCore;
        private bool bCheckDk;

        public DMLMatch(ZLibraryCategory[] libs, bool core = false)
        {
            libraryPriorityArray = libs;
            bCore = core;
            bCheckDk = true;
        }

        public class MaterialData
        {
            public string ID { get; set; }
            public string H { get; set; }
            public ZLibraryCategory Category { get; set; }
            public string Dk { get; set; }

            public bool ContainsDk(double dk, double tol = 0.001)
            {
                Dk = Dk.Trim();
                //single value or sorted by argument table -> F(a)  a:F;a:F;a:F;..
                double val = 0;
                string[] x = Dk.Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries); //by f points

                if (x.Length == 0) return false;

                if (x.Length == 1)
                {
                    //check single
                    return (Double.TryParse(Dk, out val) && Math.Abs(val - dk) <= tol);
                }
                else //dk(f)
                {
                    //get range
                    double yfirst, ylast;
                    string[] y1 = x[0].Split(':');
                    string[] y2 = x[x.Length - 1].Split(':');

                    if (((y1.Length == 2) && Double.TryParse(y1[1], out yfirst)) && ((y2.Length == 2) && Double.TryParse(y2[1], out ylast)))
                    {
                        double ymin = yfirst, ymax = ylast;
                        if (yfirst > ylast)
                        {
                            ymin = ylast; ymax = yfirst;
                        }
                        return dk >= ymin - tol && dk <= ymax + tol;
                    }
                }

                return false;
            }
        }

        internal static bool CompareResin(ZMaterial m, double value, double threshold)
        {
            string s = m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDResin).Value.TrimEnd('%');
            double a;
            if (Double.TryParse(s, out a)){
                return Math.Abs(a - value) <= threshold;
            }
            return false;
        }

        internal static bool CompareMaterial(ZMaterial m, string mName)
        {
            string materialName = m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;
            materialName = Regex.Replace(materialName, @"[- ]", String.Empty);
            materialName = materialName.ToUpper();
            mName = Regex.Replace(mName, @"[- ]", String.Empty);
            mName = mName.ToUpper();

            return mName == materialName;
        }

        internal static bool CompareThickness(ZMaterial m, double value, double threshold, bool bCore)
        {
            string s = m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value.TrimEnd();
            double a;
            if (Double.TryParse(s, out a))
            {
                if (bCore)
                {
                    return Math.Abs(a - value) <= threshold;
                }
                else //just check that unpressed thickness is reasonably greater than pressed
                {
                    return ((a > value) && (a < 1.5*value));
                }
            }
            return false;
        }

        internal static ZLibraryCategory GetLibraryCategory(string sCat)
        {
            ZLibraryCategory z = ZLibraryCategory.ZZero;
            if (ZMaterial.LibraryCategoryDictionary.ContainsKey(sCat))
            {
                z = ZMaterial.LibraryCategoryDictionary[sCat];
            }
            return z;
        }

        public List<MaterialData> Query(string typeString, string materialName, string construction, double resin, double th)
        {
            List<MaterialData> mData;
            mData = (from material in ZPlannerManager.Dml.Materials
                     where (
                         (material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDType).Value == typeString) &&
                         CompareMaterial(material, materialName) &&
                         (material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDNormalizedConstruction).Value == construction) &&
                         (libraryPriorityArray.Contains(GetLibraryCategory(material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDCategory).Value))) &&
                         CompareResin(material, resin, 0.01) &&
                         CompareThickness(material, th, 0.001, bCore)
                         )
                     select new MaterialData
                     {
                         ID = material.ID,
                         H = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value,
                         Category = GetLibraryCategory(material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDCategory).Value),
                         Dk = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDk).Value
                     }).ToList();

            return mData;
        }

        public static bool MatchDkDf(ZLayer zl)
        {
            //if material is not matched, but resin content is present then try to find Dk/Df
            bool retval = false;

            double resin = 0;
            string materialName = "";
            string construction = "";

            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDConstruction, ref construction);
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDResinContent, ref resin);
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDMaterial, ref materialName);

            List<MaterialData> mData;
            mData = (from material in ZPlannerManager.Dml.Materials
                     where (
                         CompareMaterial(material, materialName) &&
                         CompareResin(material, resin, 0.01) 
                         )
                     select new MaterialData
                     {
                         ID = material.ID,
                     }).ToList();

            if (mData.Count > 0)
            {
                ZMaterial zm = ZPlannerManager.Dml.Materials.GetByID(mData[0].ID);
                string Dk = zm.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDk).Value;
                string Df = zm.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDf).Value;

                zl.SetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant, Dk, true);
                zl.SetLayerParameterValue(ZStringConstants.ParameterIDLossTangent, Df, true);
                retval = true;
            }
            return retval;
        }

        public bool MatchMaterial(ZLayer zl, out ZMaterial mat)
        {
            string typeString = "Prepreg";
            bool bCheck = false;
            switch (zl.GetLayerType())
            {
                case ZLayerType.Prepreg :
                    bCheck = true;
                    break;
                case ZLayerType.Core:
                    typeString = "Core";
                    bCheck = true;
                    break;
            }
            mat = null;
            if (bCheck)
            {
                string construction = "";
                double resin = 0;
                string materialName = "";
                double th = 0;

                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDConstruction, ref construction);
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDResinContent, ref resin);
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDMaterial, ref materialName);
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref th);

                List<MaterialData> mData = Query(typeString, materialName, construction, resin, th);
                //if (mData.Count == 0)
                //{
                //    string[] cons = construction.Trim().Split('x');
                //    int multiplier = 0;
                //    if (cons.Length == 2 && Int32.TryParse(cons[0], out multiplier))
                //    {
                //        mData = Query(typeString, materialName, cons[1], resin, th / multiplier);
                //    }
                //}

                //additional check for Dk: find the first match
                int iMatch = -1;
                double layerDk = 0;

                bool bValidDk = false;
                ZLayerParameter lp = zl.GetLayerParameter(ZStringConstants.ParameterIDDielectricConstant);
                if (lp != null)
                {
                    if (lp.IsEdited)
                    {
                        ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDDielectricConstant, ref layerDk);
                        bValidDk = true;
                    }
                }
                if (!bValidDk) bCheckDk = false;

                if (bCheckDk)
                {
                    MaterialData md;
                    for (int i = 0; i < mData.Count; i++)
                    {
                        md = mData[i];
                        if (md.ContainsDk(layerDk))
                        {
                            iMatch = i;
                            break;
                        }
                    }
                }
                else
                {
                    iMatch = 0;
                }

                if (mData.Count > 0 && iMatch >= 0 && Double.TryParse(mData[iMatch].H, out th))
                {
                    mData.Sort(
                        delegate(MaterialData m1, MaterialData m2)
                        {
                            int retval = 0;
                            int p1 = Array.IndexOf(libraryPriorityArray, m1.Category);
                            int p2 = Array.IndexOf(libraryPriorityArray, m2.Category);
                            if (p1 > p2) retval = 1;
                            if (p1 < p2) retval = -1;

                            return retval;
                        }
                    );
                    mat = ZPlannerManager.Dml.Materials.Find(x => x.ID == mData[iMatch].ID);
                    return true;
                }
            }

            return false;
        }
    }
}