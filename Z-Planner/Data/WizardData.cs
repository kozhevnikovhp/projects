using System;
using System.Collections.Generic;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Translators;
using ZZero.ZPlanner.ZConfiguration;
using System.Linq;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.Utils;

namespace ZZero.ZPlanner.Wizard
{
    public class MaterialData
    {
        public string ID { get; set; }
        public string company { get; set; }
        public string material { get; set; }
        public string construction { get; set; }
        public string H { get; set; }
        public string resin { get; set; }

        public double dH 
        {
            get
            {
                double ret = Double.MaxValue;
                if (Double.TryParse(H, out ret))
                {
                    return ret;
                }
                else return Double.MaxValue;
            }
        }

        public double minH;
        public double maxH;

        public string type { get; set; }

        public string formatTitle()
        {
            string H_Formatted = H.ToString();
            if (!H_Formatted.Contains("."))
            {
                H_Formatted = H_Formatted + ".0";
            }
            if (resin != "")
            {
                return String.Format("{0} ({1}, {2})", H_Formatted, construction, resin);
                //--return String.Format("[{0}]\t H={1} mils,\t Resin={2}", construction, H, resin);
            }
            else
            {
                return String.Format("{0} ({1})", H_Formatted, construction);
                //--return String.Format("[{0}]\t H={1} mils", construction, H);
            }
        }
        public string title {get; set;}
    }

    public class DMLQuery
    {
        internal static string GetValue(ZMaterial m, string parId)
        {
            try
            {
                return m.MaterialParameters.GetByID(parId).Value;
            }
            catch
            {
                return "";
            }
        }

        internal static List<MaterialData> SelectMaterialByManufacturer(string producer)
        {
            List<MaterialData> mData;
                mData = (from material in ZPlannerManager.Dml.Materials
                         where ((material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value == producer))
                         select new MaterialData
                         {
                             ID = material.ID,
                             company = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value,
                             material = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value,
                             construction = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDNormalizedConstruction).Value,
                             H = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value,//GetValue(material, ZStringConstants.DMLParameterIDH)//
                             resin = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDResin).Value,
                         }).ToList();

            return mData;
        }

        internal static List<MaterialData> SelectMaterialByManufacturer(string producer, bool bCore, string materialName = "")
        {
            string sType = bCore ? "Core" : "Prepreg";
            List<MaterialData> mData;
            if (materialName.Length == 0)
            {
                mData = (from material in ZPlannerManager.Dml.Materials
                        where ((material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value == producer) &&
                        (material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDType).Value == sType))
                        select new MaterialData
                        {
                            ID = material.ID,
                            company = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value,
                            material = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value,
                            construction = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDNormalizedConstruction).Value,
                            H = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value,//GetValue(material, ZStringConstants.DMLParameterIDH)//
                            resin = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDResin).Value,
                        }).ToList();
            }
            else
            {
                mData = (from material in ZPlannerManager.Dml.Materials
                         where ((material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value == producer) &&
                         (material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDType).Value == sType) &&
                         (material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value == materialName))
                         select new MaterialData
                         {
                             ID = material.ID,
                             company = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value,
                             material = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value,
                             construction = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDNormalizedConstruction).Value,
                             H = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value,//GetValue(material, ZStringConstants.DMLParameterIDH)//
                             resin = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDResin).Value,
                         }).ToList();
            }

            return mData;
        }

        internal static List<MaterialData> SelectUniqueMaterialData(string producer)
        {
            List<MaterialData> mData = SelectMaterialByManufacturer(producer);

            SortedDictionary<string, int> materialMap = new SortedDictionary<string, int>(); 

            for (int i = 0; i < mData.Count; i++)
            {
                MaterialData m = mData[i];//new value
                if (!materialMap.ContainsKey(m.material))
                {
                    materialMap.Add(m.material, i);
                }
            }

            List<MaterialData> retval = new List<MaterialData>();
            foreach (KeyValuePair<string, int> kvp in materialMap)
            {
                retval.Add(mData[kvp.Value]);
            }

            return retval;
        }


        internal static List<MaterialData> SelectMaterialByManufacturerAndHeight(string producer, double H)
        {
            List<MaterialData> mData = SelectMaterialByManufacturer(producer);

            SortedDictionary<string, int> materialMap = new SortedDictionary<string, int>(); //material->index with the closest H

            for (int i = 0; i < mData.Count; i++)
            {
                MaterialData m = mData[i];//new value
                if (materialMap.ContainsKey(m.material))
                {
                    int d = materialMap[m.material];
                    MaterialData md = mData[d]; //old value
                    if (Math.Abs(m.dH - H) < Math.Abs(md.dH - H))
                    {
                        materialMap[m.material] = i;
                    }
                }
                else
                {
                    materialMap.Add(m.material, i);
                }
            }

            List<MaterialData> retval = new List<MaterialData>();
            foreach (KeyValuePair<string, int> kvp in materialMap)
            {
                retval.Add(mData[kvp.Value]);
            }

            return retval;
        }

        internal static List<MaterialData> SelectMaterialByManufacturerAndHeight(string producer, bool bCore, double H, string materialName = "")
        {
            List<MaterialData> mData = SelectMaterialByManufacturer(producer, bCore, materialName);

            SortedDictionary<string, int> materialMap = new SortedDictionary<string, int>(); //material->index with the closest H

            for (int i = 0; i < mData.Count; i++)
            {
                MaterialData m = mData[i];//new value
                if (materialMap.ContainsKey(m.material))
                {
                    int d = materialMap[m.material];
                    MaterialData md = mData[d]; //old value
                    if (Math.Abs(m.dH - H) < Math.Abs(md.dH - H))
                    {
                        materialMap[m.material] = i;
                    }
                }
                else
                {
                    materialMap.Add(m.material, i);
                }
            }

            List<MaterialData> retval = new List<MaterialData>();
            foreach (KeyValuePair<string, int> kvp in materialMap)
            {
                retval.Add(mData[kvp.Value]);
            }

            return retval;
        }

        internal static bool GreaterThan(ZMaterial m, string parId, double threshold)
        {
            try
            {
                return Convert.ToDouble(m.MaterialParameters.GetByID(parId).Value) >= threshold;
            }
            catch
            {
                return false;
            }
        }
        internal static bool LessThan(ZMaterial m, string parId, double threshold)
        {
            try
            {
                return Convert.ToDouble(m.MaterialParameters.GetByID(parId).Value) <= threshold;
            }
            catch
            {
                return false;
            }
        }

        internal static List<MaterialData> SelectMaterialsByParameters(double minTg, double maxDk, double minDf, double maxDf)
        {
            List<MaterialData> materials = (from material in ZPlannerManager.Dml.Materials
                                            where
                                            (GreaterThan(material, ZStringConstants.DMLParameterIDTg, minTg) &&
                                             LessThan(material, ZStringConstants.DMLParameterIDDk_1GHz, maxDk) &&
                                             LessThan(material, ZStringConstants.DMLParameterIDDf_1GHz, maxDf) &&
                                             GreaterThan(material, ZStringConstants.DMLParameterIDDf_1GHz, minDf)
                                            )
                                            select new MaterialData { 
                                                ID = material.ID,
                                                company = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value,
                                                material = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value,
                                                H = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value,
                                                construction = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDNormalizedConstruction).Value,
                                                type = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDType).Value,
                                                resin = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDResin).Value
                                            }).ToList();

            SortedDictionary<string, bool> hasCore = new SortedDictionary<string, bool>(); 
            SortedDictionary<string, bool> hasPrepreg = new SortedDictionary<string, bool>(); 
            for (int j = 0; j < materials.Count; j++)
            {
                MaterialData m = materials[j];
                if (m.type == "Core"){
                    hasCore[m.material] = true;
                }
                if (m.type == "Prepreg")
                {
                    hasPrepreg[m.material] = true;
                }
            }

            List<MaterialData> validMaterials = new List<MaterialData>();
            for (int j = 0; j < materials.Count; j++)
            {
                string s = materials[j].material;
                if (hasCore.ContainsKey(s) && hasPrepreg.ContainsKey(s))
                {
                    validMaterials.Add(materials[j]);
                }
            }

            return validMaterials;
        }


        internal static ZMaterial SelectMaterialByParametersAndHeight(bool bCore, double minTg, double maxDk, double maxDf, double height, string materialName = "")
        {
            string sType = bCore ? "Core" : "Prepreg";

            List<ZMaterial> materials;
            if (materialName.Length == 0)
            {
                materials = (from material in ZPlannerManager.Dml.Materials
                            where
                            (material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDType).Value == sType) &&
                            (GreaterThan(material, ZStringConstants.DMLParameterIDTg, minTg) &&
                                LessThan(material, ZStringConstants.DMLParameterIDDk_1GHz, maxDk) &&
                                LessThan(material, ZStringConstants.DMLParameterIDDf_1GHz, maxDf)
                            )
                            select material).ToList();
            }
            else
            {
                materials = (from material in ZPlannerManager.Dml.Materials
                             where
                             (material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDType).Value == sType) &&
                             (material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value == materialName) &&
                             (GreaterThan(material, ZStringConstants.DMLParameterIDTg, minTg) &&
                                 LessThan(material, ZStringConstants.DMLParameterIDDk_1GHz, maxDk) &&
                                 LessThan(material, ZStringConstants.DMLParameterIDDf_1GHz, maxDf)
                             )
                             select material).ToList();
            }

            //select material closest thickness
            double delta = 10000;
            ZMaterial mp = materials.Count > 0 ? materials[0] : null;
            foreach (ZMaterial item in materials)
            {
                if (item.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value == String.Empty) continue;

                double h;// = Convert.ToDouble(item.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value);
                if (Double.TryParse(item.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value, out h))
                {
                    double d = Math.Abs(h - height);
                    if (d < delta)
                    {
                        delta = d;
                        mp = item;
                    }
                }
            }

            return mp;
        }

        // queries for selecting by parameters
        //=========================================================================================================
        internal static List<MaterialData> SelectMaterialDataByManufacturer(List<MaterialData> source, string producer)
        {
            List<MaterialData> mData;
            mData = (from md in source
                     where ((md.company == producer))
                     select new MaterialData
                     {
                         ID = md.ID,
                         company = md.company,
                         material = md.material,
                         construction = md.construction,
                         H = md.H,
                         resin = md.resin
                     }).ToList();
            return mData;
        }

        internal static List<MaterialData> SelectUniqueMaterialData(List<MaterialData> source, string producer)
        {
            List<MaterialData> mData = SelectMaterialDataByManufacturer(source, producer);

            SortedDictionary<string, int> materialMap = new SortedDictionary<string, int>(); //material->index with the closest H

            for (int i = 0; i < mData.Count; i++)
            {
                MaterialData m = mData[i];//new value
                if (!materialMap.ContainsKey(m.material))
                {
                    materialMap.Add(m.material, i);
                }
            }

            List<MaterialData> retval = new List<MaterialData>();
            foreach (KeyValuePair<string, int> kvp in materialMap)
            {
                retval.Add(mData[kvp.Value]);
            }

            return retval;
        }

        internal static List<MaterialData> SelectMaterialDataByManufacturer(List<MaterialData> source, string producer, bool bCore, string materialName = "")
        {
            string sType = bCore ? "Core" : "Prepreg";
            List<MaterialData> mData;
            mData = (from md in source
                        where ((md.company == producer) &&
                        (md.type == sType) &&
                        (md.material == materialName))
                        select new MaterialData
                        {
                            ID = md.ID,
                            company = md.company,
                            material = md.material,
                            construction = md.construction,
                            H = md.H,
                            resin = md.resin
                        }).ToList();

            return mData;
        }

    }
    
    public class WizardData
    {
        public enum State {pageMetals, pageDielectrics, pageGenericDielectric, pageManufacturerDielectric, pageMaterial, pageThickness }
        public State state
        {
            get;
            set;
        }//State.pageMetals; //initial state

// data
//---------------------------------------------------------------------------------------------------        
        // pageMetals
        public int numberOfLayers = 4;
        public int numberSeqLam = 0;
        public double targetBoardWidth = 0; //[mils]
        public List<int> signals;
        public List<int> splitMixed;
        public List<int> planes;

        public double signalTraceWidth = 0;  //[mils]
        public double signalLayerWeight = 0; //[oz]
        public double splitMixedLayerWeight = 0;//[oz] - for both split/mixed and planes
        public double platingWeight = 0;//[oz] - for both split/mixed and planes

        public double outerMetalWidth = 0;
        public double outerMetalDiffWidth = 0;
        public double outerMetalDiffSpacing = 0;
        public double innerMetalWidth = 0;
        public double innerMetalDiffWidth = 0;
        public double innerMetalDiffSpacing = 0;

        // pageDielectrics
        public enum SelectDielectric {Generic, byManufacturer, byParameter}
        public SelectDielectric selectDielectric = SelectDielectric.Generic;
        public bool prepregOnOuterLayer = false;
        public bool solderMask = false;
        public double solserMaskThickness = 0.5; //mils

        // pageGenericDielectric
        public double targetDk = 0;
        public double targetDf = 0;
        public bool specifyDielectricHeight = false; //if false - height is calculated from board thickness
        public double dielectricHeight = 0; //[mils]

        //pageManufacturerDielectric
        public List<string> manufacturerList;
        public List<string> dmlList; //? probably List<Material>, or ID
        public string selManufacturer;
        public string selMaterialId;
        public string selCoreId, selPrepregId;
        public string nPlies = "1";

        //pageMaterial
        public enum GlassTransitionTemp { All, temp200, temp170, temp150, temp110 } //tempXXX means temp >= XXX
        public GlassTransitionTemp glasTransitionTemp = GlassTransitionTemp.All;

        //public enum DiffusionFactor { All, df075, df05, df025, df02, df0175, df015, df0125 } //dfXXX means Df <= 0.XXX
        //public DiffusionFactor diffusionFactor = DiffusionFactor.All;

        // Df range
        public double UpperDf = 0;
        public double LowerDf = 0;

        public enum DielectricConstant { All, dk425, dk400, dk375, dk350, dk325 } //dkXYY means Dk <= X.YY
        public DielectricConstant dielectricConstant = DielectricConstant.All;

        //pageThickness
        public List<MaterialData> groupList;
        public List<string> dmlParList;
        public string selParManufacturer;
        public string selParMaterialId;
        public string selParCoreId, selParPrepregId;

//---------------------------------------------------------------------------------------------------        

        public WizardData()
        {
            //default ctor
            //create lists
            signals =  new List<int>();
            splitMixed = new List<int>();
            planes = new List<int>();
            manufacturerList = new List<string>();
            dmlList = new List<string>(); //? probably List<Material>, or ID
            groupList = new List<MaterialData>();
            dmlParList = new List<string>();
            state = State.pageMetals;
        }

        public void Prev()
        {
            switch (state)
            {
                case State.pageMetals:
                    //shouldn't get here
                    break;
                case State.pageDielectrics:
                    state = State.pageMetals;
                    break;
                case State.pageGenericDielectric:
                    state = State.pageDielectrics;
                    break;
                case State.pageManufacturerDielectric:
                    state = State.pageDielectrics;
                    break;
                case State.pageMaterial:
                    state = State.pageDielectrics;
                    break;
                case State.pageThickness:
                    state = State.pageMaterial;
                    break;
            }
        }

        public void Next()
        {
            switch (state)
            {
                case State.pageMetals:
                    state = State.pageDielectrics;
                    break;
                case State.pageDielectrics:
        //public enum SelectDielectric {Generic, byManufacturer, byParameter}
        //public SelectDielectric selectDielectric = SelectDielectric.Generic;
                    switch (selectDielectric) { 
                        case SelectDielectric.Generic: 
                            state = State.pageGenericDielectric;
                            break;
                        case SelectDielectric.byManufacturer:
                            state = State.pageManufacturerDielectric;
                            break;
                        case SelectDielectric.byParameter:
                            state = State.pageMaterial;
                            break;
                    }
                    break;
                case State.pageGenericDielectric:
                    //shouldn't get here
                    break;
                case State.pageManufacturerDielectric:
                    //shouldn't get here
                    break;
                case State.pageMaterial:
                    state = State.pageThickness;
                    break;
                case State.pageThickness:
                    //shouldn't get here
                    break;
            }

        }

 //----------------------------------------------------------------------------------------------------------       
        private void GetMaterialParameters(out double minTg, out double maxDk, out double minDf, out double maxDf)
        {
            //get parameters value
            minTg = 0; //all
            if (glasTransitionTemp == GlassTransitionTemp.temp200)
            {
                minTg = 200;
            }
            else if (glasTransitionTemp == GlassTransitionTemp.temp170)
            {
                minTg = 170;
            }
            else if (glasTransitionTemp == GlassTransitionTemp.temp150)
            {
                minTg = 150;
            }
            else if (glasTransitionTemp == GlassTransitionTemp.temp110)
            {
                minTg = 110;
            }

            maxDk = 10; //all
            if (dielectricConstant == DielectricConstant.dk425)
            {
                maxDk = 4.25;
            }
            else if (dielectricConstant == DielectricConstant.dk400)
            {
                maxDk = 4;
            }
            else if (dielectricConstant == DielectricConstant.dk375)
            {
                maxDk = 3.75;
            }
            else if (dielectricConstant == DielectricConstant.dk350)
            {
                maxDk = 3.5;
            }
            else if (dielectricConstant == DielectricConstant.dk325)
            {
                maxDk = 3.25;
            }

            minDf = Math.Min(LowerDf, UpperDf);
            maxDf = Math.Max(LowerDf, UpperDf);
        }


        //estimate board thickness for generic stackup parameters
        public void EstimateBoardThickness()
        {
            double thick = 0;

            double plating_height = ZPlannerManager.GetThicknesByCopperWeight(platingWeight);
            double sig_height = ZPlannerManager.GetThicknesByCopperWeight(signalLayerWeight);
            double split_height = ZPlannerManager.GetThicknesByCopperWeight(splitMixedLayerWeight);
            double diel_height = Options.TheOptions.height;
            double soldermask_height = Options.TheOptions.soldermask_height;

            // top layers:
            ZLayerType bottomType = ZLayerType.SolderMask;
            if (solderMask)
            {
                thick += soldermask_height;
                bottomType = ZLayerType.SolderMask;
            }
            else
            {
                if (prepregOnOuterLayer)
                {
                    thick += diel_height;
                    bottomType = ZLayerType.Prepreg;
                }
                else
                {
                    thick += diel_height;
                    bottomType = ZLayerType.Core;
                }
            }

            int metalLayer = 1;
            //type for the next dielectric layer
            ZLayerType nextType = ZLayerType.Core;
            if (solderMask)
            {
                nextType = prepregOnOuterLayer ? ZLayerType.Prepreg : ZLayerType.Core;
            }
            else
            {
                nextType = prepregOnOuterLayer ? ZLayerType.Core : ZLayerType.Prepreg;
            }

            while (metalLayer <= numberOfLayers)
            {
                if (signals.Contains(metalLayer))
                {
                    thick += sig_height;
                }
                else if (planes.Contains(metalLayer))
                {
                    thick += split_height;
                }
                else
                {
                    thick += split_height;
                }

                if (metalLayer != numberOfLayers)
                {
                    thick += diel_height;
                    nextType = nextType == ZLayerType.Core ? ZLayerType.Prepreg : ZLayerType.Core;
                }

                metalLayer++;
            }

            //bottom lyaer
            switch (bottomType)
            {
                case ZLayerType.SolderMask: 
                    thick += soldermask_height;
                    break;
                case ZLayerType.Core:
                case ZLayerType.Prepreg:
                    thick += diel_height;
                    break;
            }

            targetBoardWidth = thick + 2*plating_height;
        }

        //Create Stackup
        private void AddPrepreg(ZStackup stk, bool bSeqLam = false, int nPly = 1)
        {
            for (int i = 0; i < nPly; i++)
            {
                stk.AddLayer(ZLayerType.Prepreg);
                int iLast = stk.Layers.Count;
                ZLayer lastLayer = stk.Layers[iLast - 1];
                ZPlannerProject.SetLayerParameterValue(lastLayer, ZStringConstants.ParameterIDSequentialLamination, bSeqLam);                    
            }
        }

        
        class cDielectricPlan
        {
            public class cEntry
            {
                public  ZLayerType type;
                public int nPlies;
                public bool bX; //seq lam prepreg

                public cEntry(ZLayerType atype, bool seqLam  =  false, int ply = 1)
                {
                    type = atype;
                    nPlies = ply;
                    bX = seqLam;
                }
            }

            List <cEntry> plan;

            int nMetals, nDielectric;
            int nX; //buildup structure nX-N-nX: N+2*nX = nMetals; 
            int nPly;
            bool bSolderMask; //on outer layers
            bool bPregOuter; //if buildup - it is Yes ???

            public cDielectricPlan(int metals, int X, int ply, bool bSM, bool bPreg) //X = seq lamination
            {
                nMetals = metals;
                nX = X;
                nPly = ply;
                bSolderMask = bSM;
                bPregOuter = bPreg;

                nDielectric = nMetals; //max dielectric number
                plan = new List<cEntry>();
                for (int i = 0; i <= nDielectric; i++) plan.Add(null);

                BuildPlan();
            }

            private void BuildPlan()
            {
                //buildup layers
                bool bBuildUp = nX > 0;
                if (bBuildUp)
                {
                    for (int i = 0; i < nX; i++)
                    {
                        plan[i] = new cEntry(ZLayerType.Prepreg, true, nPly);
                        plan[nDielectric - i] = new cEntry(ZLayerType.Prepreg, true, nPly);
                    }
                }

                //outer layers - override with solder mask
                if (bSolderMask)
                {
                    plan[0] = new cEntry(ZLayerType.SolderMask);
                    plan[nDielectric] = new cEntry(ZLayerType.SolderMask);
                }

                //core
                ZLayerType ltype = bPregOuter ? ZLayerType.Prepreg : ZLayerType.Core;
                for (int i = 0; i <= nDielectric; i++)
                {
                    if (plan[i] != null) continue;
                    plan[i] = new cEntry(ltype, false, ltype == ZLayerType.Prepreg ? nPly : 1);
                    ltype = ltype == ZLayerType.Prepreg ? ZLayerType.Core : ZLayerType.Prepreg;
                }

            }

            public cEntry GetDielectric(int metalIdx)
            {
                return plan[metalIdx];
            }
        }

        private ZStackup CreateStackupDraft()
        {
            // create stackup with the correct layers sequence   
            int nPly = Int16.Parse(nPlies);

            ZPlannerProject project = new ZPlannerProject();
            ZStackup stackup = new ZStackup(project);
            stackup.ActiveSingle = stackup.Singles[0];
            stackup.ActivePair = stackup.Pairs[0];

            project.Parameters.Clear();
            project.SubParameters.Clear();

            foreach (SelectList defaultSelectList in project.GetDefaultSelectLists())
            {
                project.Lists.Add(defaultSelectList);
            }

            foreach (ZParameter parameter in project.GetDefaultParameters())
            {
                project.Parameters.Add(parameter);
                if (parameter.SubParameters != null && parameter.SubParameters.Count > 0) project.SubParameters.AddRange(parameter.SubParameters);
            }
            
            //plan dielectrics
            cDielectricPlan plan = new cDielectricPlan(numberOfLayers, numberSeqLam, nPly, solderMask, prepregOnOuterLayer);

            int metalLayer = 1;

            cDielectricPlan.cEntry entry = plan.GetDielectric(0);
            if (entry.type == ZLayerType.Prepreg)
            {
                AddPrepreg(stackup, entry.bX, entry.nPlies);
            }
            else
            {
                stackup.AddLayer(entry.type);
            }

            while (metalLayer <= numberOfLayers)
            {
                if (signals.Contains(metalLayer))
                {
                    stackup.AddLayer(ZLayerType.Signal);
                }
                else if (planes.Contains(metalLayer))
                {
                    stackup.AddLayer(ZLayerType.Plane);
                }
                else
                {
                    stackup.AddLayer(ZLayerType.SplitMixed);
                }

                entry = plan.GetDielectric(metalLayer);
                if (entry.type == ZLayerType.Prepreg)
                {
                    AddPrepreg(stackup, entry.bX, entry.nPlies);
                }
                else
                {
                    stackup.AddLayer(entry.type);
                }

                metalLayer++;
            }

/*
            // top layers:
            ZLayerType bottomType = ZLayerType.SolderMask;
            if (solderMask)
            {
                stackup.AddLayer(ZLayerType.SolderMask);
                bottomType = ZLayerType.SolderMask;
            }
            else 
            {
                if (prepregOnOuterLayer)
                {
                    AddPrepreg(stackup, nPly);//--stackup.AddLayer(ZLayerType.Prepreg);
                    bottomType = ZLayerType.Prepreg;
                }
                else
                {
                    stackup.AddLayer(ZLayerType.Core);
                    bottomType = ZLayerType.Core;
                }
            }

            int metalLayer = 1;
            //type for the next dielectric layer
            ZLayerType nextType = ZLayerType.Core;
            if (solderMask)
            {
                nextType = prepregOnOuterLayer ? ZLayerType.Prepreg : ZLayerType.Core;
            }
            else
            {
                nextType = prepregOnOuterLayer ? ZLayerType.Core : ZLayerType.Prepreg;
            }

            while (metalLayer <= numberOfLayers)
            {
                if (signals.Contains(metalLayer))
                {
                    stackup.AddLayer(ZLayerType.Signal);
                }
                else if (planes.Contains(metalLayer))
                {
                    stackup.AddLayer(ZLayerType.Plane);
                }
                else
                {
                    stackup.AddLayer(ZLayerType.SplitMixed);
                }

                if (metalLayer != numberOfLayers)
                {
                    if (nextType == ZLayerType.Core)
                    {
                        stackup.AddLayer(nextType);
                    }
                    else
                    {
                        AddPrepreg(stackup, nPly);
                    }

                    nextType = nextType == ZLayerType.Core ? ZLayerType.Prepreg : ZLayerType.Core;
                }

                metalLayer++;
            }
            //bottom layer
            switch (bottomType) { 
                case ZLayerType.SolderMask:
                case ZLayerType.Core:
                    stackup.AddLayer(bottomType);
                    break;
                case ZLayerType.Prepreg:
                    AddPrepreg(stackup, nPly);
                    break;
            }
*/

            project.Stackup = stackup;
            ZPlannerManager.Project = project;

            NameLayers(stackup);

            return stackup;
        }

        void NameLayers(ZStackup stk)
        {
            string name;
            int metalNum = 0;
            foreach (ZLayer zl in stk.Layers)
            {
                if (zl.isMetal())
                {
                    string prefix = "Signal";
                    switch (zl.GetLayerType())
                    {
                        case ZLayerType.Signal :
                            prefix = "Signal";
                            break;
                        case ZLayerType.Plane:
                            prefix = "Plane";
                            break;
                        case ZLayerType.SplitMixed:
                            prefix = "Signal/Plane";
                            break;
                    }
                    metalNum++;
                    name = String.Format("{0}-{1}",prefix, metalNum);
                    ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDLayerName, name);                    
                }
            }
        }

        void AssignMaterialToLayer(ZLayer layer, ZMaterial material)
        {
            ZStackup stk = layer.Stackup;
            List<ZLayer> layers = new List<ZLayer>();
            layers.Add(layer);
            stk.AssignMaterialToStackup(material, layers);
        }

        void AssignMaterials(ZStackup stackup, ZMaterial mCore, ZMaterial mPrepreg)
        {
            double sig_height = ZPlannerManager.GetThicknesByCopperWeight(signalLayerWeight);
            double split_height = ZPlannerManager.GetThicknesByCopperWeight(splitMixedLayerWeight);
            double plating_height = ZPlannerManager.GetThicknesByCopperWeight(platingWeight);
            int metalLayer = 0;

            List<ZLayer> pregs = new List<ZLayer>();
            List<ZLayer> cores = new List<ZLayer>();

            foreach (ZLayer layer in stackup.Layers)
            {
                bool bOuterMetal = false;
                if (layer.isMetal())
                {
                    metalLayer++;
                    bOuterMetal = (metalLayer == 1) || (metalLayer == numberOfLayers);
                }
                switch (layer.GetLayerType())
                {
                    case ZLayerType.Core:
                        //--AssignMaterialToLayer(layer, mCore);
                        cores.Add(layer);
                        break;
                    case ZLayerType.Prepreg:
                        //--AssignMaterialToLayer(layer, mPrepreg);
                        pregs.Add(layer);
                        break;
                    case ZLayerType.Signal:
                        ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDThickness, bOuterMetal ? sig_height + plating_height : sig_height);
                        ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDCopperThickness, bOuterMetal ? signalLayerWeight + platingWeight : signalLayerWeight);
                        break;
                    case ZLayerType.SplitMixed:
                    case ZLayerType.Plane:
                        ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDThickness, bOuterMetal ? split_height + plating_height : split_height);
                        ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDCopperThickness, bOuterMetal ? splitMixedLayerWeight + platingWeight : splitMixedLayerWeight);
                        break;
                }
            }

            stackup.AssignMaterialToStackup(mCore, cores);
            stackup.AssignMaterialToStackup(mPrepreg, pregs);
        }

        double CalculateLayerHeight(ZStackup stackup)
        {
            double plating_height = ZPlannerManager.GetThicknesByCopperWeight(platingWeight);
            double sig_height = ZPlannerManager.GetThicknesByCopperWeight(signalLayerWeight);
            double split_height = ZPlannerManager.GetThicknesByCopperWeight(splitMixedLayerWeight);

            double diel_height = 0;

            bool bFromBoardThickness = false;
            switch (selectDielectric) { 
                case SelectDielectric.Generic:
                    if (specifyDielectricHeight){
                        diel_height = dielectricHeight;
                    }
                    else bFromBoardThickness = true;
                    break;                    
            }
            
            if (bFromBoardThickness) //calculate from board thickness
            {
                int nDiel = 0;
                diel_height = targetBoardWidth;
                int metalLayer = 0;
                foreach (ZLayer layer in stackup.Layers)
                {
                    bool bOuterMetal = false;
                    if (layer.isMetal())
                    {
                        metalLayer++;
                        bOuterMetal = (metalLayer == 1) || (metalLayer == numberOfLayers);
                    }
                    switch (layer.GetLayerType())
                    {
                        case ZLayerType.SolderMask:
                            diel_height -= solserMaskThickness;
                                break;
                        case ZLayerType.Signal:
                            diel_height -= sig_height;
                            if (bOuterMetal) diel_height -= plating_height;
                            break;
                        case ZLayerType.SplitMixed:
                        case ZLayerType.Plane:
                            diel_height -= split_height;
                            if (bOuterMetal) diel_height -= plating_height;
                            break;
                        case ZLayerType.Core:
                        case ZLayerType.Prepreg:
                            nDiel ++;
                            break;
                    }
                }
                diel_height /= nDiel; 
            }

            return diel_height;
        }

        void AssignGenericParameters(ZStackup stackup)
        {
            double sig_height = ZPlannerManager.GetThicknesByCopperWeight(signalLayerWeight);
            double split_height = ZPlannerManager.GetThicknesByCopperWeight(splitMixedLayerWeight);
            double plating_height = ZPlannerManager.GetThicknesByCopperWeight(platingWeight);

            double diel_height = CalculateLayerHeight(stackup);
            int metalLayer = 0;
            foreach (ZLayer layer in stackup.Layers)
            {
                bool bOuterMetal = false;
                if (layer.isMetal())
                {
                    metalLayer++;
                    bOuterMetal = (metalLayer == 1) || (metalLayer == numberOfLayers);
                }
                switch (layer.GetLayerType())
                {
                    case ZLayerType.Core:
                    case ZLayerType.Prepreg:
                        ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDThickness, diel_height);
                        foreach (var single in stackup.Singles)
                        {
                            ZPlannerProject.SetLayerParameterValue(stackup.GetLayerOfSingleImpedance(layer.ID, single.ID), ZStringConstants.ParameterIDZo_DielectricConstant, targetDk);
                            ZPlannerProject.SetLayerParameterValue(stackup.GetLayerOfSingleImpedance(layer.ID, single.ID), ZStringConstants.ParameterIDZo_LossTangent, targetDf);
                        }
                        // TBD implement for all pairs
                        foreach (var pair in stackup.Pairs)
                        {
                            ZPlannerProject.SetLayerParameterValue(stackup.GetLayerOfPairImpedance(layer.ID, pair.ID), ZStringConstants.ParameterIDZdiff_DielectricConstant, targetDk);
                            ZPlannerProject.SetLayerParameterValue(stackup.GetLayerOfPairImpedance(layer.ID, pair.ID), ZStringConstants.ParameterIDZdiff_LossTangent, targetDf);
                        }
                        break;
                    case ZLayerType.Signal:
                        ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDThickness, bOuterMetal ? sig_height + plating_height : sig_height);
                        ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDCopperThickness, bOuterMetal ? signalLayerWeight + platingWeight : signalLayerWeight);
                        break;
                    case ZLayerType.SplitMixed:
                    case ZLayerType.Plane:
                        ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDThickness, bOuterMetal ? split_height + plating_height : split_height);
                        ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDCopperThickness, bOuterMetal ? splitMixedLayerWeight + platingWeight : splitMixedLayerWeight);
                        break;
                }
            }
        }

        public bool CreateStackup()
        {
            ZPlannerManager.SuspendUpdateActiveStackupEvent();

            try
            {
                //create stckup with the correct number and sequernce of layers
                ZStackup stk = CreateStackupDraft();

                switch (selectDielectric)
                {
                    case SelectDielectric.Generic:
                        //doesn't involve specific materials
                        AssignGenericParameters(stk);
                        break;
                    case SelectDielectric.byManufacturer:
                        //
                        ZMaterial mCore = ZPlannerManager.Dml.Materials.GetByID(selCoreId);
                        ZMaterial mPrepreg = ZPlannerManager.Dml.Materials.GetByID(selPrepregId);
                        AssignMaterials(stk, mCore, mPrepreg);

                        break;
                    case SelectDielectric.byParameter:
                        ZMaterial mParCore = ZPlannerManager.Dml.Materials.GetByID(selParCoreId);
                        ZMaterial mParPrepreg = ZPlannerManager.Dml.Materials.GetByID(selParPrepregId);
                        AssignMaterials(stk, mParCore, mParPrepreg);

                        break;
                }

                bool ret = AssignMetalLayerParameters(stk);

                return ret;
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent();
            }
        }

        private bool AssignMetalLayerParameters(ZStackup stk)
        {
            bool commentsShouldBeDisplayed = false;
            int metalLayer = 1;
            foreach (ZLayer zl in stk.Layers)
            {
                ZLayerType? type = zl.GetLayerType();
                switch (type) { 
                //if (zl.isMetal()){
                    case ZLayerType.Signal:
                    case ZLayerType.SplitMixed:
                        {
                            bool bOuter = (metalLayer == 1) || (metalLayer == numberOfLayers);
                            if (bOuter && platingWeight > 0)
                            {
                                ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDComments, "Includes plating");
                                commentsShouldBeDisplayed = true;
                            }

                            double width = bOuter ? outerMetalWidth : innerMetalWidth;
                            double diff_width = bOuter ? outerMetalDiffWidth : innerMetalDiffWidth;
                            double diff_spacing = bOuter ? outerMetalDiffSpacing : innerMetalDiffSpacing;
                            double diff_pitch = diff_spacing + diff_width;
                            foreach (var single in stk.Singles)
                            {
                                ZLayer zSingleLayer = stk.GetLayerOfSingleImpedance(zl.ID, single.ID);
                                ZPlannerProject.SetLayerParameterValue(zSingleLayer, ZStringConstants.ParameterIDZo_TraceWidth, width);
                            }
                            foreach (var pair in stk.Pairs)
                            {
                                ZLayer zPairLayer = stk.GetLayerOfPairImpedance(zl.ID, pair.ID);
                                ZPlannerProject.SetLayerParameterValue(zPairLayer, ZStringConstants.ParameterIDZdiff_TraceWidth, diff_width);
                                ZPlannerProject.SetLayerParameterValue(zPairLayer, ZStringConstants.ParameterIDZdiff_TraceSpacing, diff_spacing);
                                ZPlannerProject.SetLayerParameterValue(zPairLayer, ZStringConstants.ParameterIDZdiff_TracePitch, diff_pitch);
                            }

                            metalLayer++;
                        }
                        break;
                    case ZLayerType.Plane:
                        metalLayer++;
                        break;
                }
            }

            return commentsShouldBeDisplayed;
        }
    }
}