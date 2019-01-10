using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Windows.Forms;
#if ZSANDBOX==false
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
#endif
using ZZero.ZPlanner.ZConfiguration;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.UI.Dialogs;
using ZZero.ZPlanner.Utils;

namespace ZZero.ZPlanner.FXInterface
{
    // FX Data
    public enum LAYER_TYPE { FS_ROUTING = 0, FS_PLANE = 1, FS_DIEL = 2 }
    public enum FS_ERROR_CODE {  
       FS_ERROR=-1,
        // error codes under normal conditions
       FS_ERR_OK,					// no problems detected
       FS_ERR_MEMORY,              // failure to allocate memory
       FS_ERR_ONEWIRE,             // only one conductor found (need at least two)
       // internal errors (indicate that bugs are present)
       FS_ERR_SINGULAR,            // singular matrix
       FS_ERR_SQRT,                // not a positively-defined matrix
       FS_ERR_EIGEN,               // failure in eigenvalues problem
       // wrong calls
       FS_ERR_NOTREADY,				// solver not started yet
       // wrong calls of graphic routine
       FS_ERR_NOGRAPHICDATA,		// no graphic data available
       FS_ERR_WRONGMODE,				// invalid mode number
       // errors detected on input
       FS_ERR_INPUT_LAYERCOUNT,	// no layers defined
       FS_ERR_INPUT_TRACECOUNT,	// no traces defined
       FS_ERR_INPUT_LAYERS,			// wrong layer data
       FS_ERR_INPUT_TRACES,			// wrong trace data
       FS_ERR_INPUT_INTERSECT,		// traces are too close or intersect
       FS_ERR_INPUT_TRACEDIEL,		// trace on dielectric layer
       FS_ERR_INPUT_POTENTIALS,	// invalid potential index
       FS_ERR_INPUT_GROUND,			// no ground planes/traces specified
       // user break detected
       FS_ERR_USER_BREAK,

       // cache specific codes
       FS_ERR_CACHE_CANNOT_BE_CHANGED,	// cache parameters cannot be changed
       FS_ERR_CACHE_CANNOT_LOAD,		// cache cannot be loaded
       FS_ERR_CACHE_CANNOT_SAVE,		// cache cannot be saved

       // self test return codes
       FS_ERR_TEST_FAILED,				// errors found
       FS_ERR_TEST_INVALIDTASK,		// invalid task file
       FS_ERR_TEST_INVALIDTEST,		// invalid test file
       FS_ERR_TEST_INVALIDPATH,		// cannot open file

       // If returned, this means that given function cannot be executed at
       // this time (this will cause re-entry and malfunction). I recommend 
       // running FS_AbortCalcs() and trying again, until FS_OK is received.
       FS_ERR_BUSY,

       // The licensed version of the field-solver could not detect the presence
       // of a license.
       FS_ERR_NOLICENSE
    }

#if ZSANDBOX==false
    class FX_Layer
    {
        public FX_Layer(ZLayerType? zType)//init from defaults
        {
            LAYER_TYPE aType = FX_Input.ConvertLayerType(zType);
            iLayerType = aType;
            bIsMixed = zType == ZLayerType.SplitMixed;


            //init from defaults
            switch (zType)
            {
                case ZLayerType.Core:
                    fThickness = Units.fMilsToMeters * 5f;
                    fDielConstant = 4f;
                    break;
                case ZLayerType.Prepreg:
                    fThickness = Units.fMilsToMeters * 5f;
                    fDielConstant = 4f;
                    break;
                case ZLayerType.SolderMask:
                    fThickness = Units.fMilsToMeters * 0.5f;
                    fDielConstant = 3.3f;
                    break;

                case ZLayerType.Signal:
                case ZLayerType.Plane:
                case ZLayerType.SplitMixed:
#if ZSANDBOX
                    fThickness = Units.fMilsToMeters * (float)ZZero.ZSandbox.ZSandboxManager.GetThicknesByCopperWeight(0.5);
#else
                    fThickness = Units.fMilsToMeters * (float)ZPlannerManager.GetThicknesByCopperWeight(0.5);
#endif
                    fTopRoughness = 3.5e-6f;       
                    fBottomRoughness = 3.5e-6f;    
                    break;
            }                                  
        }

        public FX_Layer(ZLayer zl, bool bOuterMetal)
        {
            ZLayer zlSingle = zl.Stackup.ActiveSingle.GetLayerOfSingleImpedance(zl.ID);
            ZLayerType? zType = zl.GetLayerType();
            LAYER_TYPE aType = FX_Input.ConvertLayerType(zType);
            iLayerType = aType;
            bIsMixed = zType == ZLayerType.SplitMixed;

            //account for plane reference setting
/*
            string planeRef = "";
            if ((zType == ZLayerType.SplitMixed) && ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDPlaneReference, ref planeRef))
            {
                if (planeRef == ZStringConstants.PlaneReference)
                {
                    iLayerType = LAYER_TYPE.FS_PLANE;
                }
            }
*/
            
            float fval = 0f;
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref fval);
            fThickness = Units.fMilsToMeters * fval;
            //redefine thickness of prepregs 
            if (ZPlannerManager.StackupPanel != null && ZPlannerManager.IsPressedThickness && zType == ZLayerType.Prepreg)
            {
                fval = 0f;
                bool b = ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDPrepregThickness, ref fval);
                if (b && fval.ToString() != String.Empty)
                {
                    fThickness = Units.fMilsToMeters * fval;
                }
            }

            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThermalConductivity, ref fval);
            fConductivity = fval;
            
            switch (zType)
            {
                case ZLayerType.Core:
                case ZLayerType.Prepreg:
                case ZLayerType.SolderMask:
                    {
                        bool bval = false;
                        ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDConformal, ref bval);
                        bIsConformal = bval;

                        if (bIsConformal)
                        {
                            fThicknessAboveTrace = fThickness;
                            fThicknessBetweenTraces = fThickness;
                        }

                        //--ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_DielectricConstant, ref fval);
                        double d;
                        zlSingle.GetDk(out d);
                        fDielConstant = (float)d;

                        //--ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_LossTangent, ref fval);
                        zlSingle.GetDf(out d);
                        fLossTangent = (float)d;
                    }
                    break;

                case ZLayerType.Signal:
                case ZLayerType.Plane:
                case ZLayerType.SplitMixed:
                    if (bOuterMetal) //metal layer facing air
                    {
                        fDielConstant = 1;
                        fLossTangent = 0;
                    }
                    else
                    {
                        fDielConstant = (float)Options.TheOptions.resin_Dk;
                        fLossTangent = (float)Options.TheOptions.resin_Df;
                    }

                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDBulkRes, ref fval);
                    fRmaterial = fval;

                    fTopRoughness = 3.5e-6f;       
                    fBottomRoughness = 3.5e-6f;

                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDRoughTop, ref fval);
                    if (fval == 0)
                    {
                        ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDCalcRoughTop, ref fval);
                    }
                    fTopRoughness = fval * 1.0e-6f * (float)Units.RzToRMS; //um to meters

                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDRoughBottom, ref fval);
                    if (fval == 0)
                    {
                        ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDCalcRoughBottom, ref fval);
                    }
                    fBottomRoughness = fval * 1.0e-6f * (float)Units.RzToRMS; //um to meters

                    break;
            }
        }

        public float fThickness = Units.fMilsToMeters * 5;		// Thickness of layer
        public float fDielConstant = 4;	// Dielectric's permittivity
        public LAYER_TYPE iLayerType = LAYER_TYPE.FS_DIEL;     // Layer's type: routing, plane, or dielectric
        public bool bIsMixed = false;
        public bool bIsConformal = false;		// Is this layer a conformal coating? Valid only
        //		if layer is dielectric and the first or last
        //		in the array
        // lossy lines support
        public float fRmaterial = 1.724e-008f;		// metal resistivity in ohms*m; if variable, should
        // be provided at necessary temperature
        public float fLossTangent = 0.02f;	// loss tangent of dielectric; this is usually small 
        // (0.02 for FR4 or less)
        public float fConductivity = 1.0e-5f; // static conductivity of dielectric in S/m; inverse 
        // to resistivity; nonzero values nake sense only for
        // semiconductors (typical values 1e-5 to 1e-4)
        public float fTopRoughness = 3.5e-6f;       // used for metal layers only; roughness in meters
        public float fBottomRoughness = 3.5e-6f;    // default value is 0 - for smooth surfaces
        // variable conformal layer thickness support
        public float fThicknessAboveTrace = 0;
        public float fThicknessBetweenTraces = 0;
    }

    class FX_Trace
    {
        public const float Trace2Plane = Units.fMilsToMeters * 10;

        public FX_Trace(ZStackup stackup, ZLayer zDpLayer, bool bPair)
        {
            ZLayer zl = stackup.GetLayerOfStackup(zDpLayer.ID);

            iLayerIndex = stackup.GetLayerOfStackupIndex(zl.ID) + 1;//1-based            

            float mid_width = 0;
            float etch = 0;
            float spacing = 0;

            //get etch factor from single impedance data
            double dval = 0;
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDEtchFactor, ref dval);
            etch = (float)dval;

            dval = 0;
            ZPlannerProject.GetLayerParameterValue(zDpLayer, ZStringConstants.ParameterIDZdiff_TraceWidth, ref dval);
            dval *= Units.MilsToMeters;
            mid_width = (float)dval;

            ZPlannerProject.GetLayerParameterValue(zDpLayer, ZStringConstants.ParameterIDZdiff_TraceSpacing, ref dval);
            dval *= Units.MilsToMeters;
            spacing = (float)dval;

            bool bNarrowTop = true;
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDNarrowTop, ref bNarrowTop);
            
            fCenter = bPair ? spacing + mid_width : 0;

            //account for etch factor
#if false
            //HL version
            double thick = 0;
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref thick);

            fBottomWidth = mid_width;
            fTopWidth = fBottomWidth * etch; //ZZero version
            double hlEtch = (fBottomWidth - fTopWidth) / thick;
            fLeftEtch = (float)hlEtch;
            fRightEtch = (float)hlEtch;
#else
            //ZZero version
            //--etch = 0.9f; //!!!TMP
            bool bUseEtch = ZPlannerManager.StackupPanel != null && ZPlannerManager.IsTrapezoidalTraces;
            if (bUseEtch)
            {
                float fWide = mid_width; //if wide = nominal //2 * mid_width / (1 + etch); //if nominal = middle
                float fNarrow = fWide * etch;
                if (bNarrowTop)
                {
                    fBottomWidth = fWide;
                    fTopWidth = fNarrow;
                }
                else
                {
                    fBottomWidth = fNarrow;
                    fTopWidth = fWide;
                }
                fLeftEtch = etch;
                fRightEtch = etch;
            }
            else
            {
                fBottomWidth = mid_width;
                fTopWidth = mid_width;
            }
#endif
            //

            iPotential = bPair ? 2 : 1; 
        }

        public FX_Trace(ZLayer zl) //for single-ended impedance
        {
            iLayerIndex = zl.Stackup.GetLayerOfStackupIndex(zl.ID) + 1;//1-based
            fCenter = 0;
            
            //width
            ZLayer zSingleImpedanceLayer = zl.Stackup.ActiveSingle.GetLayerOfSingleImpedance(zl.ID);

            double dval = 0;
            ZPlannerProject.GetLayerParameterValue(zSingleImpedanceLayer, ZStringConstants.ParameterIDZo_TraceWidth, ref dval);
            dval *= Units.MilsToMeters;
            float mid_width = (float)dval;

            dval = 0;
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDEtchFactor, ref dval);
            float etch = (float)dval;

            bool bNarrowTop = true;
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDNarrowTop, ref bNarrowTop);

            bool bUseEtch = ZPlannerManager.StackupPanel != null && ZPlannerManager.IsTrapezoidalTraces;
            if (bUseEtch)
            {
#if false
                //HL version
                double thick = 0;
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref thick);

                fBottomWidth = mid_width;
                fTopWidth = fBottomWidth * etch; //ZZero version
                double hlEtch = (fBottomWidth - fTopWidth) / thick;
                fLeftEtch = (float)hlEtch;
                fRightEtch = (float)hlEtch;
#else
                //ZZero version
                //--etch = 0.9f;//default
                float fWide = mid_width;//if wide = nominal //2 * mid_width / (1 + etch); //if mid_width = nominal
                float fNarrow = fWide * etch;
                if (bNarrowTop) {
                    fBottomWidth = fWide;
                    fTopWidth = fNarrow;
                }
                else
                {
                    fBottomWidth = fNarrow;
                    fTopWidth = fWide;
                }
                fLeftEtch = etch;
                fRightEtch = etch;
#endif
            }
            else
            {
                fBottomWidth = mid_width;
                fTopWidth = mid_width;
            }
            //

            iPotential = 0; //will be changed later
        }

        public FX_Trace(int layIndex) //init from defaults
        {
            iLayerIndex = layIndex;
            fCenter = 0;
            fBottomWidth = Units.fMilsToMeters * 10 / 1.9f;  //5 mils centerline, etch = 0.9
            fTopWidth = fBottomWidth * 0.9f;

            fLeftXtoPlane = Trace2Plane;
            fLeftEtch = 0.9f;
            fRightXtoPlane = Trace2Plane;
            fRightEtch = 0.9f;

            iPotential = 0;
        }

        public int iLayerIndex;		// Trace's layer, defined by index into fslLayers;
        // must index to a metal layer. iLayerIndex starts
        // with 1, so it corresponds to fslLayers[iLayerIndex-1]
        public float fCenter;				// X position of trace's center
        public float fTopWidth;			// Width of trace's top
        public float fBottomWidth;		// Width of trace's bottom
        // For plane layers with routing
        public float fLeftXtoPlane;		// Left-side clearance between trace and plane;
        public float fLeftEtch;        // Plane etch at the left side of this trace
        //    required only if trace's layer is a plane
        public float fRightXtoPlane;	// Right-side clearance between trace and plane;
        public float fRightEtch;       // Plane etch at the left side of this trace
        //    required only if trace's layer is a plane
        // To enable combining traces under the same potential
        public int iPotential;			// index starting with 1 (must be the same for traces 
        // that share the same potential; 0 reserved for ground
        // See additional comment below.    
    }

    class FX_Input
    {
        public List<FX_Layer> layers;
        public List<FX_Trace> traces;
        // Parameters for boundary of board
        public float fLeftEdgeX;		// Position of left and right edges of PCB;
        public float fRightEdgeX;	//	ignored if bDoEdgeEffects == FALSE
        // Characteristics of the space	above, below,
        // and (if bDoEdgeEffects) at left and right edges of board
        public float fSpaceDielConstant = 1;        // usually 1.0 (air) 
        // lossy lines support
        public float fSpaceLossTangent = 0;	// usually 0, but who knows...
        public float fSpaceConductivity = 0;  // usually 0, but who knows...

        public string passBuffer;

        public FX_Input()
        {
            passBuffer = "";
        }

        public FX_Input(ZStackup stk)
        {
            passBuffer = "";

            layers = new List<FX_Layer>();
            traces = new List<FX_Trace>();

            int iLayer = 0;
            foreach (ZLayer zl in stk.Layers)
            {
                ZLayerType? ztype = zl.GetLayerType();
                bool bOuterMetal = false;
                if (zl.isMetal() && ((iLayer == 0) || (iLayer == stk.Layers.Count - 1))){
                    bOuterMetal = true;
                }

                LAYER_TYPE ltype = ConvertLayerType(ztype);
                bool isMetal = ltype != LAYER_TYPE.FS_DIEL;

                //!- FX_Layer fL = new FX_Layer(ztype); //default parameters
                FX_Layer fL = new FX_Layer(zl, bOuterMetal);         // actual layer data
                layers.Add(fL);

                //traces
                if (isMetal)
                {
                    //!- FX_Trace tr = new FX_Trace(layerIdx); //default parameters
                    FX_Trace tr = new FX_Trace(zl);  //actual trace data
                    tr.iPotential = 1;
                    traces.Add(tr);
                }
                iLayer++;
            }

            // redefine Dk 
            // 1) for inner metal layers from adjacent dielectrics
            // 2) for microstrip layers Dk = 1 (Air)
            iLayer = 0;
            int N = stk.Layers.Count - 1; //last layer Idx
            foreach (ZLayer zl in stk.Layers)
            {
                ZLayerType? ztype = zl.GetLayerType();
                if (ztype == ZLayerType.Signal || ztype == ZLayerType.SplitMixed)
                {
                    bool bTopMicrostrip = (iLayer == 0) || ((iLayer == 1) && (stk.Layers[0].GetLayerType() == ZLayerType.SolderMask));
                    bool bBotMicrostrip = (iLayer == N) || ((iLayer == N-1) && (stk.Layers[N].GetLayerType() == ZLayerType.SolderMask));
                    bool bMicrostripMetal = bTopMicrostrip || bBotMicrostrip;
                    if (bMicrostripMetal)
                    {
                        layers[iLayer].fDielConstant = 1; //Air
                    }
                    else
                    //if ((iLayer > 0) && (iLayer < stk.Layers.Count - 1)) //inner metal layers
                    {
                        ZLayer zPrev = stk.Layers[iLayer - 1], zNext = stk.Layers[iLayer + 1];
                        FX_Layer lPrev = layers[iLayer - 1], lNext = layers[iLayer + 1];
                        double Dk = (lPrev.fDielConstant + lNext.fDielConstant) / 2;
                        if (zPrev.GetLayerType() == ZLayerType.Prepreg){
                            Dk = lPrev.fDielConstant;
                        }
                        else if (zNext.GetLayerType() == ZLayerType.Prepreg)
                        {
                            Dk = lNext.fDielConstant;
                        }
                        layers[iLayer].fDielConstant = (float)Dk;
                    }
                }                
                iLayer++;
            }        
        }

        public bool CreateSingleEndTraces(ZStackup stk, ZSingle single)
        {
            bool retval = false;
            traces.Clear();
            foreach (ZLayer zl in stk.Layers)
            {
                ZLayer seLayer = single.GetLayerOfSingleImpedance(zl.ID);

                ZLayerType? ztype = zl.GetLayerType();
                LAYER_TYPE ltype = ConvertLayerType(ztype);
                bool isMetal = ltype != LAYER_TYPE.FS_DIEL;

                if (isMetal)
                {
                    bool bLayerOn = false;
                    if (!ZPlannerProject.GetLayerParameterValue(seLayer, ZStringConstants.ParameterIDZo_IsUsed, ref bLayerOn))
                    {
                        continue;
                    }
                    if (bLayerOn && seLayer.HasValues())
                    {
                        retval = true;
                        FX_Trace tr = new FX_Trace(zl);  //actual trace data
                        tr.iPotential = 1;
                        traces.Add(tr);
                    }
                }
            }
            return retval;
        }

        public bool CreateDiffPairTraces(ZStackup stk, ZPair pair)
        {
            bool retval = false;
            traces.Clear();
            // get traces from the diff. pair
            foreach (ZLayer zLayer in stk.Layers)
            {
                //get pair layer
                ZLayer diffLayer = stk.GetLayerOfPairImpedance(zLayer.ID, pair.ID);
                ZLayerType? ztype = zLayer.GetLayerType();
                LAYER_TYPE ltype = ConvertLayerType(ztype);
                bool isMetal = ltype != LAYER_TYPE.FS_DIEL;

                if (isMetal)
                {
                    bool bLayerOn = false;
                    if (!ZPlannerProject.GetLayerParameterValue(diffLayer, ZStringConstants.ParameterIDZdiff_IsUsed, ref bLayerOn))
                    {
                        continue;
                    }
                    if (bLayerOn && diffLayer.HasValues())
                    {
                        retval = true;
                        FX_Trace tr1 = new FX_Trace(stk, diffLayer, false);  //actual trace data
                        traces.Add(tr1);
                        FX_Trace tr2 = new FX_Trace(stk, diffLayer, true);  //actual trace data
                        traces.Add(tr2);
                    }
                }
            }
            return retval;
        }

        public static LAYER_TYPE ConvertLayerType(ZLayerType? ztype)
        {
            LAYER_TYPE ltype = LAYER_TYPE.FS_DIEL;

            switch (ztype) //metal layers
            {
                case ZLayerType.Plane:
                    ltype = LAYER_TYPE.FS_PLANE;
                    break;
                case ZLayerType.Signal:
                case ZLayerType.SplitMixed:
                    ltype = LAYER_TYPE.FS_ROUTING;
                    break;
            }
            return ltype;
        }

        private void PassData(string s)
        {
            if (passBuffer.Length > 0)
            {
                passBuffer += ";";
            }
            passBuffer += s;
        }

        private void PassBuffer()
        {
            FX.PassData(passBuffer);
            passBuffer = "";
        }

        private void PassStackup()
        {
            //general parameters <name:value>
            PassData(String.Format("fLeftEdgeX:{0}", fLeftEdgeX));
            PassData(String.Format("fRightEdgeX:{0}", fRightEdgeX));
            PassData(String.Format("fSpaceDielConstant:{0}", fSpaceDielConstant));
            PassData(String.Format("fSpaceLossTangent:{0}", fSpaceLossTangent));
            PassData(String.Format("fSpaceConductivity:{0}", fSpaceConductivity));

            int idx = 0;
            foreach(FX_Layer L in layers){
                PassData(String.Format("lindex:{0}", idx)); idx++;
                PassData(String.Format("fThickness:{0}", L.fThickness));
                PassData(String.Format("fDielConstant:{0}", L.fDielConstant));
                PassData(String.Format("iLayerType:{0}", (int)L.iLayerType));
                PassData(String.Format("bIsConformal:{0}", L.bIsConformal ? "1" : "0"));
                PassData(String.Format("fRmaterial:{0}", L.fRmaterial));
                PassData(String.Format("fLossTangent:{0}", L.fLossTangent));
                // TBD  (Temporary fix)
                //FX.PassData(String.Format("fConductivity:{0}", L.fConductivity));
                PassData(String.Format("fConductivity:{0}", 0f));
                PassData(String.Format("fTopRoughness:{0}", L.fTopRoughness));
                PassData(String.Format("fBottomRoughness:{0}", L.fBottomRoughness));
                PassData(String.Format("fThicknessAboveTrace:{0}", L.fThicknessAboveTrace));
                PassData(String.Format("fThicknessBetweenTraces:{0}", L.fThicknessBetweenTraces));
            }
        }

        public void PassTrace(int traceIdx, int fsIndex)
        {
            FX_Trace T = traces[traceIdx];

            PassData(String.Format("tindex:{0}", fsIndex));
            PassData(String.Format("iLayerIndex:{0}", T.iLayerIndex));
            PassData(String.Format("fCenter:{0}", T.fCenter));
            PassData(String.Format("fTopWidth:{0}", T.fTopWidth));
            PassData(String.Format("fBottomWidth:{0}", T.fBottomWidth));
            PassData(String.Format("fLeftXtoPlane:{0}", T.fLeftXtoPlane));
            PassData(String.Format("fLeftEtch:{0}", T.fLeftEtch));
            PassData(String.Format("fRightXtoPlane:{0}", T.fRightXtoPlane));
            PassData(String.Format("fRightEtch:{0}", T.fRightEtch));
            PassData(String.Format("iPotential:{0}", T.iPotential));

            PassBuffer();
        }

        //used for altering Split/Mixed Layer type for the current FX run. Can be FS_ROUTING = 0, FS_PLANE = 1
        //!!! must be restored after FX run
        public void PassLayerTypeChange(int idx, int iLayerType)
        {
            PassData(String.Format("lindex:{0}", idx)); 
            PassData(String.Format("iLayerType:{0}", iLayerType));

            PassBuffer();
        }
        /*
        public void PassDataOneTrace(int traceIdx)
        {
            PassData(String.Format("iLayerCount:{0}", layers.Count));
            PassData(String.Format("iTraceCount:{0}", 1));

            PassStackup();
            PassTrace(traceIdx, 0);
        }

        public void PassDataTwoTraces(int traceIdx1, int traceIdx2)
        {
            PassData(String.Format("iLayerCount:{0}", layers.Count));
            PassData(String.Format("iTraceCount:{0}", 2));

            PassStackup();
            PassTrace(traceIdx1, 0);
            PassTrace(traceIdx2, 1);
        }
        */
        // more efficient interface
        public void PassCommonData(bool bSingle)
        {
            PassData(String.Format("iLayerCount:{0}", layers.Count));
            PassData(String.Format("iMaxTraceCount:{0}", traces.Count));
            PassData(String.Format("iTraceCount:{0}", bSingle ? 1 : 2)); 

            PassStackup();

            PassBuffer();
        }

        public void PassFinish()
        {
            FX.PassData(String.Format("Done:{0}", 0));
        }
    }
#endif

    [DataContract]
    public class FX_Output
    {
        [DataMember]
        public int	iNumberOfModes;		// number of propagation modes
        [DataMember]
        public bool bIsHomog;				// TRUE if cross section is approximately homogeneous

        [DataMember]
        public List<float> fCapacitance;			// Capacitance matrix (F/m)					[n*n]
        [DataMember]
        public List<float> fInductance;			// Inductance matrix (H/m)						[n*n]
        [DataMember]
        public List<float> fPropSpeeds;			// Vector of propagation speeds (m/s)		[n]
        [DataMember]
        public List<float> fUBasisVectors;		// Matrix "U" of basis vectors				[n*n]
        [DataMember]
        public List<float> fUInverse;				// Inverse of matrix U							[n*n]
        [DataMember]
        public List<float> fZzero;					// Characteristic impedance matrix (ohm)	[n*n]
        [DataMember]
        public List<float> fEquivR;				// Matrix of "equivalent resistors" that synthesize Z0 (ohm) [n*n]
        [DataMember]
        public List<float> fDcResistance;		// DC Resistance matrix (see comment above) (ohm/m) [n*n]
        // lossy lines support
        [DataMember]
        public List<float> fSkinResistance;		// Resistance matrix at 1 GHz (ohm/m)     [n*n]
                                    // due to skin effect (means sqrt model)	
        [DataMember]
        public List<float> fGzero;             // G Matrix (1/(ohm*m)) at DC             [n*n]
        [DataMember]
        public List<float> fGMatrix;			   // G Matrix (1/(ohm*m)) at 1 GHz          [n*n]
        [DataMember]
        public List<float> fRoughnessResistance;   // Resistance matrix at 1 GHz (ohm/m) [n*n]
                                  // due to surface roughness
       // overall error code
        [DataMember]
        public FS_ERROR_CODE iErrorCode;		// Error code

        public FX_Output()
        {

        }

        public static FX_Output Create(String fx_output)
        {
            //deserialize
            DataContractJsonSerializer js = new DataContractJsonSerializer(typeof(FX_Output));

            using (MemoryStream ms = new MemoryStream(System.Text.ASCIIEncoding.ASCII.GetBytes(fx_output)))
            {
                try
                {
                    FX_Output outp = (FX_Output)js.ReadObject(ms);
                    return outp;
                }
                catch {
                    return null;
                }
            }
        }
    }

    
    class FX
    {
        // interaction with C++
        //C++: 
        //     int FXExecute(char* output, int len)
        //     void FXData(char* input) 

        //C#
#if FALSE
#if DEBUG
        [DllImport("FieldSolverd.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FXData(String input);

        [DllImport("FieldSolverd.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int FXExecute(StringBuilder output, int len);
#else
        [DllImport("FieldSolver.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FXData(String input);

        [DllImport("FieldSolver.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int FXExecute(StringBuilder output, int len);
#endif
#endif

#if DEBUG
        [DllImport("Z0FSd.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FXData(String input);

        [DllImport("Z0FSd.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int FXExecute(StringBuilder output, int len);
#else
        [DllImport("Z0FS.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FXData(String input);

        [DllImport("Z0FS.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int FXExecute(StringBuilder output, int len);
#endif
        public static void PassData(String s)
        {
            //call C++ code
            FXData(s);
        }

#if ZSANDBOX==false
        public static int Run(ZStackup stackup)
        {
            Cursor currentCursor = Cursor.Current;
            Cursor.Current = Cursors.WaitCursor;

            FX_Input fxinp = new FX_Input(stackup);
            fxinp.PassCommonData(true);

            int retval = RunSingleEnded(stackup, ref fxinp);

            if (retval == 0)
            {
                FX.PassData(String.Format("iTraceCount:{0}", 2)); 
                retval = RunDiffPairs(stackup, ref fxinp);
            }

            fxinp.PassFinish();
            
            Cursor.Current = currentCursor;
            return retval;
        }

        private static Dictionary<string, FX_Output> fxResults_SE = new Dictionary<string, FX_Output>();
        private static Dictionary<string, FX_Output> fxResults_Diff = new Dictionary<string, FX_Output>();

        public static void LossViewer(ZLayer zl, ZSingle single)
        {
            string id = zl.ID;
            FX_Output fxOut = null;
            bool bValid = false;
            if (fxResults_SE.Count > 0)
            {
                string diffId = single.ID + "_" + id;
                bValid = fxResults_SE.TryGetValue(id, out fxOut);
            }

            if (bValid)
            {
                string t = zl.Stackup.GetLayerOfStackup(id).GetLayerParameterValue(ZStringConstants.ParameterIDThickness);
                string n = zl.Stackup.GetLayerOfStackup(id).GetLayerParameterValue(ZStringConstants.ParameterIDLayerName);
                string s = String.Format("Stackup: {0}, Layer: {1}, Type: {2}, Thickness: {3} (mils)", zl.Stackup.Title, n, zl.GetLayerType(), t);
                LossPlot dlg = new LossPlot(fxOut, ZPlannerManager.IsRoughness, s);
                dlg.ShowDialog();
            }
            else
            {
                string err = "Can't run Loss Viewer due to errors in the stackup." + Environment.NewLine + "Please run DRC for details.";
                MessageBox.Show(err, "Stackup Error");
            }
        }

        public static void LossViewer(ZLayer zl, ZPair zDifferential)
        {
            string id = zl.ID;
            FX_Output fxOut = null;
            bool bValid = false;
            if (zDifferential == null) return;

            if (fxResults_Diff.Count > 0){
                string diffId = zDifferential.ID + "_" + id;
                bValid = fxResults_Diff.TryGetValue(diffId, out fxOut);
            }            
            if (bValid){                
                string t = zl.Stackup.GetLayerOfStackup(id).GetLayerParameterValue(ZStringConstants.ParameterIDThickness);
                string n = zl.Stackup.GetLayerOfStackup(id).GetLayerParameterValue(ZStringConstants.ParameterIDLayerName);
                string s = String.Format("Stackup: {0}, Layer: {1}, Type: {2}, Thickness: {3} (mils)", zl.Stackup.Title, n, zl.GetLayerType(), t);
                LossPlot dlg = new LossPlot(fxOut, ZPlannerManager.IsRoughness, s);
                dlg.ShowDialog();                
            }
            else
            {
                string err = "Can't run Loss Viewer due to errors in the stackup." + Environment.NewLine + "Please run DRC for details.";
                MessageBox.Show(err, "Stackup Error");
            }
        }

        public static ZLayer GetMetalLayerByNumber(ZStackup stackup, int index)
        {
            int idx = 1;
            foreach (ZLayer zl in stackup.Layers)
            {
                if (zl.isMetal())
                {
                    if (idx == index) return zl;
                    idx++;
                }
            }
            return null;
        }

        public static void FXReferences(ZSingle single, int layerIndex /*0-based*/, out int iTopRef, out int iBotRef)
        {
            ZStackup stackup = single.Stackup;
            ZLayer baseLayer = stackup.GetLayerOfStackup(layerIndex);

            //get defaults
            ZLayer.PlaneReferences pR = new ZLayer.PlaneReferences();
            baseLayer.GetPlaneReferences(out pR);

            ZLayer topRef = pR.defUp, botRef = pR.defDown;

            //override with actuals            
            ZLayer singleLayer = single.GetLayerOfSingleImpedance(layerIndex);

            string topId = singleLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_TopReference);
            if (!string.IsNullOrWhiteSpace(topId)) topRef = stackup.GetLayerOfStackup(topId);
            string botId = singleLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_BottomReference);
            if (!string.IsNullOrWhiteSpace(botId)) botRef = stackup.GetLayerOfStackup(botId);

            iTopRef = iBotRef = -1;
            if (topRef != null) iTopRef = stackup.GetLayerOfStackupIndex(topRef.ID);
            if (botRef != null) iBotRef = stackup.GetLayerOfStackupIndex(botRef.ID);
        }

        public static void FXReferences(ZPair pair, int layerIndex /*0-based*/, out int iTopRef, out int iBotRef)
        {
            ZStackup stackup = pair.Stackup;
            ZLayer baseLayer = stackup.GetLayerOfStackup(layerIndex);

            //get defaults
            ZLayer.PlaneReferences pR = new ZLayer.PlaneReferences();
            baseLayer.GetPlaneReferences(out pR);

            ZLayer topRef = pR.defUp, botRef = pR.defDown;

            //override with actuals            
            ZLayer pairLayer = pair.GetLayerOfPairImpedance(layerIndex);

            string topId = pairLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_TopReference);
            if (!string.IsNullOrWhiteSpace(topId)) topRef = stackup.GetLayerOfStackup(topId);
            string botId = pairLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_BottomReference);
            if (!string.IsNullOrWhiteSpace(botId)) botRef = stackup.GetLayerOfStackup(botId);

            iTopRef = iBotRef = -1;
            if (topRef != null) iTopRef = stackup.GetLayerOfStackupIndex(topRef.ID);
            if (botRef != null) iBotRef = stackup.GetLayerOfStackupIndex(botRef.ID);
        }

        private static int RunSingleEnded(ZStackup stackup, ref FX_Input fxinp)
        {
            fxResults_SE.Clear();
            //FX_Input fxinp = new FX_Input(stackup);

            //fxinp.PassCommonData(true);
            int retval = 0;
            foreach (ZSingle single in stackup.Singles)
            {
                string singleId = single.ID;
                if (!fxinp.CreateSingleEndTraces(stackup, single)) continue;
                
                for (int i = 0; i < fxinp.traces.Count; i++)
                {
                    int idx = fxinp.traces[i].iLayerIndex - 1;
                    ZLayer baseLayer = stackup.GetLayerOfStackup(idx);

                    int iRefUp = -1, iRefDown = -1;
                    FXReferences(single, idx, out iRefUp, out iRefDown);

                    if (iRefUp < 0 && iRefDown < 0) continue; //no valid refernce

                    bool bChangeUp = false, bChangeDown = false;
                    if (iRefUp >= 0 && fxinp.layers[iRefUp].bIsMixed)
                    {
                        bChangeUp = true;
                    }
                    if (iRefDown >= 0 && fxinp.layers[iRefDown].bIsMixed)
                    {
                        bChangeDown = true;
                    }
                    
                    String sOutput = "";

                    fxinp.PassTrace(i, 0);

                    if (bChangeUp)   fxinp.PassLayerTypeChange(iRefUp, 1); //to Plane
                    if (bChangeDown) fxinp.PassLayerTypeChange(iRefDown, 1); //to Plane
                    
                    retval = Run(ref sOutput);

                    if (bChangeUp) fxinp.PassLayerTypeChange(iRefUp, 0); //back to Signal
                    if (bChangeDown) fxinp.PassLayerTypeChange(iRefDown, 0); //back to Signal

                    if (retval != 0) break; //error

                    FX_Output fxout = new FX_Output();
                    fxout = FX_Output.Create(sOutput);

                    //write result to the DB
                    if (fxout != null)
                    {
                        int layerIndex = fxinp.traces[i].iLayerIndex - 1;
                        string layerID = stackup.GetLayerOfStackup(layerIndex).ID;

                        fxResults_SE.Add(singleId + "_" + layerID, fxout);

                        // get impedance layer object
                        ZLayer zl = single.GetLayerOfSingleImpedance(layerID);
                        // set impedance
                        ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_Zo, fxout.fZzero[0]);

                        //calculate insertion loss [db/inch]
                        FXLosses loss = new FXLosses(fxout);
                        double f = zl.GetFrequency();//--stackup.Frequency;//must be in GHz
                        double R = loss.Attenuation_R(f);
                        double Ro = loss.Attenuation_Ro(f);
                        double D = loss.Attenuation_D(f);
                        double totalLoss = (R + D) * 0.0254;//[db/inch]
                        if (ZPlannerManager.IsRoughness) totalLoss += Ro * 0.0254;

                        //--zl = stackup.GetLayerOfStackup(layerID);
                        ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_InsertionLoss, totalLoss);

                        //propagation velocity
                        ZLayer lay = stackup.GetLayerOfStackup(layerIndex);
                        double PropSpeed = fxout.fPropSpeeds[0]; // [m/s]
                        double c1 = Units.MetersToMils * 0.001; //meters to inch
                        double c2 = 1.0e-9; //S to ns

                        PropSpeed = PropSpeed * c1 * c2; //[inch/ns]
                        ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_PropagationVelocity, PropSpeed);
                    }
                }
            }

            //fxinp.PassFinish();

            return retval;            

        }

        private static int RunDiffPairs(ZStackup stackup, ref FX_Input fxinp)
        {
            //FX_Input fxinp = new FX_Input(stackup);

            //fxinp.PassCommonData(false);
            fxResults_Diff.Clear();

            int retval = 0;
            foreach (ZPair pair in stackup.Pairs)
            {
                string dpId = pair.ID;

                //FX_Input fxinp = new FX_Input(stackup, pair);
                if (fxinp.CreateDiffPairTraces(stackup, pair)){

                    for (int i = 0; i < fxinp.traces.Count; i += 2)
                    {
                        int idx = fxinp.traces[i].iLayerIndex - 1;
                        ZLayer baseLayer = stackup.GetLayerOfStackup(idx);

                        int iRefUp = -1, iRefDown = -1;
                        FXReferences(pair, idx, out iRefUp, out iRefDown);

                        bool bChangeUp = false, bChangeDown = false;
                        if (iRefUp >= 0 && fxinp.layers[iRefUp].bIsMixed)
                        {
                            bChangeUp = true;
                        }
                        if (iRefDown >= 0 && fxinp.layers[iRefDown].bIsMixed)
                        {
                            bChangeDown = true;
                        }

                        String sOutput = "";

                        //--fxinp.PassDataTwoTraces(i, i + 1);
                        fxinp.PassTrace(i, 0);
                        fxinp.PassTrace(i + 1, 1);

                        if (bChangeUp) fxinp.PassLayerTypeChange(iRefUp, 1); //to Plane
                        if (bChangeDown) fxinp.PassLayerTypeChange(iRefDown, 1); //to Plane

                        retval = Run(ref sOutput);

                        if (bChangeUp) fxinp.PassLayerTypeChange(iRefUp, 0); //back to Signal
                        if (bChangeDown) fxinp.PassLayerTypeChange(iRefDown, 0); //back to Signal

                        if (retval != 0) return retval;

                        FX_Output fxout = new FX_Output();
                        fxout = FX_Output.Create(sOutput);

                        if (fxout != null)
                        {
                            double ZDiff = 2 * (fxout.fZzero[0] - fxout.fZzero[1]); //2*(Z11-Z12)

                            //write result to the DB
                            //assuming that both traces are on the same layer
                            int layerIndex = fxinp.traces[i].iLayerIndex - 1;
                            string layerID = stackup.GetLayerOfStackup(layerIndex).ID;

                            fxResults_Diff.Add(dpId + "_" + layerID, fxout);

                            // get diffpair layer object
                            ZLayer zDpLayer = pair.GetLayerOfPairImpedance(layerID);
                            // set diff impedance
                            // set impedance
                            ZPlannerProject.SetLayerParameterValue(zDpLayer, ZStringConstants.ParameterIDZdiff_Zo, fxout.fZzero[0]);
                            ZPlannerProject.SetLayerParameterValue(zDpLayer, ZStringConstants.ParameterIDZdiff_Zdiff, ZDiff);

                            //calculate insertion loss [db/inch]
                            FXLosses loss = new FXLosses(fxout);
                            double f = zDpLayer.GetFrequency();//--stackup.Frequency;//must be in GHz
                            double R = loss.Attenuation_R(f);
                            double Ro = loss.Attenuation_Ro(f);
                            double D = loss.Attenuation_D(f);
                            double totalLoss = (R + D) * 0.0254;//[db/inch]
                            if (ZPlannerManager.IsRoughness) totalLoss += Ro * 0.0254;

                            ZPlannerProject.SetLayerParameterValue(zDpLayer, ZStringConstants.ParameterIDZdiff_InsertionLoss, totalLoss);

                            //propagation velocity
                            double PropSpeed = fxout.fPropSpeeds[0]; // [m/s]
                            double c1 = Units.MetersToMils * 0.001; //meters to inch
                            double c2 = 1.0e-9; //S to ns

                            PropSpeed = PropSpeed * c1 * c2; //[inch/ns]
                            ZPlannerProject.SetLayerParameterValue(zDpLayer, ZStringConstants.ParameterIDZdiff_PropagationVelocity, PropSpeed);

                        }
                    }
                }
            }

            //fxinp.PassFinish();

            return retval;

        }
#endif

        private static int Run(ref String sOutput)
        {
            const int buffer_size = 10000;
            StringBuilder sb = new StringBuilder(buffer_size);

            //call C++ code
            int ret = FXExecute(sb, sb.Capacity);
            if (ret == 0)
            {
                sOutput = sb.ToString();
            }
            else
            {
                string messageBoxText = String.Format("Wrong stackup data ({0})", ret);
                string caption = "Field Solver";
                MessageBox.Show(messageBoxText, caption, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            return ret;
        }


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Sandbox
//////////////////////////////////////////////////////////////////////////////////////////////////////
        public static int RunOnSandboxCX(ref sbData data)
        {
            int retval = RunSingleEnded(ref data);
            if (retval == 0)
            {
                retval = RunDifferential(ref data);
            }
            return retval;
        }

        public static int RunDifferential(ref sbData data)
        {
            SB_Input sbInput = new SB_Input(data);

            sbInput.passCX(true);

            // run FX
            int retval = 0;
            String sOutput = "";

            retval = Run(ref sOutput);

            if (retval == 0)
            {
                FX_Output fxout = new FX_Output();
                fxout = FX_Output.Create(sOutput);
                data.outDiff = fxout;

                //write result to sandbox data
                data.Z0diff = fxout.fZzero[0];
                data.Zdiff = 2 * (fxout.fZzero[0] - fxout.fZzero[1]); //2*(Z11-Z12)

                data.R1diff = fxout.fSkinResistance[0];
                data.G1diff = fxout.fGMatrix[0];
                data.G0diff = fxout.fGzero[0];
                data.RRough1diff = fxout.fRoughnessResistance[0];            
            }
            //finish
            PassData(String.Format("Done:{0}", 0));

            return retval;
        }

        public static int RunSingleEnded(ref sbData data)
        {
            SB_Input sbInput = new SB_Input(data);

            sbInput.passCX(false);

            // run FX
            int retval = 0;
            String sOutput = "";

            retval = Run(ref sOutput);

            if (retval == 0)
            {
                FX_Output fxout = new FX_Output();
                fxout = FX_Output.Create(sOutput);

                data.outSE = fxout;

                //write result to sandbox data
                data.Z0 = fxout.fZzero[0];
                data.R1 = fxout.fSkinResistance[0];
                data.G1 = fxout.fGMatrix[0];
                data.G0 = fxout.fGzero[0];
                data.RRough1 = fxout.fRoughnessResistance[0];
            }
            //finish
            PassData(String.Format("Done:{0}", 0));

            return retval;
        }

    }

    class SB_Input
    {
        private sbData data;
        private int trace_layer, layer_count;
        
        public SB_Input(sbData aData)
        {
            data = aData;

            trace_layer = 0;
            layer_count = 0;

            switch (data.type)
            {
                case csType.SurfaceMicrostrip:
                    if (aData.hasSolderMask)
                    {
                        trace_layer = 2;
                        layer_count = 4;
                    }
                    else
                    {
                        trace_layer = 1;
                        layer_count = 3;
                    }
                    break;
                case csType.BuriedMicrostrip:
                    trace_layer = 2;
                    layer_count = 4;
                    break;
                case csType.Stripline:
                    trace_layer = 3;
                    layer_count = 5;
                    break;
            }
        }

        private void sbLayer(int stk_index, LAYER_TYPE type, bool bFirst = false, bool bConformal = false)
        {
            double h = bFirst ? Units.MilsToMeters * data.h2 : Units.MilsToMeters * data.h1;

            double dk = bFirst ? data.Dk2 : data.Dk1;
            double df = bFirst ? data.Df2 : data.Df1;
            double metalDk = stk_index == 0 ? 1 : 4;
            double metalDf = stk_index == 0 ? 0 : 0.02;
            //internal signal layer
            if (stk_index > 0 && type == LAYER_TYPE.FS_ROUTING)
            {
                metalDk = data.Dkr;//--(data.Dk1 + data.Dk2) / 2; //average of adjacent dielectrics
                metalDf = data.Dfr;
            }

            bool bSolderMask = bConformal;//data.hasSolderMask;
            if (bSolderMask)
            {
                dk = data.Dk2;
                df = data.Df2;
                h = Units.MilsToMeters * data.h2;
            }

            FX.PassData(String.Format("lindex:{0}", stk_index));
            FX.PassData(String.Format("fThickness:{0}", type == LAYER_TYPE.FS_DIEL ? h : Units.MilsToMeters * data.t));
            FX.PassData(String.Format("fDielConstant:{0}", type == LAYER_TYPE.FS_DIEL ? dk : metalDk));
            FX.PassData(String.Format("iLayerType:{0}", (int)type));
            FX.PassData(String.Format("bIsConformal:{0}", bConformal ? "1" : "0"));
            FX.PassData(String.Format("fRmaterial:{0}", 1.724e-8));//default = copper
            FX.PassData(String.Format("fLossTangent:{0}", type == LAYER_TYPE.FS_DIEL ? df : metalDf));
            FX.PassData(String.Format("fConductivity:{0}", 0));
            FX.PassData(String.Format("fTopRoughness:{0}", type == LAYER_TYPE.FS_DIEL ? 0 : data.Rz1 * 1.0e-6f * (float)Units.RzToRMS));//3.5e-6f));
            FX.PassData(String.Format("fBottomRoughness:{0}", type == LAYER_TYPE.FS_DIEL ? 0 : data.Rz2 * 1.0e-6f * (float)Units.RzToRMS));//3.5e-6f));
            FX.PassData(String.Format("fThicknessAboveTrace:{0}", bSolderMask ? Units.MilsToMeters * data.h2 : 0));
            FX.PassData(String.Format("fThicknessBetweenTraces:{0}", bSolderMask ? Units.MilsToMeters * data.h2_2 : 0));
        }

        private void sbTrace(bool bDifferential)
        {
            //--double mid_width = Units.MilsToMeters * data.w;
            //--double etch = (data.isEtch) ? data.etch : 0;
            double fBottomWidth = Units.MilsToMeters * data.w;//mid_width;//2 * mid_width / (1 + etch);
            double fTopWidth = Units.MilsToMeters * data.w2;//fBottomWidth * etch;
            double etch = (fBottomWidth - fTopWidth) / data.t;
            double fLeftEtch = etch;
            double fRightEtch = etch;

            FX.PassData(String.Format("tindex:{0}", bDifferential ? 1 : 0));
            FX.PassData(String.Format("iLayerIndex:{0}", trace_layer));
            FX.PassData(String.Format("fCenter:{0}", bDifferential ? Units.MilsToMeters * data.s + fBottomWidth : 0));
            FX.PassData(String.Format("fTopWidth:{0}", fTopWidth));
            FX.PassData(String.Format("fBottomWidth:{0}", fBottomWidth));
            FX.PassData(String.Format("fLeftXtoPlane:{0}", 0));
            FX.PassData(String.Format("fLeftEtch:{0}", etch));
            FX.PassData(String.Format("fRightXtoPlane:{0}", 0));
            FX.PassData(String.Format("fRightEtch:{0}", etch));
            FX.PassData(String.Format("iPotential:{0}", bDifferential ? 2 : 1));
        }

        public void passCX(bool bDifferential)
        {
            //prolog
            FX.PassData(String.Format("iLayerCount:{0}", layer_count));
            FX.PassData(String.Format("iMaxTraceCount:{0}", 2));
            FX.PassData(String.Format("iTraceCount:{0}", bDifferential ? 2 : 1));

            FX.PassData(String.Format("fLeftEdgeX:{0}", 0));
            FX.PassData(String.Format("fRightEdgeX:{0}", 0));
            FX.PassData(String.Format("fSpaceDielConstant:{0}", 1));
            FX.PassData(String.Format("fSpaceLossTangent:{0}", 0));
            FX.PassData(String.Format("fSpaceConductivity:{0}", 0));

            //layers
            switch (data.type)
            {
                case csType.SurfaceMicrostrip:
                    if (data.hasSolderMask)
                    {
                        sbLayer(0, LAYER_TYPE.FS_DIEL, true, true);
                        sbLayer(1, LAYER_TYPE.FS_ROUTING);
                        sbLayer(2, LAYER_TYPE.FS_DIEL);
                        sbLayer(3, LAYER_TYPE.FS_PLANE);
                    }
                    else
                    {
                        sbLayer(0, LAYER_TYPE.FS_ROUTING);
                        sbLayer(1, LAYER_TYPE.FS_DIEL);
                        sbLayer(2, LAYER_TYPE.FS_PLANE);
                    }
                    break;
                //sbLayer(int stk_index, LAYER_TYPE type, bool bFirst = true, bool bConformal = false)
                case csType.BuriedMicrostrip:
                    sbLayer(0, LAYER_TYPE.FS_DIEL, true);
                    sbLayer(1, LAYER_TYPE.FS_ROUTING);
                    sbLayer(2, LAYER_TYPE.FS_DIEL);
                    sbLayer(3, LAYER_TYPE.FS_PLANE);
                    break;
                case csType.Stripline:
                    sbLayer(0, LAYER_TYPE.FS_PLANE);
                    sbLayer(1, LAYER_TYPE.FS_DIEL, true);
                    sbLayer(2, LAYER_TYPE.FS_ROUTING);
                    sbLayer(3, LAYER_TYPE.FS_DIEL);
                    sbLayer(4, LAYER_TYPE.FS_PLANE);
                    break;
            }

            //traces
            sbTrace(false);
            if (bDifferential)
            {
                sbTrace(true);
            }
        }      
    }

    class FXLosses
    {
        //Hammerstad and Jensen model
        class HammerstadModel
        {
            private double arg;

            public HammerstadModel(FX_Output fxOut)
            {
                double R1 = fxOut.fSkinResistance[0];
                double Ro1 = fxOut.fRoughnessResistance[0];
                double x = Math.Min(Ro1 / R1, 0.999);
                arg = Math.Tan(0.5*Math.PI*x);
            }
            public double GetArg()
            {
                return arg;
            }
            public double K(double f) //GHz
            {
                f /= 2 * Math.PI;

                double ret = 0;
                ret = 2 * Math.Atan(arg * f) / Math.PI;
                return ret;
            }
        }

        private FX_Output fxOut;
        private HammerstadModel roughModel;

        public FXLosses(FX_Output res)
        {
            fxOut = res;
            roughModel = new HammerstadModel(res);
        }

        public double Attenuation_R(double f) //db/m
        {
            double coef = 20 / Math.Log(10);
            double R = fxOut.fSkinResistance[0] * Math.Sqrt(f);//?mode

            double Z0 = fxOut.fZzero[0];
            return (0.5 * R / Z0) * coef;
        }

        public double Attenuation_Ro(double f) //db/m
        {
            double coef = 20 / Math.Log(10);
            double Z0 = fxOut.fZzero[0];
            //--double R = fxOut.fRoughnessResistance[0] * Math.Sqrt(f);//?mode
            double R = fxOut.fSkinResistance[0] * Math.Sqrt(f);
            double Ro = roughModel.K(f) * R;
            return (0.5 * Ro / Z0) * coef;
        }

        public double Attenuation_D(double f) //db/m
        {
            double coef = 20 / Math.Log(10);
            double G = fxOut.fGzero[0] + f * fxOut.fGMatrix[0];//?mode
            double Z0 = fxOut.fZzero[0];
            return (0.5 * G * Z0) * coef;
        }

    }
}