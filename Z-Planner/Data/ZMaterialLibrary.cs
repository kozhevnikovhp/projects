using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Commands;
using ZZero.ZPlanner.Data.Attributes;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.Data
{
    public class ZMaterialLibrary
    {
        public ZMaterialLibrary()
        {
            Materials = new ZList<ZMaterial>(this);
            Parameters = new ZList<ZParameter>(this);
            Lists = new ZList<SelectList>(this);
            IsEdited = false;
        }

        public bool IsEdited { get; set; }

        internal ZList<ZMaterial> Materials { get; private set; }
        internal ZList<ZParameter> Parameters { get; private set; }
        internal ZList<SelectList> Lists { get; private set; }

        internal string AddMaterial(ZLayerType type)
        {
            ZMaterial material = null;
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            try
            {
                material = new ZMaterial(type);
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }

            System.Diagnostics.Debug.Assert(material != null, "Incorrect material creation on AddMaterial method.");
            if (material != null)
            {
                AddMaterial(material);
                return material.ID;
            }

            return string.Empty;
        }

        internal void AddMaterial(ZMaterial material)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                if (!ZPlannerManager.IsUserHaveAccessToMaterial(material)) return;
                this.Materials.Add(material);
                IsEdited = true;
                ZPlannerManager.MainMenu.EnableLocalLibrary();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new AddMaterialCommand(material);
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        internal void RemoveMaterial(ZMaterial material)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                if (!ZPlannerManager.IsUserHaveAccessToMaterial(material)) return;
                this.Materials.Remove(material);
                IsEdited = true;
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveMaterialCommand(material);
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        internal void CopyMaterialForStackup(ZMaterial material)
        {
            ZPlannerManager.Clipboard.Clear();
            List<ZEntity> materials = new List<ZEntity>();
            materials.Add(material.Clone());
            ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeyMaterialsForStackup, materials);
        }

        internal void CopyMaterial(ZMaterial material)
        {
            ZPlannerManager.Clipboard.Clear();
            List<ZEntity> materials = new List<ZEntity>();
            materials.Add(material.Clone());
            ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeyMaterials, materials);
        }

        internal void CutMaterial(ZMaterial material)
        {
            ZPlannerManager.Clipboard.Clear();
            List<ZEntity> materials = new List<ZEntity>();
            materials.Add(material.Clone());
            ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeyMaterials, materials);
            this.RemoveMaterial(material);
        }

        public void PasteMaterial()
        {
            if (!ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyMaterials)) return;
            List<ZEntity> materials = ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials] as List<ZEntity>;
            if (materials == null || materials.Count == 0) 
            {
                ZPlannerManager.Clipboard.Clear();
                return;
            }

            ZMaterial material = materials[0] as ZMaterial;
            if (material == null) return;
            material = material.Clone();

            material.IsReadOnly = false;
            material.IsCustom = true;
            //material.SetMaterialCategory(ZPlannerManager.GetMeterialCategoryStringByUser());
            string materialName =  material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDMaterial) ?? string.Empty;
            string materialSuffix = Options.TheOptions.Company;
            if (string.IsNullOrWhiteSpace(materialSuffix)) materialSuffix = "Local";

            material.SetMaterialParameterValue(ZStringConstants.DMLParameterIDMaterial, string.Format("{0} ({1})", materialName, materialSuffix));
            material.SetMaterialParameterValue(ZStringConstants.DMLParameterIDDk, string.Empty);
            material.SetMaterialParameterValue(ZStringConstants.DMLParameterIDDf, string.Empty);
            material.SetMaterialParameterValue(ZStringConstants.DMLParameterIDCategory, ZPlannerManager.GetMeterialCategoryStringByUser());

            AddMaterial(material);
        }

        //----------------------------------------------------------------
        internal void AddLibrary (ZMaterialLibrary other, ZLibraryCategory addAs = ZLibraryCategory.Local)
        {
            //add/merge lists
            foreach (SelectList aList in other.Lists)
            {
                AddSelectList(aList);
            }

            //add/merge parameters
            foreach (ZParameter parameter in other.Parameters)
            {
                //already exists?
                ZParameter p = Parameters.Find(s => s.ID == parameter.ID);
                if (p == null) //not found
                {
                    //ZParameter par = parameter.MakeCopy();                        
                    Parameters.Add(parameter);
                }
            }

            //add materials/check for duplicates
            foreach (ZMaterial material in other.Materials)
            {
                if (Materials.Find(s => s.ID == material.ID) == null)
                {
                    //add only unique materials 
                    material.IsCustom = (addAs != ZLibraryCategory.ZZero);
                    //category
                    if (addAs == ZLibraryCategory.ZZero)
                    {
                        material.SetMaterialCategory(ZLibraryCategory.ZZero);
                    }
                    else if (addAs == ZLibraryCategory.Corporate)
                    {
                        material.SetMaterialCategory(ZLibraryCategory.Corporate);
                    }
                    else
                    {
                        material.SetMaterialCategory(ZLibraryCategory.Local);
                    }
                    this.Materials.Add(material);
                }
            }
        }

        private void AddSelectList(SelectList aList)
        {
            //find in existing lists            
            SelectList result = Lists.Find(s => s.ID == aList.ID);
            if (result != null) //if found - merge
            {
                foreach (string selectItem in aList.GetValues())
                {
                    string str = result.GetValues().Find(y => y == selectItem);
                    if (String.IsNullOrEmpty(str))//not in list
                    {
                        result.AddValue(str);
                    }
                }
            }
            else //if not found - add
            {                
                SelectList selectList = new SelectList(aList.ID);
                foreach (string selectItem in aList.GetValues())
                {
                    selectList.AddValue(selectItem);
                }
                Lists.Add(selectList);
            }
        }

        internal void ClearLibrary(ZLibraryCategory category)
        {
            this.Materials.RemoveAll(x => x.GetMaterialCategory() == category);
        }

        // Dependency between Manufacturer and Material Name
        private Dictionary<string, List<string>> materialManufacturerDictionary = new Dictionary<string, List<string>>();

        public void AddMaterialManufacturer(string manufacturer, string material)
        {
            if (string.IsNullOrWhiteSpace(manufacturer) || string.IsNullOrWhiteSpace(material)) return;

            List<string> materials;
            if (materialManufacturerDictionary.ContainsKey(manufacturer))
            {
                materials = materialManufacturerDictionary[manufacturer];
            }
            else
            {
                materials = new List<string>();
                materialManufacturerDictionary.Add(manufacturer, materials);
            }

            if (!materials.Contains(material)) materials.Add(material);
        }

        public List<string> GetMaterialsByManufacturer(string manufacturer)
        {
            if (materialManufacturerDictionary.ContainsKey(manufacturer))
            {
                return materialManufacturerDictionary[manufacturer];
            }
            
            return new List<string>();
        }

        public List<string> GetMaterialsByManufacturers(IEnumerable<object> manufacturers)
        {
            List<string> materials = new List<string>();
            foreach (var obj in manufacturers)
            {
                string manufacturer = obj as string;
                if (!string.IsNullOrWhiteSpace(manufacturer) && materialManufacturerDictionary.ContainsKey(manufacturer))
                {
                    materials.AddRange(materialManufacturerDictionary[manufacturer]);
                }
            }

            return materials;
        }

        public void ExtractMaterialManufacturerDictionary()
        {
            materialManufacturerDictionary.Clear();

            foreach(ZMaterial material in Materials)
            {
                AddMaterialManufacturer(material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDManufacturer), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDMaterial));
            }
        }

        internal void AddDMLParameters(ZList<ZParameter> parameters, ref int order)
        {
            List<string> ignoreParameters = new List<string>() { ZStringConstants.DMLParameterIDCategory, ZStringConstants.DMLParameterIDQualified, ZStringConstants.DMLParameterIDManufacturer,
                ZStringConstants.DMLParameterIDMaterial, ZStringConstants.DMLParameterIDType, ZStringConstants.DMLParameterIDConstruction, ZStringConstants.DMLParameterIDNormalizedConstruction,
                ZStringConstants.DMLParameterIDResin, ZStringConstants.DMLParameterIDH, ZStringConstants.DMLParameterIDDk, ZStringConstants.DMLParameterIDDf, ZStringConstants.DMLParameterIDHide,
                ZStringConstants.DMLParameterIDCost, ZStringConstants.DMLParameterIDDfType, ZStringConstants.DMLParameterIDDatasheetURL, ZStringConstants.DMLParameterIDWarpYarnCount,
                ZStringConstants.DMLParameterIDFillYarnCount, ZStringConstants.DMLParameterIDWeavePitch, ZStringConstants.DMLParameterIDFillPitch, ZStringConstants.DMLParameterIDSquareWeave,
                ZStringConstants.DMLParameterIDLeadTime, ZStringConstants.DMLParameterIDDescription, ZStringConstants.DMLParameterIDComments, ZStringConstants.DMLParameterIDTgType,
                ZStringConstants.DMLParameterIDDf_01GHz, ZStringConstants.DMLParameterIDDk_01GHz, ZStringConstants.DMLParameterIDDf_05GHz, ZStringConstants.DMLParameterIDDk_05GHz,
                ZStringConstants.DMLParameterIDDk_1GHz, ZStringConstants.DMLParameterIDDf_1GHz, ZStringConstants.DMLParameterIDDk_2GHz, ZStringConstants.DMLParameterIDDf_2GHz,
                ZStringConstants.DMLParameterIDDk_3GHz, ZStringConstants.DMLParameterIDDf_3GHz, ZStringConstants.DMLParameterIDDk_4GHz, ZStringConstants.DMLParameterIDDf_4GHz,
                ZStringConstants.DMLParameterIDDk_5GHz, ZStringConstants.DMLParameterIDDf_5GHz, ZStringConstants.DMLParameterIDDk_6GHz, ZStringConstants.DMLParameterIDDf_6GHz,
                ZStringConstants.DMLParameterIDDk_7GHz, ZStringConstants.DMLParameterIDDf_7GHz, ZStringConstants.DMLParameterIDDk_8GHz, ZStringConstants.DMLParameterIDDf_8GHz,
                ZStringConstants.DMLParameterIDDk_9GHz, ZStringConstants.DMLParameterIDDf_9GHz, ZStringConstants.DMLParameterIDDk_10GHz, ZStringConstants.DMLParameterIDDf_10GHz,
                ZStringConstants.DMLParameterIDDk_12GHz, ZStringConstants.DMLParameterIDDf_12GHz, ZStringConstants.DMLParameterIDDk_15GHz, ZStringConstants.DMLParameterIDDf_15GHz,
                ZStringConstants.DMLParameterIDDk_18GHz, ZStringConstants.DMLParameterIDDf_18GHz, ZStringConstants.DMLParameterIDDk_20GHz, ZStringConstants.DMLParameterIDDf_20GHz,
                ZStringConstants.DMLParameterIDDk_23GHz, ZStringConstants.DMLParameterIDDf_23GHz, ZStringConstants.DMLParameterIDDk_29GHz, ZStringConstants.DMLParameterIDDf_29GHz,
                ZStringConstants.DMLParameterIDDk_34GHz, ZStringConstants.DMLParameterIDDf_34GHz};

            foreach (ZParameter templateParameter in this.Parameters)
            {
                if (ignoreParameters.Contains(templateParameter.ID) || parameters.Find(x => x.ID == templateParameter.ID) != null) continue;

                ZParameter parameter = new ZParameter(templateParameter.ID);
                parameter.Title = templateParameter.Title;
                parameter.Description = templateParameter.Description;
                parameter.Table = ZTableType.Stackup;
                parameter.ValueType = templateParameter.ValueType;
                parameter.IsHidden = true;
                parameter.Width = 100;
                parameter.Order = ++order;
                parameter.List = templateParameter.List;
                parameter.Measure = templateParameter.Measure;
                parameter.DisplayFactor = templateParameter.DisplayFactor;
                parameter.DisplayMeasure = templateParameter.DisplayMeasure;
                parameter.Width = templateParameter.Width;
                parameter.WidthToExpand = templateParameter.WidthToExpand;
                parameter.IsCollapsed = templateParameter.IsCollapsed;
                parameter.DefaultValue = templateParameter.DefaultValue;
                parameters.Add(parameter);
            }
        }

        internal ZMaterial FinedMaterial(ZLibraryCategory[] libraryCetegories, MaterialSearchAttributes attributes)
        {
            
            List<ZMaterial> materials = Materials.FindAll(x =>
                attributes.LayerType == x.GetMaterialType() &&
                attributes.MaterialName == x.GetMaterialParameterValue(ZStringConstants.DMLParameterIDMaterial) &&
                attributes.Construction == x.GetMaterialParameterValue(ZStringConstants.DMLParameterIDNormalizedConstruction) &&
                ZPlannerManager.CompareAsDouble(attributes.Resin, x.GetMaterialParameterValue(ZStringConstants.DMLParameterIDResin), 0.01) &&
                ZPlannerManager.CompareAsTable(attributes.Dk, x.GetMaterialParameterValue(ZStringConstants.DMLParameterIDDk), 0.001) &&
                ZPlannerManager.CompareAsThickness(attributes.Thickness, x.GetMaterialParameterValue(ZStringConstants.DMLParameterIDH).TrimEnd(), 0.001, attributes.LayerType == ZLayerType.Core)/*&&
                ZPlannerManager.CompareAsDouble(attributes.Thickness, x.GetMaterialParameterValue(ZStringConstants.DMLParameterIDH), 0.1)*/);

            foreach (ZLibraryCategory category in libraryCetegories)
            {
                foreach (ZMaterial material in materials)
                {
                    if (material.GetMaterialCategoryString(category) == material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDCategory)) return material;
                }
            }

            return null;
        }
    }
}
