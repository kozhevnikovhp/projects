using System;
using System.Globalization;
using System.Linq;
using ZZero.ZPlanner.Commands;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Data.Properties;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.Data
{
    public class ZPlannerProject
    {
        private ZStackup stackup;

        public ZPlannerProject()
        {
            Parameters = new ZList<ZParameter>(this);
            SubParameters = new ZList<ZParameter>(this);
            Lists = new ZList<SelectList>(this);
            Version = 1.1F;
        }

        internal double Version { get; set; }

        internal ZList<ZParameter> Parameters { get; private set; }
        internal ZList<ZParameter> SubParameters { get; private set; }
        internal ZList<SelectList> Lists { get; private set; }

        internal DateTime Created { get; set; }
        internal DateTime Edited { get; set; }

        internal ZStackup Stackup 
        {
            get
            {
                if (stackup == null)
                {
                    ZPlannerManager.CreateNewStackup(this);
                }
                return stackup;
            }
            set
            {
                stackup = value;
            }
        }

        internal bool StackupIsEmpty
        { get { return stackup == null; } }

        internal static void ShiftParametersOrder(ZList<ZParameter> parameters, int order)
        {
            foreach (ZParameter parameter in parameters.OrderByDescending(X => X.Order))
            {
                if (parameter.Order >= order) ++parameter.Order;
            }
        }

        internal static void ShiftParametersOrder(ZList<ZParameter> parameters, ZParameter parameterToSetOrder, int order)
        {
            foreach (ZParameter parameter in parameters.OrderByDescending(X => X.Order))
            {
                if (parameter !=parameterToSetOrder && parameter.Order >= order) ++parameter.Order;
            }

            parameterToSetOrder.Order = order;
        }

        internal void CreateCustomParameter(ZTableType table, int order, int columnCount)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                if (table != ZTableType.Stackup) return;

                ZParameter parameter = new ZParameter();
                parameter.IsCustom = true;
                parameter.Order = columnCount;
                parameter.Table = table;
                parameter.ValueType = ZValueType.Text;

                InternalCreateCustomParameter(parameter, order);

                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new CreateCustomParameterCommand(parameter, order);
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        internal void InternalCreateCustomParameter(ZParameter parameter, int order)
        {
            foreach (ZLayer layer in Stackup.Layers)
            {
                ZLayerParameter layerParameter = new ZLayerParameter(parameter, layer);
                layer.LayerParameters.Add(layerParameter);
            }

            this.Parameters.Add(parameter);
            ShiftParametersOrder(Parameters, parameter, order);
            ZPlannerManager.PropertiesPanel.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(parameter);
        }

        internal void RemoveCustomParameter(ZTableType table, string id)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                if (table != ZTableType.Stackup) return;

                ZParameter parameter = Parameters.Find(X => X.ID == id);
                if (parameter == null || !parameter.IsCustom) return;

                int order = parameter.Order;
                InternalRemoveCustomParameter(parameter);

                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveCustomParameterCommand(parameter, order);
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        internal void InternalRemoveCustomParameter(ZParameter parameter)
        {
            if (parameter == null || !parameter.IsCustom) return;

            foreach (ZLayer layer in Stackup.Layers)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(X => X.ID == parameter.ID);
                layer.LayerParameters.Remove(layerParameter);
            }

            Parameters.Remove(parameter);
            ZPlannerManager.PropertiesPanel.SelectedObject = null;
        }

        internal void AddDefaultSelectLists()
        {
            foreach(SelectList defaultSelectList in GetDefaultSelectLists())
            {
                SelectList selectList = Lists.Find(x => x.ID == defaultSelectList.ID);
                if (selectList == null) Lists.Add(defaultSelectList);
            }
        }

        internal void AddDefaultParameters()
        {
            //add/merge parameters
            foreach (ZParameter defaultParameter in GetDefaultParameters())
            {
                //already exists?
                ZParameter parameter = Parameters.Find(s => s.ID == defaultParameter.ID);
                if (parameter != null)
                {
                    if (defaultParameter.SubParameters != null)
                    {
                        foreach (ZParameter defaultSubParameter in defaultParameter.SubParameters)
                        {
                            if (parameter.SubParameters != null)
                            {
                                //already exists?
                                ZParameter subparameter = parameter.SubParameters.Find(s => s.ID == defaultSubParameter.ID);
                                if (subparameter != null)
                                {
                                    subparameter.IsReadOnly = defaultSubParameter.IsReadOnly;
                                    subparameter.IsPrivate = defaultSubParameter.IsPrivate;
                                    subparameter.Table = defaultParameter.Table;
                                    subparameter.IsFrozen = defaultParameter.IsFrozen;
                                    continue;
                                }
                            }
                            else
                            {
                                parameter.SubParameters = new ZList<ZParameter>(parameter);
                            }

                            //defaultSubParameter.IsHidden = true;
                            defaultSubParameter.Order = GetNewParameterOrder();
                            parameter.SubParameters.Add(defaultSubParameter);
                            SubParameters.Add(defaultSubParameter);
                        }
                    }
                    parameter.IsReadOnly = defaultParameter.IsReadOnly;
                    parameter.IsPrivate = defaultParameter.IsPrivate;
                    parameter.Table = defaultParameter.Table;
                    parameter.IsFrozen = defaultParameter.IsFrozen;
                }
                else  //not found
                {
                    //defaultParameter.IsHidden = true;
                    defaultParameter.Order = GetNewParameterOrder();
                    Parameters.Add(defaultParameter);
                    if (defaultParameter.SubParameters != null && defaultParameter.SubParameters.Count > 0) SubParameters.AddRange(defaultParameter.SubParameters);
                }
            }
        }

        private int GetNewParameterOrder()
        {
            int parameterMaxOrder = (Parameters.Count > 0) ? Parameters[Parameters.Count - 1].Order + 1 : 0;
            int subparameterMaxOrder = (SubParameters.Count > 0) ? SubParameters[SubParameters.Count - 1].Order + 1 : 0;
            if (parameterMaxOrder > subparameterMaxOrder) return parameterMaxOrder;
            return subparameterMaxOrder;
        }

        /* // Bad performance
        internal static bool GetLayerParameterValue<T>(ZLayer layer, string parameterName, ref T value)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                if (layerParameter != null)
                {
                    var converter = TypeDescriptor.GetConverter(typeof(T));
                    if (converter != null)
                    {
                        try
                        {
                            value = (T)converter.ConvertFromString(null, CultureInfo.InvariantCulture, layerParameter.Value);
                            return true;
                        }
                        catch { }
                    }
                }
            }

            value = default(T);
            return false;
        }
        */

        internal static bool GetLayerParameterValue(ZLayer layer, string parameterName, ref double value)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, layer.Owner.GetType().Name));
                if (layerParameter != null)
                {
                    if (double.TryParse(layerParameter.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out value)) 
                    {
                        return true;
                    }
                }
            }

            value = 0D;
            return false;
        }

        internal static bool GetLayerParameterValue(ZLayer layer, string parameterName, ref float value)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, layer.Owner.GetType().Name));
                if (layerParameter != null)
                {
                    if (float.TryParse(layerParameter.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out value))
                    {
                        return true;
                    }
                }
            }

            value = 0F;
            return false;
        }

        internal static bool GetLayerParameterValue(ZLayer layer, string parameterName, ref int value)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, layer.Owner.GetType().Name));
                if (layerParameter != null)
                {
                    if (int.TryParse(layerParameter.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out value))
                    {
                        return true;
                    }
                }
            }

            value = 0;
            return false;
        }

        internal static bool GetLayerParameterValue(ZLayer layer, string parameterName, ref bool value)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, layer.Owner.GetType().Name));
                if (layerParameter != null)
                {
                    if (bool.TryParse(layerParameter.Value, out value))
                    {
                        return true;
                    }
                }
            }

            value = false;
            return false;
        }

        internal static bool GetLayerParameterValue(ZLayer layer, string parameterName, ref string value)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, layer.Owner.GetType().Name));
                if (layerParameter != null)
                {
                    value = layerParameter.Value;
                    return true;
                }
            }

            value = string.Empty;
            return false;
        }

        /* // Bad performance
        internal static bool SetLayerParameterValue<T>(ZLayer layer, string parameterName, T value)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                if (layerParameter != null)
                {
                    layerParameter.Value = value.ToString();
                    return true;
                }
            }

            return false;
        }
        */

        internal static bool SetLayerParameterValue(ZLayer layer, string parameterName, double value, bool isEdited = false)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, layer.Owner.GetType().Name));
                if (layerParameter != null)
                {
                    if (isEdited) layerParameter.SetEditedValue ((!double.IsNaN(value)) ? value.ToString(ZPlannerManager.GetFormatByParameter(layerParameter.Parameter), CultureInfo.InvariantCulture) : string.Empty);
                    else layerParameter.Value = (!double.IsNaN(value)) ? value.ToString(ZPlannerManager.GetFormatByParameter(layerParameter.Parameter), CultureInfo.InvariantCulture) : string.Empty;
                    return true;
                }
            }

            return false;
        }

        internal static bool SetLayerParameterValue(ZLayer layer, string parameterName, float value, bool isEdited = false)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, layer.Owner.GetType().Name));
                if (layerParameter != null)
                {
                    if (isEdited) layerParameter.SetEditedValue(value.ToString(ZPlannerManager.GetFormatByParameter(layerParameter.Parameter), CultureInfo.InvariantCulture));
                    else layerParameter.Value = value.ToString(ZPlannerManager.GetFormatByParameter(layerParameter.Parameter), CultureInfo.InvariantCulture);
                    return true;
                }
            }

            return false;
        }

        internal static bool SetLayerParameterValue(ZLayer layer, string parameterName, int value, bool isEdited = false)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, layer.Owner.GetType().Name));
                if (layerParameter != null)
                {
                    if (isEdited) layerParameter.SetEditedValue(value.ToString(CultureInfo.InvariantCulture));
                    else layerParameter.Value = value.ToString(CultureInfo.InvariantCulture);
                    return true;
                }
            }

            return false;
        }

        internal static bool SetLayerParameterValue(ZLayer layer, string parameterName, bool value, bool isEdited = false)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, layer.Owner.GetType().Name));
                if (layerParameter != null)
                {
                    if (isEdited) layerParameter.SetEditedValue(value.ToString());
                    else layerParameter.Value = value.ToString();
                    return true;
                }
            }

            return false;
        }

        internal static bool SetLayerParameterValue(ZLayer layer, string parameterName, string value, bool isEdited = false)
        {
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, layer.Owner.GetType().Name));
                if (layerParameter != null)
                {
                    if (isEdited) layerParameter.SetEditedValue(value);
                    else layerParameter.Value = value;
                    return true;
                }
            }

            return false;
        }

        internal ZList<SelectList> GetDefaultSelectLists()
        {
            ZList<SelectList> selectLists = new ZList<SelectList>(this);

            SelectList selectList = new SelectList("CopperThicknessList");
            selectList.AddValue("0.5");
            selectList.AddValue("1.0");
            selectList.AddValue("1.5");
            selectList.AddValue("2.0");
            selectList.AddValue("2.5");
            selectLists.Add(selectList);

            selectList = new SelectList("ViaSpanFilledList");
            selectList.AddValue("Resin Filled");
            selectList.AddValue("Copper Filled");
            selectLists.Add(selectList);

            return selectLists;
        }

        internal ZList<ZParameter> GetDefaultParameters()
        {
            ZList<ZParameter> parameters = new ZList<ZParameter>(this);

            ZParameter parameter;
            ZParameter subparameter;
            int order = 0;
            Options options = Options.TheOptions;


//-------------------------------------------------------------------------------------------------------------------
// Frozen parameters
//
            parameter = new ZParameter(ZStringConstants.ParameterIDLayerNumber);
            parameter.Title = "Layer #";
            parameter.Description = "Layer # of conductive layers.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.IsReadOnly = true;
            parameter.IsFrozen = true;
            parameter.Width = 80;
            parameter.Order = order++;
            parameters.Add(parameter);


//-------------------------------------------------------------------------------------------------------------------
// Via Span
//
            parameter = new ZParameter(ZStringConstants.ParameterIDViaSpan);
            parameter.Title = "Via Spans";
            parameter.Description = "Via Spans parameters.";
            parameter.Table = ZTableType.Viaspan;
            parameter.ValueType = ZValueType.Span;
            parameter.Order = order++;
            parameters.Add(parameter);

//-------------------------------------------------------------------------------------------------------------------
// Basic Layer parameters
//
            parameter = new ZParameter(ZStringConstants.ParameterIDLayerType);
            parameter.Title = "Type";
            parameter.Description = "Row type, for both conductive and dielectric materials.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Select;
            parameter.Width = 90;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDLayerName);
            parameter.Title = "Layer Name";
            parameter.Description = "User-defined name name. (E.g., GND, VCC, Signal L1)";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Text;
            parameter.Width = 100;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDMaterial);
            parameter.Title = "Material Name";
            parameter.Description = "Dielectric, conductive, or a specific material name. (e.g. the laminate manufacturer's product name)";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Text;
            parameter.Width = 90;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDManufacturer);
            parameter.Title = "Manufacturer";
            parameter.Description = "Manufacturer of the material.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Text;
            parameter.IsHidden = true;
            parameter.Width = 90;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDConstruction);
            parameter.Title = "Construction";
            parameter.Description = "Type of glass-cloth used, and the # of plies of cloth (ref: IPC-4121 and 4412).";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Text;
            parameter.Width = 110;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDResinContent);
            parameter.Title = "Resin (%)";
            parameter.Description = "Resin content (%) of dielectric material (with the remaining % being glass; resin has lower Dk and higher Df than glass).";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Percent;
            parameter.Measure = ZMeasures.Percent;
            parameter.DisplayMeasure = ZMeasures.Percent;
            parameter.DisplayFactor = 1.0;
            parameter.Width = 70;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDCopperPercent);
            parameter.Title = "% Copper (%)";
            parameter.Description = "% Copper coverage.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Percent;
            parameter.Measure = ZMeasures.Percent;
            parameter.DisplayMeasure = ZMeasures.Percent;
            parameter.DisplayFactor = 1.0;
            parameter.IsHidden = true;
            parameter.Width = 70;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDCopperThickness);
            parameter.Title = "Copper Weight (oz)";
            parameter.Description = "Copper Weight. (E.g., 1/2, 1, or 2 oz.)";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Select;
            parameter.List = Lists.Find( x => x.ID == "CopperThicknessList" );
            parameter.Measure = ZMeasures.Oz;
            parameter.DisplayMeasure = ZMeasures.Oz;
            parameter.DisplayFactor = 1.0;
            parameter.Width = 70;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDPrepregThickness);
            parameter.Title = "Prepreg Thickness";
            parameter.Description = "Thickness of the prepreg layer.";
            parameter.SetLengthDisplayUnits();
            parameter.Measure = ZMeasures.Mils;
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.DisplayFactor = 1.0;
            parameter.IsReadOnly = true;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDThickness);
            parameter.Title = "Thickness";
            parameter.Description = "Row thickness.";
            parameter.SetLengthDisplayUnits();
            parameter.Measure = ZMeasures.Mils;
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.DisplayFactor = 1.0;
            parameter.Width = 60;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDOriginThickness);
            parameter.Title = "Origin Thickness";
            parameter.Description = "Origin row thickness.";
            parameter.SetLengthDisplayUnits();
            parameter.Measure = ZMeasures.Mils;
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.DisplayFactor = 1.0;
            parameter.IsReadOnly = true;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDDielectricConstant);
            parameter.Title = "Dk (f)";
            parameter.Description = "Dielectric Constant (aka: Er; Relative Permitivity) at frequency shown.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Table;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDLossTangent);
            parameter.Title = "Df (f)";
            parameter.Description = "Dissipation Factor (aka: Loss Tangent) at frequency shown.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Table;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDRoughTop);
            parameter.Title = "Rz-Top (um) ";
            parameter.Description = "Rough Top";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.Measure = ZMeasures.Um;
            parameter.DisplayMeasure = ZMeasures.Um;
            //parameter.IsHidden = true;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDRoughBottom);
            parameter.Title = "Rz-Bottom (um)";
            parameter.Description = "Rough Bottom";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.Measure = ZMeasures.Um;
            parameter.DisplayMeasure = ZMeasures.Um;
            //parameter.IsHidden = true;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDDescription);
            parameter.Title = "Description";
            parameter.Description = "Description of row usage.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Text;
            parameter.IsPrivate = true;
            parameter.Width = 100;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDThermalConductivity);
            parameter.Title = "Thermal Conductivity";
            parameter.Description = "Thermal Conductivity";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDPlating);
            parameter.Title = "Plating";
            parameter.Description = "Plating";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Boolean;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDBulkRes);
            parameter.Title = "Bulk Resistivity (ohm*m)";
            parameter.Description = "Bulk Resistivity";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.Measure = ZMeasures.Ohm_by_M;
            parameter.DisplayMeasure = ZMeasures.Ohm_by_M;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDThermalCoef);
            parameter.Title = "ThermCoef";
            parameter.Description = "ThermCoef";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDCalcRoughTop);
            parameter.Title = "Rough Top";
            parameter.Description = "Rough Top";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.IsReadOnly = true;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDCalcRoughBottom);
            parameter.Title = "Rough Bottom";
            parameter.Description = "Rough Bottom";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.IsReadOnly = true;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDEtchFactor);
            parameter.Title = "EtchFactor";
            parameter.Description = "EtchFactor";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDNarrowTop);
            parameter.Title = "NarrowTop";
            parameter.Description = "NarrowTop";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Boolean;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDConformal);
            parameter.Title = "Conformal";
            parameter.Description = "Conformal";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Boolean;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDAtchMetal);
            parameter.Title = "AtchMetal";
            parameter.Description = "AtchMetal";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Boolean;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDPlaneReference);
            parameter.Title = "Plane Reference";
            parameter.Description = "Defines if the layer can be a reference plane for adjacent layers";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Select;
            parameter.Width = 90;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDWarpYarnCount);
            parameter.Title = "Warp Yarn Count";
            parameter.Description = "Warp Yarn Count.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.DisplayFactor = 1.0;
            parameter.IsHidden = true;
            parameter.Width = 60;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDFillYarnCount);
            parameter.Title = "Fill Yarn Count";
            parameter.Description = "Fill Yarn Count.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.DisplayFactor = 1.0;
            parameter.IsHidden = true;
            parameter.Width = 60;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDWeavePitch);
            parameter.Title = "Weave Pitch";
            parameter.Description = "Weave Pitch = 1000 / Warp Yarn Count.";
            parameter.SetLengthDisplayUnits();
            parameter.Measure = ZMeasures.Mils;
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.DisplayFactor = 1.0;
            parameter.IsReadOnly = true;
            parameter.IsPrivate = true;
            parameter.Width = 80;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDFillPitch);
            parameter.Title = "Fill Pitch";
            parameter.Description = "Fill Pitch = 1000 / Fill Yarn Count.";
            parameter.SetLengthDisplayUnits();
            parameter.Measure = ZMeasures.Mils;
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Number;
            parameter.DisplayFactor = 1.0;
            parameter.IsReadOnly = true;
            parameter.IsPrivate = true;
            parameter.Width = 80;
            parameter.Order = order++;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDComments);
            parameter.Title = "Comments";
            parameter.Description = "Row comments.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Text;
            parameter.IsHidden = true;
            parameter.Width = 100;
            parameter.Order = ++order;
            parameters.Add(parameter);

            parameter = new ZParameter(ZStringConstants.ParameterIDSequentialLamination);
            parameter.Title = "Sequential Lamination";
            parameter.Description = "ParameterIDSequentialLamination.";
            parameter.Table = ZTableType.Stackup;
            parameter.ValueType = ZValueType.Boolean;
            //--parameter.IsHidden = true;
            parameter.IsPrivate = true;
            parameter.Width = 60;
            parameter.Order = ++order;
            parameters.Add(parameter);

            ZPlannerManager.Dml.AddDMLParameters(parameters, ref order);

//-------------------------------------------------------------------------------------------------------------------
// Impedance
//
            parameter = new ZParameter(ZStringConstants.ParameterIDSingleEndedImpedanceZo);
            parameter.Title = "SINGLE-ENDED SIGNALS (Zo)";
            parameter.Table = ZTableType.Single;
            parameter.ValueType = ZValueType.Container;
            parameter.Order = order++;
            parameter.SubParameters = new ZList<ZParameter>(parameter);
            parameters.Add(parameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_TraceWidth);
            subparameter.Title = "Trace Width";
            subparameter.Description = "Trace width for single-ended impedance calculation.";
            subparameter.SetLengthDisplayUnits();
            subparameter.Measure = ZMeasures.Mils;
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Width = 80;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

/*            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_TraceSpacing);
            subparameter.Title = "Trace Spacing (mils)";
            subparameter.Description = "Trace Spacing for single-ended impedance.";
            subparameter.ValueType = ZValueType.Number;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);
*/
            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_Target);
            subparameter.Title = "Target Zo (ohms)";
            subparameter.Description = "Target value for single-ended impedance.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Ohm;
            subparameter.DisplayMeasure = ZMeasures.Ohm;
            subparameter.IsPrivate = true;
            subparameter.Width = 50;
            subparameter.IsReadOnly = true;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_Zo);
            subparameter.Title = "Zo (ohms)";
            subparameter.Description = "Single-ended impedance calculation.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Ohm;
            subparameter.DisplayMeasure = ZMeasures.Ohm;
            subparameter.Width = 50;
            subparameter.IsReadOnly = true;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_Frequency);
            subparameter.Title = "Frequency (GHz)";
            subparameter.Description = "Target frequency for Dk (f) and Df (f).";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Number;
            subparameter.IsHidden = true;
            subparameter.Measure = ZMeasures.GHz;
            subparameter.DisplayMeasure = ZMeasures.GHz;
            subparameter.Width = 70;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_DielectricConstant);
            subparameter.Title = "Dk (f)";
            subparameter.Description = "Dielectric Constant (aka: Er; Relative Permitivity) at frequency shown.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Table;
            subparameter.Width = 60;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_LossTangent);
            subparameter.Title = "Df (f)";
            subparameter.Description = "Dissipation Factor (aka: Loss Tangent) at frequency shown.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Table;
            subparameter.IsHidden = true;
            subparameter.Width = 60;
            subparameter.Order = ++order;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_LossViewer);
            subparameter.Title = "Loss Viewer";
            subparameter.Description = "Click to view loss for signal/mixed layers.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Link;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 70;
            subparameter.ParameterGroup = "Loss Planning";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_InsertionLoss);
            subparameter.Title = "Insertion Loss (dB/in)";
            subparameter.Description = "Insertion Loss for signal/mixed layers.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Db_per_In;
            subparameter.DisplayMeasure = ZMeasures.Db_per_In;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 60;
            subparameter.ParameterGroup = "Loss Planning";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_TraceLength);
            subparameter.Title = "Trace Length (in)";
            subparameter.Description = "Trace length for signal/mixed layers.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.In;
            subparameter.DisplayMeasure = ZMeasures.In;
            subparameter.IsReadOnly = false;
            subparameter.IsHidden = true;
            subparameter.Width = 60;
            subparameter.ParameterGroup = "Loss Planning";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_TotalLoss);
            subparameter.Title = "Total Loss (dB)";
            subparameter.Description = "Total Loss for signal/mixed layers.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Db;
            subparameter.DisplayMeasure = ZMeasures.Db;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 60;
            subparameter.ParameterGroup = "Loss Planning";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_PropagationVelocity);
            subparameter.Title = "Propagation Velocity (in/ns)";
            subparameter.Description = "Propagation Velocity for signal/mixed layers.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.In_per_Ns;
            subparameter.DisplayMeasure = ZMeasures.In_per_Ns;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 70;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_PropagationDelay);
            subparameter.Title = "Propagation Delay (ns)";
            subparameter.Description = "Propagation Delay for signal/mixed layers.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Ns;
            subparameter.DisplayMeasure = ZMeasures.Ns;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 70;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZo_IsUsed);
            subparameter.Title = "ON/OFF";
            subparameter.Description = "Check this to include this row into Zo calculations.";
            subparameter.Table = ZTableType.Single;
            subparameter.ValueType = ZValueType.Boolean;
            subparameter.Width = 50;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            //-------------------------------------------------------------------------------------------------------------------
// Differential Impedance
//
            parameter = new ZParameter(ZStringConstants.ParameterIDDifferentialImpedanceZdiff);
            parameter.Title = "DIFFERENTIAL SIGNALS (Zdiff)";
            parameter.Table = ZTableType.Pair;
            parameter.ValueType = ZValueType.Container;
            parameter.Order = order++;
            parameter.SubParameters = new ZList<ZParameter>(parameter);
            parameters.Add(parameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_TraceWidth);
            subparameter.Title = "Diff. Trace Width";
            subparameter.Description = "Trace width for differential-impedance calculation.";
            subparameter.SetLengthDisplayUnits();
            subparameter.Measure = ZMeasures.Mils;
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Width = 100;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_Zo);
            subparameter.Title = "Diff. Zo (ohms)";
            subparameter.Description = "Zo for differential impedance.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Ohm;
            subparameter.DisplayMeasure = ZMeasures.Ohm;
            subparameter.IsHidden = false;
            subparameter.Width = 50;
            subparameter.IsReadOnly = true;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_TraceSpacing);
            subparameter.Title = "Spacing";
            subparameter.Description = "Trace spacing for differential-impedance calculation.";
            subparameter.SetLengthDisplayUnits();
            subparameter.Measure = ZMeasures.Mils;
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Width = 80;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_TracePitch);
            subparameter.Title = "Diff. Trace Pitch";
            subparameter.Description = "Trace Pitch for differential-impedance calculation.";
            subparameter.SetLengthDisplayUnits();
            subparameter.Measure = ZMeasures.Mils;
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 80;
            subparameter.ParameterGroup = "Glass Pitch";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_WeavePitch);
            subparameter.Title = "Weave Pitch";
            subparameter.Description = "Weave Pitch = 1000 / Warp Yarn Count.";
            subparameter.SetLengthDisplayUnits();
            subparameter.Measure = ZMeasures.Mils;
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.DisplayFactor = 1.0;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 80;
            subparameter.ParameterGroup = "Glass Pitch";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_FillPitch);
            subparameter.Title = "Fill Pitch";
            subparameter.Description = "Fill Pitch = 1000 / Fill Yarn Count.";
            subparameter.SetLengthDisplayUnits();
            subparameter.Measure = ZMeasures.Mils;
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.DisplayFactor = 1.0;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 80;
            subparameter.ParameterGroup = "Glass Pitch";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_Zdiff);
            subparameter.Title = "Zdiff (ohms)";
            subparameter.Description = "Differential impedance calculation.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Ohm;
            subparameter.DisplayMeasure = ZMeasures.Ohm;
            subparameter.Width = 50;
            subparameter.IsReadOnly = true;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_Frequency);
            subparameter.Title = "Diff. Frequency (GHz)";
            subparameter.Description = "Target frequency for Dk (f) and Df (f).";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.GHz;
            subparameter.DisplayMeasure = ZMeasures.GHz;
            subparameter.Width = 70;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_BitRate);
            subparameter.Title = "Bit Rate (Gb/s)";
            subparameter.Description = "Bit Rate";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Gb_per_s;
            subparameter.DisplayMeasure = ZMeasures.Gb_per_s;
            subparameter.IsHidden = true;
            subparameter.IsReadOnly = true;
            subparameter.Width = 40;
            subparameter.ParameterGroup = "GWS";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_UnitInterval);
            subparameter.Title = "Unit Interval (pSec) ";
            subparameter.Description = "Unit Interval";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.pS;
            subparameter.DisplayMeasure = ZMeasures.pS;
            subparameter.IsHidden = true;
            subparameter.IsReadOnly = true;
            subparameter.Width = 50;
            subparameter.ParameterGroup = "GWS";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_SkewTolerance);
            subparameter.Title = "Skew Tolerance (pSec)";
            subparameter.Description = "This is the approximate eye tolerance for various differential skew sources, including glass-weave skew (aka: fiber-weave effect).";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.pS;
            subparameter.DisplayMeasure = ZMeasures.pS;
            subparameter.IsHidden = true;
            subparameter.IsReadOnly = true;
            subparameter.Width = 50;
            subparameter.ParameterGroup = "GWS";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_DielectricConstant);
            subparameter.Title = "Diff. Dk (f)";
            subparameter.Description = "Dielectric Constant (aka: Er; Relative Permitivity) at frequency shown.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Table;
            subparameter.Width = 60;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_LossTangent);
            subparameter.Title = "Diff. Df (f)";
            subparameter.Description = "Dissipation Factor (aka: Loss Tangent) at frequency shown.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Table;
            subparameter.Width = 60;
            subparameter.Order = ++order;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_LossViewer);
            subparameter.Title = "Loss Viewer";
            subparameter.Description = "Click to view loss for signal/mixed layers.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Link;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 70;
            subparameter.ParameterGroup = "Loss Planning";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_RoughTop);
            subparameter.Title = "Rz-Top (um) ";
            subparameter.Description = "Rough Top";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Um;
            subparameter.DisplayMeasure = ZMeasures.Um;
            subparameter.IsHidden = true;
            subparameter.Width = 60;
            subparameter.ParameterGroup = "Copper Roughness";
            subparameter.Order = ++order;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_RoughBottom);
            subparameter.Title = "Rz-Bottom (um)";
            subparameter.Description = "Rough Bottom";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Um;
            subparameter.DisplayMeasure = ZMeasures.Um;
            subparameter.IsHidden = true;
            subparameter.Width = 60;
            subparameter.ParameterGroup = "Copper Roughness";
            subparameter.Order = ++order;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_InsertionLoss);
            subparameter.Title = "Insertion Loss (dB/in)";
            subparameter.Description = "Insertion Loss for signal/mixed layers.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Db_per_In;
            subparameter.DisplayMeasure = ZMeasures.Db_per_In;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 60;
            subparameter.ParameterGroup = "Loss Planning";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_TraceLength);
            subparameter.Title = "Trace Length (in)";
            subparameter.Description = "Trace length for signal/mixed layers.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.In;
            subparameter.DisplayMeasure = ZMeasures.In;
            subparameter.IsReadOnly = false;
            subparameter.IsHidden = true;
            subparameter.Width = 60;
            subparameter.ParameterGroup = "Loss Planning";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_TotalLoss);
            subparameter.Title = "Total Loss (dB)";
            subparameter.Description = "Total Loss for signal/mixed layers.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Db;
            subparameter.DisplayMeasure = ZMeasures.Db;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.Width = 60;
            subparameter.ParameterGroup = "Loss Planning";
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_PropagationVelocity);
            subparameter.Title = "Diff. Propagation Velocity (in/ns)";
            subparameter.Description = "Propagation Velocity for signal/mixed layers.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.In_per_Ns;
            subparameter.DisplayMeasure = ZMeasures.In_per_Ns;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.IsPrivate = true;
            subparameter.Width = 70;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_PropagationDelay);
            subparameter.Title = "Diff. Propagation Delay (ns)";
            subparameter.Description = "Propagation Delay for signal/mixed layers.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Number;
            subparameter.Measure = ZMeasures.Ns;
            subparameter.DisplayMeasure = ZMeasures.Ns;
            subparameter.IsReadOnly = true;
            subparameter.IsHidden = true;
            subparameter.IsPrivate = true;
            subparameter.Width = 70;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            subparameter = new ZParameter(ZStringConstants.ParameterIDZdiff_IsUsed);
            subparameter.Title = "ON/OFF";
            subparameter.Description = "Check this to include this row into Zdiff calculations.";
            subparameter.Table = ZTableType.Pair;
            subparameter.ValueType = ZValueType.Boolean;
            subparameter.Width = 50;
            subparameter.Order = order++;
            parameter.SubParameters.Add(subparameter);

            return parameters;
        }
    }
}
