using System;
using System.IO;
//
using ZZero.ZPlanner;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;


namespace ZZero.ZPlanner.Data
{
    class GlassPitch
    {
        ZLayer layer;
        bool isNanya;
        string construction;
        bool bValid;

        public GlassPitch(ZLayer lay)
        {
            bValid = lay.isDielectric() && !lay.IsMaterialAssigned;
            if (bValid){
                layer = lay;
                string manufacturer = layer.GetLayerParameterValue(ZStringConstants.ParameterIDManufacturer);
                isNanya = manufacturer.ToLower() == "nanya";
                string all_constructions = layer.GetLayerParameterValue(ZStringConstants.ParameterIDConstruction);
                construction = "";
                if (!string.IsNullOrWhiteSpace(all_constructions))
                {
                    //define if NOT multiple constructions
                    string[] groups = all_constructions.Split('/');
                    if (groups.Length == 1)
                    {
                        //get main construction
                        string[] cons = all_constructions.Split('x');
                        switch (cons.Length)
                        {
                            case 1: construction = cons[0];
                                break;
                            case 2: construction = cons[1];
                                break;
                        }
                    }

                }
                bValid = !string.IsNullOrWhiteSpace(construction);
            }
        }

        //should be called once for uninited layers
        public bool Init()
        {
            bool retval = bValid;
            if (bValid)
            {
                string v1 = layer.GetLayerParameterValue(ZStringConstants.ParameterIDFillPitch);
                string v2 = layer.GetLayerParameterValue(ZStringConstants.ParameterIDWeavePitch);
                if (string.IsNullOrWhiteSpace(v1) && string.IsNullOrWhiteSpace(v2))
                {
                    retval = Update();
                }
            }
            return retval;
        }

        //should be called on construction change
        public bool onConstructionChange()
        {
            bool retval = bValid;
            if (bValid)
            {
                retval = Update();
            }
            return retval;
        }

        private bool Update()
        {
            bool retval = false;
            double warpPitch = 0, fillPitch = 0;
            int warpYarnCount = 0, fillYarnCount = 0;
            retval = GetWarpYarnCount(out warpYarnCount) && GetFillYarnCount(out fillYarnCount);
            if (retval)
            {
                warpPitch = 1000.0 / (double)warpYarnCount;
                fillPitch = 1000.0 / (double)fillYarnCount;

                layer.SetLayerParameterValue(ZStringConstants.ParameterIDWarpYarnCount, warpYarnCount.ToString());
                layer.SetLayerParameterValue(ZStringConstants.ParameterIDFillYarnCount, fillYarnCount.ToString());

                layer.SetLayerParameterValue(ZStringConstants.ParameterIDFillPitch, fillPitch.ToString());
                layer.SetLayerParameterValue(ZStringConstants.ParameterIDWeavePitch, warpPitch.ToString());
            }
            return retval;
        }

        private bool GetWarpYarnCount(out int warpYarnCount)
        {
            bool bConstructionExists = true;
            int ret = 0;
            switch (construction)
            {
                case "1027":
                    ret = 75;
                    if (isNanya)
                    {
                        ret = 74;
                    }
                    break;

                case "1037":
                    ret = 70;
                    break;
                case "106":
                    ret = 56;
                    break;
                case "1035":
                    ret = 66;
                    break;
                case "1067":
                    ret = 70;
                    if (isNanya)
                    {
                        ret = 71;
                    }
                    break;

                case "1078":
                    ret = 54;
                    if (isNanya)
                    {
                        ret = 55;
                    }
                    break;

                case "1080":
                    ret = 60;
                    break;
                case "1086":
                    ret = 60;
                    break;
                case "2112":
                    ret = 40;
                    break;
                case "2113":
                    ret = 60;
                    break;
                case "2313":
                    ret = 60;
                    break;
                case "3313":
                    ret = 60;
                    break;
                case "2116":
                    ret = 60;
                    break;
                case "1506":
                    ret = 47;
                    break;
                case "7628":
                    ret = 44;
                    break;
                case "2125":
                    ret = 40;
                    break;
                case "2117":
                    ret = 66;
                    break;
                case "1504":
                    ret = 60;
                    break;
                case "7627":
                    ret = 44;
                    break;
                case "1501":
                    ret = 46;
                    break;
                case "7629":
                    ret = 44;
                    break;
                case "2013":
                    ret = 47;
                    break;
                case "1652":
                    ret = 52;
                    break;
                case "3070":
                    ret = 70;
                    break;
                case "1015":
                    ret = 96;
                    break;
                case "1065":
                    ret = 56;
                    break;
                case "1017":
                    ret = 95;
                    break;
                case "1500":
                    ret = 49;
                    break;
                default:
                    bConstructionExists = false;
                    break;
            }

            warpYarnCount = ret;
            return bConstructionExists;
        }

        private bool GetFillYarnCount(out int fillYarnCount)
        {
            bool bConstructionExists = true;
            int ret = 0;
            switch (construction)
            {
                case "1027":
                    ret = 75;
                    if (isNanya)
                    {
                        ret = 74;
                    }
                    break;

                case "1037":
                    ret = 72;
                    if (isNanya)
                    {
                        ret = 73;
                    }
                    break;

                case "106":
                    ret = 56;
                    break;

                case "1035":
                    ret = 68;
                    break;

                case "1067":
                    ret = 70;
                    if (isNanya)
                    {
                        ret = 69;
                    }
                    break;

                case "1078":
                    ret = 54;
                    if (isNanya)
                    {
                        ret = 53;
                    }
                    break;

                case "1080":
                    ret = 47;
                    break;

                case "1086":
                    ret = 58;
                    break;

                case "2112":
                    ret = 39;
                    if (isNanya)
                    {
                        ret = 40;
                    }
                    break;

                case "2113":
                    ret = 56;
                    break;

                case "2313":
                    ret = 64;
                    break;

                case "3313":
                    ret = 62;
                    break;

                case "2116":
                    ret = 58;
                    break;

                case "1506":
                    ret = 45;
                    break;

                case "7628":
                    ret = 31;
                    if (isNanya)
                    {
                        ret = 33;
                    }
                    break;

                case "2125":
                    ret = 39;
                    break;

                case "2117":
                    ret = 55;
                    break;

                case "1504":
                    ret = 50;
                    break;

                case "7627":
                    ret = 30;
                    break;

                case "1501":
                    ret = 45;
                    break;

                case "7629":
                    ret = 34;
                    break;

                case "2013":
                    ret = 45;
                    break;

                case "1652":
                    ret = 52;
                    break;

                case "3070":
                    ret = 70;
                    break;

                case "1015":
                    ret = 96;
                    break;

                case "1065":
                    ret = 56;
                    break;

                case "1017":
                    ret = 95;
                    break;

                case "1500":
                    ret = 42;
                    break;

                default:
                    bConstructionExists = false;
                    break;
            }

            fillYarnCount = ret;
            return bConstructionExists;
        }
    }
}