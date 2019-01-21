using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;
using Crypto;

namespace ZZero.ZPlanner.Data.Provider
{
    /// <summary>
    /// Represents an XML data provider.
    /// </summary>
    class XMLDataProvider:DataProvider
    {
        private bool isLegacyDkDf = false;

        /// <summary>
        /// Create XMLDataProvider object.
        /// </summary>
        internal XMLDataProvider()
        {            
        }

        /// <summary>
        /// Provides ZPlanner project data source.
        /// </summary>
        /// <param name="projectPath">Path to the project file.</param>
        internal override ZPlannerProject OpenZPlannerProject(string projectPath)
        {
            isLegacyDkDf = false;
            ZPlannerProject project = new ZPlannerProject();
            XElement root = XElement.Load(projectPath);

            // Fill FileInfo data
            SetFileInfo(root, project);

            // Fill SelectList collection
            SetSelectLists(root, project.Lists);
            project.AddDefaultSelectLists();

            // Fill Parameters collection
            SetParameters(root, project.Parameters, project.Lists);
            SetSubParameters(root, project.SubParameters, project.Lists, project.Parameters);
            project.AddDefaultParameters();

            // Set Stackup
            SetStackup(root, project, project.Lists, project.Parameters);

            return project;
        }

        /// <summary>
        /// Save ZPlanner project data source.
        /// </summary>
        /// <param name="projectPath">Path to the project file.</param>
        /// <param name="project">Project to be saved.</param>
        internal override void SaveZPlannerProject(string projectPath, ZPlannerProject project)
        {
            XElement root = new XElement("ZPlannerProject");

            root.Add(GetFileInfoElement(project));

            root.Add(GetSelectListsElement(project.Lists));

            root.Add(GetParametersElement(project.Parameters));

            root.Add(GetStackupElement(project.Stackup));

            root.Save(projectPath);
        }

        /// <summary>
        /// Provides DML data source.
        /// </summary>
        /// <param name="dmlPath">Path to DML file</param>
        internal override ZMaterialLibrary OpenZMaterialLibrary(string dmlPath, ZMaterialLibrary z0Library = null)
        {
            ZMaterialLibrary library = new ZMaterialLibrary();

            XElement root;
            string ext = Path.GetExtension(dmlPath);
            bool isEncrypted = ext == ".z0lib";
            if (isEncrypted)
            {
                string key = KeyHelper.GetKey();
                Encryption enc = new Encryption(key);
                string s = enc.Decrypt(dmlPath);
                root = XElement.Parse(s);
            }
            else
            {
                root = XElement.Load(dmlPath);
            }

            if (z0Library != null)
            {
                foreach (SelectList z0List in z0Library.Lists)
                    library.Lists.Add(z0List);
            
                foreach (ZParameter z0Parameter in z0Library.Parameters)
                    library.Parameters.Add(z0Parameter);
            }

            

            // Fill SelectList collection
            SetSelectLists(root, library.Lists);
            
            // Fill Parameters collection
            SetParameters(root, library.Parameters, library.Lists);

            // Add Empty Material
            if (ZPlannerManager.DmlIsEmpty) AddEmptyMaterial(library.Materials, library.Parameters);

            // Fill Materials collection
            SetMaterials(root, library.Materials, library.Parameters);

            return library;
        }

        /// <summary>
        /// Sets file info.
        /// </summary>
        /// <param name="root">Root element.</param>
        /// <param name="project">Project to keep the file info.</param>
        private void SetFileInfo(XElement root, ZPlannerProject project)
        {
            var query =
            from item in root.Descendants("FileInfo")
            select item;

            foreach (XElement infoElement in query)
            {
                project.Created = GetDateTimeValue(infoElement.Attribute("created"));
                project.Edited = GetDateTimeValue(infoElement.Attribute("edited"));
                project.Version = GetDoubleValue(infoElement.Attribute("version"));
                project.IsProtected = GetBoolValue(infoElement.Attribute("isProtected"));
            }
        }

        /// <summary>
        /// Gets file info.
        /// </summary>
        /// <param name="project">Project that keep file info.</param>
        /// <returns>XML ellement of file info.</returns>
        private XElement GetFileInfoElement(ZPlannerProject project)
        {
            XElement fileInfoElement = new XElement("FileInfo");
            AddDateTimeAttribute(fileInfoElement, "created", project.Created);
            AddDateTimeAttribute(fileInfoElement, "edited", DateTime.Now);
            AddDoubleAttribute(fileInfoElement, "version", ZPlannerManager.GetProductVersion()); //project.Version);
            if (project.IsProtected) AddBoolAttribute(fileInfoElement, "isProtected", project.IsProtected);
            return fileInfoElement;
        }

        /// <summary>
        /// Read SelectList collection from XML.
        /// </summary>
        /// <param name="root">Root element.</param>
        /// <param name="lists">Collection of SelectList.</param>
        private void SetSelectLists(XElement root, ZList<SelectList> lists)
        {
            // Fill SelectList collection
            var query =
            from item in root.Descendants("SelectItem")
            group item by item.Parent.Attribute("id").Value;

            foreach (var selectListGroup in query)
            {
                SelectList selectList = lists.Find(x => x.ID == selectListGroup.Key);
                if (selectList == null)
                {
                    selectList = new SelectList(selectListGroup.Key);
                    lists.Add(selectList);
                }

                foreach (string selectItem in selectListGroup)
                {
                    if (!selectList.GetValues().Contains(selectItem)) selectList.AddValue(selectItem);
                }                
            }
        }

        /// <summary>
        /// Create SelectList element.
        /// </summary>
        /// <param name="selectLists">SelectList collectiuon.</param>
        /// <returns>XML element of SelectList collection</returns>
        private XElement GetSelectListsElement(ZList<SelectList> selectLists)
        {
            XElement selectListsElement = new XElement("SelectLists");

            foreach (SelectList selectList in selectLists)
            {
                XElement selectListElement = new XElement("SelectList", new XAttribute("id", selectList.ID));

                foreach (string selectItem in selectList.GetValues())
                {
                    XElement selectItemElement = new XElement("SelectItem", selectItem);
                    selectListElement.Add(selectItemElement);
                }

                selectListsElement.Add(selectListElement);
            }

            return selectListsElement;
        }

        /// <summary>
        /// Read Parameter collection.
        /// </summary>
        /// <param name="root">Root element.</param>
        /// <param name="parameters">ZParameter collection</param>
        /// <param name="selectLists">SelectList collection</param>
        private void SetParameters(XElement root, ZList<ZParameter> parameters, ZList<SelectList> selectLists)
        {
            // Fill Parameters collection
            var query =
            from item in root.Descendants("Parameter")
            select item;

            int index = parameters.Count;
            foreach (XElement parameterElement in query)
            {
                string parameterId = GetTextValue(parameterElement.Attribute("id"));
                if (parameters.Find(x => x.ID == parameterId) != null) continue;

                ZParameter parameter = new ZParameter(parameterId);
                parameter.Title = GetTextValue(parameterElement.Attribute("title"));
                parameter.Description = GetTextValue(parameterElement.Attribute("description"));
                parameter.ValueType = GetEnumValue<ZValueType>(parameterElement.Attribute("type"));
                parameter.Measure = GetEnumValue<ZMeasures>(parameterElement.Attribute("measure"));
                parameter.DisplayMeasure = GetEnumValue<ZMeasures>(parameterElement.Attribute("displayMeasure"));
                parameter.DisplayFactor = GetDoubleValue(parameterElement.Attribute("displayFactor"));
                parameter.DefaultValue = GetTextValue(parameterElement.Attribute("defaultValue"));
                parameter.IsCustom = GetBoolValue(parameterElement.Attribute("isCustom"));
                parameter.IsHidden = GetBoolValue(parameterElement.Attribute("isHidden"));
                parameter.IsPrivate = GetBoolValue(parameterElement.Attribute("isPrivate"));
                parameter.IsCollapsed = GetBoolValue(parameterElement.Attribute("isCollapsed"));
                parameter.IsReadOnly = GetBoolValue(parameterElement.Attribute("isReadOnly")) || parameter.ID == ZStringConstants.DMLParameterIDCategory;
                parameter.IsNotEmpty = GetBoolValue(parameterElement.Attribute("isNotEmpty"));
                parameter.IsFrozen = GetBoolValue(parameterElement.Attribute("isFrozen"));
                parameter.List = GetListValue(parameterElement.Attribute("list"), selectLists);
                parameter.Table = GetEnumValue<ZTableType>(parameterElement.Attribute("table"));
                parameter.Width = (parameter.IsCollapsed) ? ZConstants.WidthToCollapse : GetIntValue(parameterElement.Attribute("width"));
                parameter.WidthToExpand = GetIntValue(parameterElement.Attribute("width"));
                parameter.ParameterGroup = GetTextValue(parameterElement.Attribute("group"));
                parameter.Order = index;

                parameters.Add(parameter);
                ++index;
            }
        }

        private void SetSubParameters(XElement root, ZList<ZParameter> subParameters, ZList<SelectList> selectLists, ZList<ZParameter> parameters)
        {
            // Fill SubParameters collection
            var query =
            from item in root.Descendants("SubParameter")
            group item by item.Parent.Attribute("id").Value into itemsGroup
            select new { Key = itemsGroup.Key, SubParameters = itemsGroup };

            int index;
            foreach (var parameterGroup in query)
            {
                ZParameter parameter = GetItemByID(parameters, parameterGroup.Key);
                parameter.SubParameters = new ZList<ZParameter>(parameter);
                index = parameter.Order;

                foreach (XElement subParameterElement in parameterGroup.SubParameters)
                {
                    string id = GetTextValue(subParameterElement.Attribute("id"));
                    // Backward Compatebility
                    if (id == ZStringConstants.ParameterIDDielectricConstant) { id = ZStringConstants.ParameterIDZo_DielectricConstant; isLegacyDkDf = true; }
                    if (id == ZStringConstants.ParameterIDLossTangent) { id = ZStringConstants.ParameterIDZo_LossTangent; isLegacyDkDf = true; }
                    if (parameter.SubParameters.Find(x => x.ID == id) != null) continue;

                    ZParameter subParameter = new ZParameter(id);
                    subParameter.Title = GetTextValue(subParameterElement.Attribute("title"));
                    subParameter.Description = GetTextValue(subParameterElement.Attribute("description"));
                    subParameter.ValueType = GetEnumValue<ZValueType>(subParameterElement.Attribute("type"));
                    subParameter.Measure = GetEnumValue<ZMeasures>(subParameterElement.Attribute("measure"));
                    subParameter.DisplayMeasure = GetEnumValue<ZMeasures>(subParameterElement.Attribute("displayMeasure"));
                    subParameter.DisplayFactor = GetDoubleValue(subParameterElement.Attribute("displayFactor"));
                    subParameter.DefaultValue = GetTextValue(subParameterElement.Attribute("defaultValue"));
                    subParameter.IsCustom = GetBoolValue(subParameterElement.Attribute("isCustom"));
                    subParameter.IsHidden = GetBoolValue(subParameterElement.Attribute("isHidden"));
                    subParameter.IsPrivate = GetBoolValue(subParameterElement.Attribute("isPrivate"));
                    subParameter.IsCollapsed = GetBoolValue(subParameterElement.Attribute("isCollapsed"));
                    subParameter.IsReadOnly = GetBoolValue(subParameterElement.Attribute("isReadOnly"));
                    subParameter.IsNotEmpty = GetBoolValue(subParameterElement.Attribute("isNotEmpty"));
                    subParameter.IsFrozen = GetBoolValue(subParameterElement.Attribute("isFrozen"));
                    subParameter.Table = parameter.Table;
                    subParameter.List = GetListValue(subParameterElement.Attribute("list"), selectLists);
                    subParameter.Width = (subParameter.IsCollapsed) ? ZConstants.WidthToCollapse : GetIntValue(subParameterElement.Attribute("width"));
                    subParameter.WidthToExpand = GetIntValue(subParameterElement.Attribute("width"));
                    subParameter.ParameterGroup = GetTextValue(subParameterElement.Attribute("group"));
                    subParameter.Order = index;

                    subParameters.Add(subParameter);
                    parameter.SubParameters.Add(subParameter);
                    if (index > parameter.Order)
                        ZPlannerProject.ShiftParametersOrder(parameters, index);
                    ++index;
                }
            }
        }

        private XElement GetParametersElement(ZList<ZParameter> parameters)
        {
            XElement parametersElement = new XElement("Parameters");

            foreach(ZParameter parameter in parameters.OrderBy(X => X.Order))
            {
                XElement parameterElement = new XElement("Parameter", new XAttribute("id", parameter.ID));
                AddTextAttribute(parameterElement, "title", parameter.Title);
                AddTextAttribute(parameterElement, "description", parameter.Description);
                AddEnumAttribute(parameterElement, "type", parameter.ValueType);
                AddEnumAttribute(parameterElement, "measure", parameter.Measure);
                AddEnumAttribute(parameterElement, "displayMeasure", parameter.DisplayMeasure);
                AddDoubleAttribute(parameterElement, "displayFactor", parameter.DisplayFactor);
                AddTextAttribute(parameterElement, "defaultValue", parameter.DefaultValue);
                AddBoolAttribute(parameterElement, "isCustom", parameter.IsCustom);
                AddBoolAttribute(parameterElement, "isHidden", parameter.IsHidden);
                AddBoolAttribute(parameterElement, "isPrivate", parameter.IsPrivate);
                AddBoolAttribute(parameterElement, "isCollapsed", parameter.IsCollapsed);
                AddBoolAttribute(parameterElement, "isReadOnly", parameter.IsReadOnly);
                AddBoolAttribute(parameterElement, "isNotEmpty", parameter.IsNotEmpty);
                AddBoolAttribute(parameterElement, "isFrozen", parameter.IsFrozen);
                AddListAttribute(parameterElement, "list", parameter.List);
                AddEnumAttribute(parameterElement, "table", parameter.Table);
                AddTextAttribute(parameterElement, "group", parameter.ParameterGroup);
                if (parameter.IsCollapsed) AddIntAttribute(parameterElement, "width", parameter.WidthToExpand);
                else AddIntAttribute(parameterElement, "width", parameter.Width);

                if (parameter.SubParameters != null)
                    foreach (ZParameter subParameter in parameter.SubParameters.OrderBy(X => X.Order)) 
                    {
                        XElement subParameterElement = new XElement("SubParameter", new XAttribute("id", subParameter.ID));
                        AddTextAttribute(subParameterElement, "title", subParameter.Title);
                        AddTextAttribute(subParameterElement, "description", subParameter.Description);
                        AddEnumAttribute(subParameterElement, "type", subParameter.ValueType);
                        AddEnumAttribute(subParameterElement, "measure", subParameter.Measure);
                        AddEnumAttribute(subParameterElement, "displayMeasure", subParameter.DisplayMeasure);
                        AddDoubleAttribute(subParameterElement, "displayFactor", subParameter.DisplayFactor);
                        AddTextAttribute(subParameterElement, "defaultValue", subParameter.DefaultValue);
                        AddBoolAttribute(subParameterElement, "isCustom", subParameter.IsCustom);
                        AddBoolAttribute(subParameterElement, "isHidden", subParameter.IsHidden);
                        AddBoolAttribute(subParameterElement, "isPrivate", subParameter.IsPrivate);
                        AddBoolAttribute(subParameterElement, "isCollapsed", subParameter.IsCollapsed);
                        AddBoolAttribute(subParameterElement, "isReadOnly", subParameter.IsReadOnly);
                        AddBoolAttribute(subParameterElement, "isNotEmpty", subParameter.IsNotEmpty);
                        AddBoolAttribute(subParameterElement, "isFrozen", subParameter.IsFrozen);
                        AddListAttribute(subParameterElement, "list", subParameter.List);
                        AddTextAttribute(subParameterElement, "group", subParameter.ParameterGroup);
                        if (subParameter.IsCollapsed) AddIntAttribute(subParameterElement, "width", subParameter.WidthToExpand);
                        else AddIntAttribute(subParameterElement, "width", subParameter.Width);

                        parameterElement.Add(subParameterElement);
                    }

                parametersElement.Add(parameterElement);
            }

            return parametersElement;
        }

        private void SetStackup(XElement root, ZPlannerProject project, ZList<SelectList> selectLists, ZList<ZParameter> parameters)
        {
            // Fill Stackup collection
            var query =
            from item in root.Descendants("Stackup")
            select item;

            foreach (var stackupElement in query)
            {
                ZStackup stackup = new ZStackup(project, GetTextValue(stackupElement.Attribute("id")));
                string title = GetTextValue(stackupElement.Attribute("title"));
                stackup.Title = (!string.IsNullOrWhiteSpace(title)) ? title : (!string.IsNullOrWhiteSpace(ZPlannerManager.ProjectFile)) ? Path.GetFileNameWithoutExtension(ZPlannerManager.ProjectFile) : "UNSAVED";
                double frequency = GetDoubleValue(stackupElement.Attribute("frequency"));
                if (!double.IsNaN(frequency)) stackup.Frequency = frequency;

                // Roughness
                double outerLayerRoughness = GetDoubleValue(stackupElement.Attribute("outerRoughness"));
                if (!double.IsNaN(outerLayerRoughness)) stackup.OuterLayerRoughness = outerLayerRoughness;

                double prepregSideRoughness = GetDoubleValue(stackupElement.Attribute("prepregRoughness"));
                if (!double.IsNaN(prepregSideRoughness)) stackup.PrepregSideRoughness = prepregSideRoughness;

                double laminateSideRoughness = GetDoubleValue(stackupElement.Attribute("laminateRoughness"));
                if (!double.IsNaN(laminateSideRoughness)) stackup.LaminateSideRoughness = laminateSideRoughness;

                // Pressed Thickness
                XAttribute pressedByCopperCoverageAttribute = stackupElement.Attribute("copperCoverageType");
                if (pressedByCopperCoverageAttribute != null) stackup.CopperCoverageType = GetEnumValue<ZCopperCoverageType>(pressedByCopperCoverageAttribute);
                //else stackup.CopperCoverageType = ZCopperCoverageType.ManuallyEntered;

                double forSignal = GetDoubleValue(stackupElement.Attribute("pressedForSignal"));
                if (!double.IsNaN(forSignal)) stackup.ForSignal = forSignal;

                double forMixed = GetDoubleValue(stackupElement.Attribute("pressedForMixed"));
                if (!double.IsNaN(forMixed)) stackup.ForMixed = forMixed;

                double forPlane = GetDoubleValue(stackupElement.Attribute("pressedForPlane"));
                if (!double.IsNaN(forPlane)) stackup.ForPlane = forPlane;

                double forSignalSignal = GetDoubleValue(stackupElement.Attribute("pressedForSignalSignal"));
                if (!double.IsNaN(forSignalSignal)) stackup.ForSignalSignal = forSignalSignal;

                double forSignalMixed = GetDoubleValue(stackupElement.Attribute("pressedForSignalMixed"));
                if (!double.IsNaN(forSignalMixed)) stackup.ForSignalMixed = forSignalMixed;

                double forSignalPlane = GetDoubleValue(stackupElement.Attribute("pressedForSignalPlane"));
                if (!double.IsNaN(forSignalPlane)) stackup.ForSignalPlane = forSignalPlane;

                double forMixedMixed = GetDoubleValue(stackupElement.Attribute("pressedForMixedMixed"));
                if (!double.IsNaN(forMixedMixed)) stackup.ForMixedMixed = forMixedMixed;

                double forMixedPlane = GetDoubleValue(stackupElement.Attribute("pressedForMixedPlane"));
                if (!double.IsNaN(forMixedPlane)) stackup.ForMixedPlane = forMixedPlane;

                double forPlanePlane = GetDoubleValue(stackupElement.Attribute("pressedForPlanePlane"));
                if (!double.IsNaN(forPlanePlane)) stackup.ForPlanePlane = forPlanePlane;

                double etch = GetDoubleValue(stackupElement.Attribute("etchFactor"));
                if (!double.IsNaN(etch)) stackup.Etch = etch;

                stackup.IsRoughness = GetBoolValue(stackupElement.Attribute("isRoughnessEnabled"));
                stackup.IsPressedThickness = GetBoolValue(stackupElement.Attribute("isPressedThicknessEnabled"));
                stackup.IsSequentialLamination = GetBoolValue(stackupElement.Attribute("isSequentialLaminationEnabled"));
                stackup.IsKeepImportedPressedThickness = GetBoolValue(stackupElement.Attribute("isKeepImportedPressedThicknessEnabled"));
                stackup.IsTrapezoidalTraces = GetBoolValue(stackupElement.Attribute("isTrapezoidalTracesEnabled"));
                stackup.IsLossPlanning = GetBoolValue(stackupElement.Attribute("isLossPlanningEnabled"));
                stackup.IsGlassPitch = GetBoolValue(stackupElement.Attribute("isGlassPitchEnabled"));
                stackup.IsGws = GetBoolValue(stackupElement.Attribute("isGwsEnabled"));
                stackup.IsCenterLineVisible = GetBoolValue(stackupElement.Attribute("isCenterLineEnabled"));
                stackup.IsCoreLocked = GetBoolValue(stackupElement.Attribute("isCoreLockedEnabled"));
                stackup.IsAutoMirror = GetBoolValue(stackupElement.Attribute("isAutoMirrorEnabled"));
                stackup.IsHeadersVisible = GetBoolValue(stackupElement.Attribute("isHeadersEnabled"));
                stackup.IsColorDisabledCells = GetBoolValue(stackupElement.Attribute("isDisabledCellsEnabled"));
                stackup.IsFileOpened = project.Version >= 1.1;

                project.Stackup = stackup;
                break;
            }

            SetLayers(root, project.Stackup, parameters);
            SetSpans(root, project.Stackup);
            SetSingles(root, project.Stackup, parameters);
            SetPairs(root, project.Stackup, parameters);

            if (project.Stackup.Singles.Count == 0) project.Stackup.Singles.Add(new ZSingle(project.Stackup));
            if (project.Stackup.Pairs.Count == 0) project.Stackup.Pairs.Add(new ZPair(project.Stackup));
        }

        private XElement GetStackupElement(ZStackup stackup)
        {
            XElement stackupElement = new XElement("Stackup", new XAttribute("id", stackup.ID), new XAttribute("title", stackup.Title), new XAttribute("frequency", stackup.Frequency),
                new XAttribute("isRoughnessEnabled", ZPlannerManager.IsRoughness), new XAttribute("isPressedThicknessEnabled", ZPlannerManager.IsPressedThickness), new XAttribute("isSequentialLaminationEnabled", ZPlannerManager.IsSequentialLamination),
                new XAttribute("isKeepImportedPressedThicknessEnabled", stackup.IsKeepImportedPressedThickness), new XAttribute("isTrapezoidalTracesEnabled", ZPlannerManager.IsTrapezoidalTraces),
                new XAttribute("isLossPlanningEnabled", ZPlannerManager.IsLossPlanning), new XAttribute("isGlassPitchEnabled", ZPlannerManager.IsGlassPitch), new XAttribute("isGwsEnabled", ZPlannerManager.IsGws),
                new XAttribute("isCenterLineEnabled", ZPlannerManager.IsCenterLineVisible), new XAttribute("isCoreLockedEnabled", ZPlannerManager.IsCoreLocked), new XAttribute("isAutoMirrorEnabled", ZPlannerManager.IsAutoMirror),
                new XAttribute("isHeadersEnabled", ZPlannerManager.IsHeadersVisible), new XAttribute("isDisabledCellsEnabled", ZPlannerManager.IsColorDisabledCells),
                new XAttribute("outerRoughness", stackup.OuterLayerRoughness), new XAttribute("prepregRoughness", stackup.PrepregSideRoughness), new XAttribute("laminateRoughness", stackup.LaminateSideRoughness),
                new XAttribute("copperCoverageType", stackup.CopperCoverageType.ToString()), new XAttribute("pressedForSignal", stackup.ForSignal), new XAttribute("pressedForMixed", stackup.ForMixed), new XAttribute("pressedForPlane", stackup.ForPlane),
                new XAttribute("pressedForSignalSignal", stackup.ForSignalSignal), new XAttribute("pressedForSignalMixed", stackup.ForSignalMixed), new XAttribute("pressedForSignalPlane", stackup.ForSignalPlane),
                new XAttribute("pressedForMixedMixed", stackup.ForMixedMixed), new XAttribute("pressedForMixedPlane", stackup.ForMixedPlane), new XAttribute("pressedForPlanePlane", stackup.ForPlanePlane),
                new XAttribute("etchFactor", stackup.Etch));
            stackupElement.Add(GetViaSpanElement(stackup.Spans));
            stackupElement.Add(GetLayersElement(stackup.Layers));
            stackupElement.Add(GetImpedanceElement(stackup.Singles, stackup.Pairs));

            return stackupElement;
        }

        private XElement GetViaSpanElement(ZList<ZSpan> spans)
        {
            XElement spansElement = new XElement("ViaSpan");

            foreach (ZSpan span in spans)
            {
                XElement spanElement = new XElement("Span", new XAttribute("id", span.ID));
                AddTextAttribute(spanElement, "title", span.Title);
                AddDoubleAttribute(spanElement, "size", span.Size);
                AddTextAttribute(spanElement, "firstLayerID", span.FirstLayer.ID);
                AddTextAttribute(spanElement, "lastLayerID", span.LastLayer.ID);
                AddDoubleAttribute(spanElement, "aspectRatio", span.AspectRatio);
                AddBoolAttribute(spanElement, "isEnable", span.IsEnable);
                AddTextAttribute(spanElement, "filled", span.Filled);
                AddBoolAttribute(spanElement, "backDrilled", span.IsBackDrilled);

                spansElement.Add(spanElement);
            }

            return spansElement;
        }

        private XElement GetLayersElement(ZList<ZLayer> layers)
        {
            XElement layersElement = new XElement("Layers");

            foreach (ZLayer layer in layers)
            {
                XElement layerElement = new XElement("Layer", new XAttribute("id", layer.ID));
                AddTextAttribute(layerElement, "title", layer.Title);
                AddBoolAttribute(layerElement, "isCustom", layer.IsCustom);
                AddBoolAttribute(layerElement, "isHidden", layer.IsHidden);
                AddBoolAttribute(layerElement, "isEdited", false); //layer.IsEdited);
                AddBoolAttribute(layerElement, "isReadOnly", layer.IsReadOnly);
                AddBoolAttribute(layerElement, "isMaterialAssigned", layer.IsMaterialAssigned);
                AddBoolAttribute(layerElement, "isFrozen", layer.IsFrozen);
                AddIntAttribute(layerElement, "height", layer.Height);

                foreach(ZLayerParameter layerParameter in layer.LayerParameters)
                {
                    XElement layerParameterElement = new XElement("LayerParameter", new XAttribute("parameterId", layerParameter.Parameter.ID), new XAttribute("value", layerParameter.Value));
                    //AddTextAttribute(layerParameterElement, "id", layerParameter.ID);
                    AddTextAttribute(layerParameterElement, "title", layerParameter.Title);
                    AddBoolAttribute(layerParameterElement, "isEdited", layerParameter.IsEdited); // false);
                    
                    layerElement.Add(layerParameterElement);
                }

                layersElement.Add(layerElement);
            }

            return layersElement;
        }

        private XElement GetImpedanceElement(ZList<ZSingle> singles, ZList<ZPair> pairs)
        {
            XElement impedanceElement = new XElement("Impedance");

            // Singles.
            foreach (ZSingle single in singles)
            {
                XElement singleElement = new XElement("Single", new XAttribute("id", single.ID), new XAttribute("title", single.Title));

                foreach (ZLayer layer in single.Layers)
                {
                    XElement layerElement = new XElement("Layer", new XAttribute("id", layer.ID));
                    AddTextAttribute(layerElement, "title", layer.Title);
                    AddBoolAttribute(layerElement, "isCustom", layer.IsCustom);
                    AddBoolAttribute(layerElement, "isHidden", layer.IsHidden);
                    AddBoolAttribute(layerElement, "isEdited", false); //layer.IsEdited);
                    AddBoolAttribute(layerElement, "isReadOnly", layer.IsReadOnly);
                    AddBoolAttribute(layerElement, "isFrozen", layer.IsFrozen);
                    AddIntAttribute(layerElement, "height", layer.Height);

                    foreach (ZLayerParameter layerParameter in layer.LayerParameters)
                    {
                        if (layerParameter.Value == null) layerParameter.Value = string.Empty;
                        XElement layerParameterElement = new XElement("LayerParameter", new XAttribute("parameterId", layerParameter.Parameter.ID), new XAttribute("value", layerParameter.Value));
                        //AddTextAttribute(layerParameterElement, "id", layerParameter.ID);
                        AddTextAttribute(layerParameterElement, "title", layerParameter.Title);
                        AddBoolAttribute(layerParameterElement, "isEdited", layerParameter.IsEdited); // false);

                        layerElement.Add(layerParameterElement);
                    }

                    singleElement.Add(layerElement);
                }

                impedanceElement.Add(singleElement);
            }

            // Pairs.
            foreach (ZPair pair in pairs)
            {
                XElement pairElement = new XElement("Pair", new XAttribute("id", pair.ID), new XAttribute("title", pair.Title));

                foreach (ZLayer layer in pair.Layers)
                {
                    XElement layerElement = new XElement("Layer", new XAttribute("id", layer.ID));
                    AddTextAttribute(layerElement, "title", layer.Title);
                    AddBoolAttribute(layerElement, "isCustom", layer.IsCustom);
                    AddBoolAttribute(layerElement, "isHidden", layer.IsHidden);
                    AddBoolAttribute(layerElement, "isEdited", false); //layer.IsEdited);
                    AddBoolAttribute(layerElement, "isReadOnly", layer.IsReadOnly);
                    AddBoolAttribute(layerElement, "isFrozen", layer.IsFrozen);
                    AddIntAttribute(layerElement, "height", layer.Height);

                    foreach (ZLayerParameter layerParameter in layer.LayerParameters)
                    {
                        if (layerParameter.Value == null) layerParameter.Value = string.Empty;
                        XElement layerParameterElement = new XElement("LayerParameter", new XAttribute("parameterId", layerParameter.Parameter.ID), new XAttribute("value", layerParameter.Value));
                        //AddTextAttribute(layerParameterElement, "id", layerParameter.ID);
                        AddTextAttribute(layerParameterElement, "title", layerParameter.Title);
                        AddBoolAttribute(layerParameterElement, "isEdited", layerParameter.IsEdited); // false);

                        layerElement.Add(layerParameterElement);
                    }

                    pairElement.Add(layerElement);
                }

                impedanceElement.Add(pairElement);
            }

            return impedanceElement;
        }

        private void SetSpans(XElement root, ZStackup stackup)
        {
            // Fill Spans collection
            var query =
            from item in root.Descendants("Span")
            group item by item.Parent.Parent.Attribute("id").Value into itemsGroup
            select new { Key = itemsGroup.Key, Spans = itemsGroup };

            foreach (var stackupGroup in query)
            {
                if (stackup.ID != stackupGroup.Key) continue;

                foreach (XElement spanElement in stackupGroup.Spans)
                {
                    ZSpan span = new ZSpan(GetTextValue(spanElement.Attribute("id")), stackup);
                    span.Title = GetTextValue(spanElement.Attribute("title"));
                    span.Size = GetDoubleValue(spanElement.Attribute("size"));
                    string firstLayerID = GetTextValue(spanElement.Attribute("firstLayerID"));
                    string lastLayerID = GetTextValue(spanElement.Attribute("lastLayerID"));
                    span.AspectRatio = GetDoubleValue(spanElement.Attribute("aspectRatio"));
                    span.IsEnable = GetBoolValue(spanElement.Attribute("isEnable"));
                    span.Filled = GetTextValue(spanElement.Attribute("filled"));
                    span.IsBackDrilled = GetBoolValue(spanElement.Attribute("backDrilled"));

                    span.FirstLayer = stackup.Layers.Find((ZLayer x) => x.ID == firstLayerID);
                    span.LastLayer = stackup.Layers.Find((ZLayer x) => x.ID == lastLayerID);

                    stackup.Spans.Add(span);
                }
            }
        }

        private void SetSingles(XElement root, ZStackup stackup, ZList<ZParameter> parameters)
        {
            List<ZParameter> singleSubparameters = new List<ZParameter>();
            foreach (ZParameter singleParameter in parameters.FindAll(x => x.Table == ZTableType.Single))
            {
                if (singleParameter.SubParameters != null && singleParameter.SubParameters.Count > 0) singleSubparameters.AddRange(singleParameter.SubParameters);
                else singleSubparameters.Add(singleParameter);
            }

            // Fill Singles collection
            var query =
            from item in root.Descendants("LayerParameter")
            where item.Parent.Parent.Name == "Single"
            group item by item.Parent.Parent.Parent.Parent.Attribute("id").Value into stackupGroup
            select new
            {
                Key = stackupGroup.Key,
                Singles =
                    from item in stackupGroup
                    group item by item.Parent.Parent.Attribute("id").Value into singleGroup
                    select new
                    {
                        Key = singleGroup.Key,
                        Single = singleGroup.FirstOrDefault().Parent.Parent,
                        Layers =
                            from item in singleGroup
                            group item by item.Parent.Attribute("id").Value into layerGroup
                            select new { Key = layerGroup.Key, Layer = singleGroup.FirstOrDefault().Parent, LayerParameters = layerGroup }
                    }
            };

            foreach (var stackupGroup in query)
            {
                if (stackup.ID != stackupGroup.Key) continue;

                foreach (var singleGroup in stackupGroup.Singles)
                {
                    ZSingle single = new ZSingle(singleGroup.Key, stackup);
                    single.Title = GetTextValue(singleGroup.Single.Attribute("title"));

                    foreach (ZLayer stackupLayer in stackup.Layers)
                    {
                        ZLayer layer = new ZLayer(stackupLayer.ID, stackup, single);
                        layer.IsFrozen = stackupLayer.IsFrozen;
                        layer.IsCustom = stackupLayer.IsCustom;
                        layer.IsEdited = stackupLayer.IsEdited;
                        layer.IsHidden = stackupLayer.IsHidden;
                        layer.IsReadOnly = stackupLayer.IsReadOnly;
                        layer.IsMaterialAssigned = stackupLayer.IsMaterialAssigned;
                        layer.Height = stackupLayer.Height;


                        foreach (ZParameter parameter in singleSubparameters)
                        {
                            ZLayerParameter layerParameter = new ZLayerParameter(parameter.ID);
                            layerParameter.Value = string.Empty;
                            layerParameter.Layer = layer;
                            layerParameter.Parameter = parameter;

                            layer.LayerParameters.Add(layerParameter);
                        }

                        single.Layers.Add(layer);
                    }

                    foreach (var layerGroup in singleGroup.Layers)
                    {
                        ZLayer layer = single.Layers.Find(x => x.ID == layerGroup.Key);
                        if (layer == null) continue;
                        layer.IsFrozen = GetBoolValue(layerGroup.Layer.Attribute("isFrozen"));
                        layer.IsCustom = GetBoolValue(layerGroup.Layer.Attribute("isCustom"));
                        layer.IsEdited = GetBoolValue(layerGroup.Layer.Attribute("isEdited"));
                        layer.IsHidden = GetBoolValue(layerGroup.Layer.Attribute("isHidden"));
                        layer.IsReadOnly = GetBoolValue(layerGroup.Layer.Attribute("isReadOnly"));
                        layer.Height = GetIntValue(layerGroup.Layer.Attribute("height"));

                        List<string> alreadySetLayerParameters = new List<string>();

                        foreach (XElement layerParameterElement in layerGroup.LayerParameters)
                        {
                            string parameterId = GetTextValue(layerParameterElement.Attribute("parameterId"));
                            // Backward Compatebility
                            if (isLegacyDkDf && (parameterId == ZStringConstants.ParameterIDZo_DielectricConstant || parameterId == ZStringConstants.ParameterIDZo_LossTangent)) continue;
                            ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterId);
                            if (layerParameter == null)
                            {
                                // Backward Compatebility
                                if (parameterId == ZStringConstants.ParameterIDDielectricConstant) parameterId = ZStringConstants.ParameterIDZo_DielectricConstant;
                                if (parameterId == ZStringConstants.ParameterIDLossTangent) parameterId = ZStringConstants.ParameterIDZo_LossTangent;
                                layerParameter = layer.LayerParameters.Find(x => x.ID == parameterId);
                                if (layerParameter == null) continue;
                            }

                            layerParameter.Value = GetCellValue(layerParameterElement.Attribute("value"));
                            layerParameter.IsEdited = GetBoolValue(layerParameterElement.Attribute("isEdited"));
                            alreadySetLayerParameters.Add(layerParameter.ID);
                        }

                        foreach (ZLayerParameter layerParameter in layer.LayerParameters)
                        {
                            if (!alreadySetLayerParameters.Contains(layerParameter.ID))
                            {
                                ZLayerType? type = layer.GetLayerType();
                                if (type != null)
                                {
                                    layerParameter.Value = layerParameter.GetDefaulLayerParameterValue((ZLayerType)type);
                                }
                            }

                            if (layerParameter.Parameter.ValueType == ZValueType.Boolean && string.IsNullOrWhiteSpace(layerParameter.Value)) layerParameter.Value = "false";
                        }
                    }

                    stackup.Singles.Add(single);
                }
            }
        }

        private void SetPairs(XElement root, ZStackup stackup, ZList<ZParameter> parameters)
        {
            List<ZParameter> pairSubparameters = new List<ZParameter>();
            foreach (ZParameter pairParameter in parameters.FindAll(x => x.Table == ZTableType.Pair))
            {
                if (pairParameter.SubParameters != null && pairParameter.SubParameters.Count > 0) pairSubparameters.AddRange(pairParameter.SubParameters);
                else pairSubparameters.Add(pairParameter);
            }

            // Fill Pairs collection
            var query =
            from item in root.Descendants("LayerParameter")
            where item.Parent.Parent.Name == "Pair"
            group item by item.Parent.Parent.Parent.Parent.Attribute("id").Value into stackupGroup
            select new
            {
                Key = stackupGroup.Key,
                Pairs =
                    from item in stackupGroup
                    group item by item.Parent.Parent.Attribute("id").Value into pairGroup
                    select new { Key = pairGroup.Key, Pair = pairGroup.FirstOrDefault().Parent.Parent, Layers =
                        from item in pairGroup
                        group item by item.Parent.Attribute("id").Value into layerGroup
                        select new { Key = layerGroup.Key, Layer = pairGroup.FirstOrDefault().Parent, LayerParameters = layerGroup }
                    }
            };

            foreach (var stackupGroup in query)
            {
                if (stackup.ID != stackupGroup.Key) continue;

                foreach (var pairGroup in stackupGroup.Pairs)
                {
                    ZPair pair = new ZPair(pairGroup.Key, stackup);
                    pair.Title = GetTextValue(pairGroup.Pair.Attribute("title"));

                    foreach (ZLayer stackupLayer in stackup.Layers)
                    {
                        ZLayer layer = new ZLayer(stackupLayer.ID, stackup, pair);
                        layer.IsFrozen = stackupLayer.IsFrozen;
                        layer.IsCustom = stackupLayer.IsCustom;
                        layer.IsEdited = stackupLayer.IsEdited;
                        layer.IsHidden = stackupLayer.IsHidden;
                        layer.IsReadOnly = stackupLayer.IsReadOnly;
                        layer.IsMaterialAssigned = stackupLayer.IsMaterialAssigned;
                        layer.Height = stackupLayer.Height;

                        foreach (ZParameter parameter in pairSubparameters)
                        {
                            ZLayerParameter layerParameter = new ZLayerParameter(parameter.ID);
                            layerParameter.Value = string.Empty;
                            layerParameter.Layer = layer;
                            layerParameter.Parameter = parameter;

                            layer.LayerParameters.Add(layerParameter);
                        }

                        pair.Layers.Add(layer);
                    }

                    foreach(var layerGroup in pairGroup.Layers)
                    {
                        ZLayer layer = pair.Layers.Find(x => x.ID == layerGroup.Key);
                        if (layer == null) continue;
                        layer.IsFrozen = GetBoolValue(layerGroup.Layer.Attribute("isFrozen"));
                        layer.IsCustom = GetBoolValue(layerGroup.Layer.Attribute("isCustom"));
                        layer.IsEdited = GetBoolValue(layerGroup.Layer.Attribute("isEdited"));
                        layer.IsHidden = GetBoolValue(layerGroup.Layer.Attribute("isHidden"));
                        layer.IsReadOnly = GetBoolValue(layerGroup.Layer.Attribute("isReadOnly"));
                        layer.Height = GetIntValue(layerGroup.Layer.Attribute("height"));

                        List<string> alreadySetLayerParameters = new List<string>();

                        foreach (XElement layerParameterElement in layerGroup.LayerParameters)
                        {
                            string parameterId = GetTextValue(layerParameterElement.Attribute("parameterId"));
                            ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterId);
                            if (layerParameter == null) continue;

                            layerParameter.Value = GetCellValue(layerParameterElement.Attribute("value"));
                            layerParameter.IsEdited = GetBoolValue(layerParameterElement.Attribute("isEdited"));
                            alreadySetLayerParameters.Add(layerParameter.ID);
                        }

                        foreach (ZLayerParameter layerParameter in layer.LayerParameters)
                        {
                            if (!alreadySetLayerParameters.Contains(layerParameter.ID))
                            {
                                ZLayerType? type = layer.GetLayerType();
                                if (type != null)
                                {
                                    layerParameter.Value = layerParameter.GetDefaulLayerParameterValue((ZLayerType)type);
                                }
                            }

                            if (layerParameter.Parameter.ValueType == ZValueType.Boolean && string.IsNullOrWhiteSpace(layerParameter.Value)) layerParameter.Value = "false";
                        }
                    }

                    stackup.Pairs.Add(pair);
                }
            }
        }

        private void SetLayers(XElement root, ZStackup stackup, ZList<ZParameter> parameters)
        {
            List<ZParameter> stackupSubparameters = new List<ZParameter>();
            foreach (ZParameter parameter in parameters)
            {
                if (parameter.Table != ZTableType.Stackup) continue;
                if (parameter.SubParameters != null && parameter.SubParameters.Count > 0) stackupSubparameters.AddRange(parameter.SubParameters);
                else stackupSubparameters.Add(parameter);
            }

            // Fill Layers collection
            var query =
            from item in root.Descendants("LayerParameter")
            where item.Parent.Parent.Name == "Layers"
            group item by item.Parent.Parent.Parent.Attribute("id").Value into itemsGroup
            select new
            {
                Key = itemsGroup.Key,
                Layers =
                    from item in itemsGroup
                    group item by item.Parent.Attribute("id").Value into itemsSubGroup
                    select new { Key = itemsSubGroup.Key, Layer = itemsSubGroup.FirstOrDefault().Parent, LayerParameters = itemsSubGroup }
            };

            foreach (var stackupGroup in query)
            {
                if (stackup.ID != stackupGroup.Key) continue;

                int index = 0;
                foreach (var layerGroup in stackupGroup.Layers)
                {
                    ZLayer layer = new ZLayer(layerGroup.Key, stackup, stackup);
                    layer.Title = GetTextValue(layerGroup.Layer.Attribute("title"));
                    layer.IsFrozen = GetBoolValue(layerGroup.Layer.Attribute("isFrozen"));
                    layer.IsCustom = GetBoolValue(layerGroup.Layer.Attribute("isCustom"));
                    layer.IsEdited = GetBoolValue(layerGroup.Layer.Attribute("isEdited"));
                    layer.IsHidden = GetBoolValue(layerGroup.Layer.Attribute("isHidden"));
                    layer.IsReadOnly = GetBoolValue(layerGroup.Layer.Attribute("isReadOnly"));
                    layer.IsMaterialAssigned = GetBoolValue(layerGroup.Layer.Attribute("isMaterialAssigned"));
                    layer.Height = GetIntValue(layerGroup.Layer.Attribute("height"));
                    layer.Order = index;

                    foreach (ZParameter parameter in stackupSubparameters)
                    {
                        ZLayerParameter layerParameter = new ZLayerParameter(parameter.ID);
                        layerParameter.Value = string.Empty;
                        layerParameter.Layer = layer;
                        layerParameter.Parameter = parameter;

                        layer.LayerParameters.Add(layerParameter);
                    }

                    List<string> alreadySetLayerParameters = new List<string>();

                    foreach (XElement layerParameterElement in layerGroup.LayerParameters)
                    {
                        string parameterId = GetTextValue(layerParameterElement.Attribute("parameterId"));
                        ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterId);
                        if (layerParameter == null) continue;

                        layerParameter.Value = GetCellValue(layerParameterElement.Attribute("value"));
                        layerParameter.IsEdited = GetBoolValue(layerParameterElement.Attribute("isEdited"));
                        alreadySetLayerParameters.Add(layerParameter.ID);
                    }

                    foreach (ZLayerParameter layerParameter in layer.LayerParameters)
                    {
                        if (!alreadySetLayerParameters.Contains(layerParameter.ID))
                        {
                            ZLayerParameter typeLayerParameter = layer.LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDLayerType);
                            if (typeLayerParameter != null && ZLayer.LayerTypeDictionary.ContainsKey(typeLayerParameter.Value))
                            {
                                layerParameter.Value = layerParameter.GetDefaulLayerParameterValue(ZLayer.LayerTypeDictionary[typeLayerParameter.Value]);
                            }                            
                        }

                        if (layerParameter.Parameter.ValueType == ZValueType.Boolean && string.IsNullOrWhiteSpace(layerParameter.Value)) layerParameter.Value = "false";
                    }

                    stackup.Layers.Add(layer);
                    ++index;
                }
            }
        }

        private void AddEmptyMaterial(ZList<ZMaterial> materials, ZList<ZParameter> parameters)
        {
            ZMaterial material = new ZMaterial(ZStringConstants.EmptyMaterialID);
            material.IsFrozen = false;
            material.IsCustom = false;
            material.IsEdited = false;
            material.IsHidden = true;
            material.IsFiltered = true;
            material.IsReadOnly = true;
            material.Order = 0;

            foreach (ZParameter parameter in parameters)
            {
                ZMaterialParameter materialParameter = new ZMaterialParameter(parameter.ID);
                materialParameter.Parameter = parameter;
                materialParameter.Material = material;
                materialParameter.Value = string.Empty;
                materialParameter.IsEdited = false;
                parameter.MaterialParameters.Add(materialParameter);
                material.MaterialParameters.Add(materialParameter);
            }

            materials.Add(material);
        }

        private void SetMaterials(XElement root, ZList<ZMaterial> materials, ZList<ZParameter> parameters)
        {
            // Fill Materials collection
            var query =
            from item in root.Descendants("MaterialParameter")
            group item by item.Parent into itemsGroup
            select new { Key = itemsGroup.Key.Attribute("id").Value, Material = itemsGroup.FirstOrDefault().Parent, MaterialParameters = itemsGroup };

            int index = 0;
            foreach (var materialGroup in query)
            {
                ZMaterial material = new ZMaterial(materialGroup.Key);
                material.IsFrozen = GetBoolValue(materialGroup.Material.Attribute("isFrozen"));
                material.IsCustom = GetBoolValue(materialGroup.Material.Attribute("isCustom"));
                material.IsEdited = false; //GetBoolValue(materialGroup.Material.Attribute("isEdited"));
                material.IsHidden = GetBoolValue(materialGroup.Material.Attribute("isHidden"));
                material.IsFiltered = false;
                material.IsReadOnly = GetBoolValue(materialGroup.Material.Attribute("isReadOnly"));
                material.Height = GetIntValue(materialGroup.Material.Attribute("height"));
                material.Order = index;

                foreach (ZParameter parameter in parameters)
                {
                    ZMaterialParameter materialParameter = new ZMaterialParameter(parameter.ID);
                    materialParameter.Parameter = parameter;
                    materialParameter.Material = material;
                    materialParameter.Value = null;
                    materialParameter.IsEdited = false;
                    parameter.MaterialParameters.Add(materialParameter);
                    material.MaterialParameters.Add(materialParameter);
                }

                string materialManufacturer = string.Empty;

                foreach (XElement materialParameterElement in materialGroup.MaterialParameters)
                {
                    ZMaterialParameter materialParameter = material.MaterialParameters.Find(x => x.ID == GetTextValue(materialParameterElement.Attribute("parameterId")));
                    if (materialParameter == null) continue;

                    materialParameter.Material = null;
                    materialParameter.Value = GetCellValue(materialParameterElement.Attribute("value")) ?? string.Empty;
                    if (materialParameter.Parameter.ValueType == ZValueType.Boolean && string.IsNullOrWhiteSpace(materialParameter.Value)) materialParameter.Value = "false";
                    //materialParameter.IsEdited = GetBoolValue(materialParameterElement.Attribute("isEdited"));
                    materialParameter.Material = material;

                    if (materialParameter.Parameter.ID == ZStringConstants.DMLParameterIDManufacturer) materialManufacturer = materialParameter.Value.ToUpper();
                }

                string licensedManufacturer = License.TheLicense.dml.ToUpper();
                if (licensedManufacturer != "ALL" && licensedManufacturer != materialManufacturer) continue;

                materials.Add(material);
                ++index;
            }
        }

        /// <summary>
        /// Gets Boolean value of XML attribute.
        /// </summary>
        /// <param name="attr">XML attribute.</param>
        /// <returns>Boolean value of XML attribute.</returns>
        private bool GetBoolValue(XAttribute attr)
        {
            bool result;
            if (attr != null && bool.TryParse(attr.Value, out result)) return result;
            return false;
        }

        /// <summary>
        /// Adds XML attribute with Boolean value to the XML element.
        /// </summary>
        /// <param name="element">XML element in which the attribute should be added.</param>
        /// <param name="name">Name of attribute.</param>
        /// <param name="value">Value of attribute.</param>
        private void AddBoolAttribute(XElement element, string name, bool value)
        {
            if (element != null && value) element.Add(new XAttribute(name, value.ToString()));
        }

        /// <summary>
        /// Gets String value of XML attribute.
        /// </summary>
        /// <param name="attr">XML attribute.</param>
        /// <returns>String value of XML attribute.</returns>
        private string GetTextValue(XAttribute attr)
        {
            return attr != null ? attr.Value : "";
        }

        /// <summary>
        /// Adds XML attribute with String value to the XML element.
        /// </summary>
        /// <param name="element">XML element in which the attribute should be added.</param>
        /// <param name="name">Name of attribute.</param>
        /// <param name="value">Value of attribute.</param>
        private void AddTextAttribute(XElement element, string name, string value)
        {
            if (element != null && !string.IsNullOrEmpty(value)) element.Add(new XAttribute(name, value));
        }

        /// <summary>
        /// Gets Integer value of XML attribute.
        /// </summary>
        /// <param name="attr">XML attribute.</param>
        /// <returns>Integer value of XML attribute.</returns>
        private int GetIntValue(XAttribute attr)
        {
            int result;
            if (attr != null && int.TryParse(attr.Value, out result)) return result;
            return -1;
        }

        /// <summary>
        /// Adds XML attribute with Integer value to the XML element.
        /// </summary>
        /// <param name="element">XML element in which the attribute should be added.</param>
        /// <param name="name">Name of attribute.</param>
        /// <param name="value">Value of attribute.</param>
        private void AddIntAttribute(XElement element, string name, int value)
        {
            if (element != null && value >= 0) element.Add(new XAttribute(name, value.ToString()));
        }

        /// <summary>
        /// Gets Double value of XML attribute.
        /// </summary>
        /// <param name="attr">XML attribute.</param>
        /// <returns>Double value of XML attribute.</returns>
        private double GetDoubleValue(XAttribute attr)
        {
            double result;
            if (attr != null && double.TryParse(attr.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out result)) return result;
            return double.NaN;
        }

        /// <summary>
        /// Adds XML attribute with Double value to the XML element.
        /// </summary>
        /// <param name="element">XML element in which the attribute should be added.</param>
        /// <param name="name">Name of attribute.</param>
        /// <param name="value">Value of attribute.</param>
        private void AddDoubleAttribute(XElement element, string name, double value)
        {
            if (element != null && value != double.NaN) element.Add(new XAttribute(name, value.ToString()));
        }

        /// <summary>
        /// Gets DateTime value of XML attribute.
        /// </summary>
        /// <param name="attr">XML attribute.</param>
        /// <returns>DateTime value of XML attribute.</returns>
        private DateTime GetDateTimeValue(XAttribute attr)
        {
            DateTime result;
            if (attr != null && DateTime.TryParseExact(attr.Value, ZStringConstants.DateTimePattern, CultureInfo.InvariantCulture, DateTimeStyles.None, out result)) return result;
            return DateTime.Now;
        }

        /// <summary>
        /// Adds XML attribute with DateTime value to the XML element.
        /// </summary>
        /// <param name="element">XML element in which the attribute should be added.</param>
        /// <param name="name">Name of attribute.</param>
        /// <param name="value">Value of attribute.</param>
        private void AddDateTimeAttribute(XElement element, string name, DateTime value)
        {
            if (element != null) element.Add(new XAttribute(name, value.ToString(ZStringConstants.DateTimePattern, CultureInfo.InvariantCulture)));
        }

        /// <summary>
        /// Gets SelectList value of XML attribute.
        /// </summary>
        /// <param name="attr">XML attribute.</param>
        /// <returns>SelectList value of XML attribute.</returns>
        private SelectList GetListValue(XAttribute attr, ZList<SelectList> lists)
        {
            return attr != null ? lists.Find(x => x.ID == attr.Value) : null;
        }

        /// <summary>
        /// Adds XML attribute with SelectList value to the XML element.
        /// </summary>
        /// <param name="element">XML element in which the attribute should be added.</param>
        /// <param name="name">Name of attribute.</param>
        /// <param name="value">Value of attribute.</param>
        private void AddListAttribute(XElement element, string name, SelectList value)
        {
            if (element != null && value != null) element.Add(new XAttribute(name, value.ID));
        }
        
        /// <summary>
        /// Gets Enumeration value of XML attribute.
        /// </summary>
        /// <param name="attr">XML attribute.</param>
        /// <returns>Enumeration value of XML attribute.</returns>
        private T GetEnumValue<T>(XAttribute attr)
            where T : struct
        {
            T result;
            if (attr != null && !string.IsNullOrEmpty(attr.Value) && (System.Enum.TryParse<T>(attr.Value.First().ToString().ToUpper() + attr.Value.Substring(1).ToLower(), out result) || System.Enum.TryParse<T>(attr.Value.ToString(), out result))) return result;
            return default(T);
        }

        /// <summary>
        /// Adds XML attribute with Enumeration value to the XML element.
        /// </summary>
        /// <param name="element">XML element in which the attribute should be added.</param>
        /// <param name="name">Name of attribute.</param>
        /// <param name="value">Value of attribute.</param>
        private void AddEnumAttribute<T>(XElement element, string name, T value)
        {
            if (element != null && !value.Equals(default(T))) element.Add(new XAttribute(name, value.ToString()));
        }

        /// <summary>
        /// Gets Cell value of XML attribute.
        /// </summary>
        /// <param name="attr">XML attribute.</param>
        /// <returns>Cell value of XML attribute.</returns>
        private string GetCellValue(XAttribute attr)
        {
            if (attr == null) return "";
            
            switch (attr.Value)
            {
                case "YES":
                case "Yes":
                case "yes":
                    return "true";
                case "NO":
                case "No":
                case "no":
                    return "false";
                default:
                    return attr.Value;
            }            
        }

        /// <summary>
        /// Adds XML attribute with Cell value to the XML element.
        /// </summary>
        /// <param name="element">XML element in which the attribute should be added.</param>
        /// <param name="name">Name of attribute.</param>
        /// <param name="value">Value of attribute.</param>
        private void AddCellAttribute(XElement element, string name, string value)
        {
            if (element != null && !string.IsNullOrEmpty(value)) 
                switch(value)
                {
                    case "true":
                        element.Add(new XAttribute(name, "yes"));
                        break;
                    case "false":
                        element.Add(new XAttribute(name, "no"));
                        break;
                    default:
                        element.Add(new XAttribute(name, value));
                        break;
                }
        }

        /// <summary>
        /// Gets item by ID.
        /// </summary>
        /// <typeparam name="T">T is a subclass of ZEntity.</typeparam>
        /// <param name="list">Collection of items.</param>
        /// <param name="id">Item ID.</param>
        /// <returns></returns>
        private T GetItemByID<T>(ZList<T> list, string id)
            where T : ZEntity
        {
            return list.Find((T x) => x.ID == id);
        }
    }
}
