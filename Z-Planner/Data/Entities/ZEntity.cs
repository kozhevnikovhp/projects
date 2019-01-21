using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using ZZero.ZPlanner.ZConfiguration;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.Utils;
using System.Globalization;
using ZZero.ZPlanner.Commands;
using System.Text.RegularExpressions;
using System.Collections.Specialized;
using System.Windows.Forms;
using ZZero.ZPlanner.Translators;
using ZZero.ZPlanner.Data.Attributes;

namespace ZZero.ZPlanner.Data.Entities
{
    enum ZValueType { Text, Number, Boolean, Select, Span, Container, Percent, Link, Ratio, Table}
    enum ZMeasures { None, In, Um, Mm, C, GHz, Mils, Percent, Oz, Db, Db_per_In, Ns, In_per_Ns, Ohm, Ohm_by_M, Gb_per_s, pS }
    enum ZLayerType {Core, Prepreg, SolderMask, Plane, Signal, SplitMixed }
    public enum ZLibraryCategory { ZZero, Corporate, Local}
    enum ZCopperCoverageType { ManuallyEntered, PrepregProportional, PrepregPercent, RestoredOnImport }

    enum OxideAlternativeTreatmentsType {MacDermidMultiBond100LE, MacDermidEnthoneAlphaPrep, MacDermidMultiBond100ZK, AtotechBondFilm, MECetchBONDCZ8100, MacDermidMultiBond100HP}
    public enum ZTableType { Undefined, Stackup, Viaspan, Single, Pair }

    public interface IOrder
    {
        int Order { get; set; }
    }

    abstract class ZEntity
    {
        string _id;
        public string ID 
        { 
            get 
            { 
                return _id; 
            } 
            set 
            { 
                _id = value; 
                if (!idList.Contains(_id))
                    idList.Add(_id); 
            } 
        }

        string _title;
        public virtual string getTitle() { return _title; }

        public string Title
        {
            get { return getTitle(); }
            set { if (value != _title) { new ChangeEntityTitleCommand(this, _title, value); _title = value; NotifyPropertyChanged("Title"); } }
        }

        public void SetTitle(string title) { _title = title; }

        public event PropertyChangedEventHandler PropertyChanged;

        public void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        protected string GenerateID(string prefix)
        {
            string newID = prefix + "_0";

            for (uint i = 1; i < uint.MaxValue; ++i )
            {
                newID = prefix + "_" + i;
                if (!idList.Contains(newID))
                break;
            }

            return newID;
        }

        protected string GenerateTitle<T>(ZList<T> collection, string prefix)
            where T:ZEntity
        {
            string newTitle = prefix + "  0";

            for (uint i = 1; i < uint.MaxValue; ++i)
            {
                if (i < 10) newTitle = prefix + "  " + i;
                else newTitle = prefix + " " + i;
                var foundItem = collection.Find((T x) => x.Title == newTitle);
                if (foundItem == null) break;
            }

            return newTitle;
        }

        internal static HashSet<string> IdList
        { get { return idList; } }

        protected static HashSet<string> idList = new HashSet<string>();

        public override string ToString()
        {
            return Title;
        }
    }

    class ZParameter : ZEntity, IOrder
    {
        public ZParameter()
        {
            ID = GenerateID("Parameter");
            this.Title = GenerateTitle(ZPlannerManager.Project.Parameters, "Custom");
            MaterialParameters = new ZList<ZMaterialParameter>(this);
        }
        public ZParameter(string id)
        {
            ID = id;
            MaterialParameters = new ZList<ZMaterialParameter>(this);
        }

        public ZList<ZMaterialParameter> MaterialParameters { get; private set; }

        public void SetPitchDisplayUnits()
        {
            if (Options.TheOptions.isUnitsEnglish())
                this.DisplayMeasure = ZMeasures.Mils;
            else
                this.DisplayMeasure = ZMeasures.Mm;
        }

        public void SetWidthSpacingDisplayUnits()
        {
            if (Options.TheOptions.isUnitsEnglish())
                this.DisplayMeasure = ZMeasures.Mils;
            else
                this.DisplayMeasure = ZMeasures.Mm;
        }

        public void SetThicknessDisplayUnits()
        {
            if (Options.TheOptions.isUnitsEnglish())
                this.DisplayMeasure = ZMeasures.Mils;
            else
                this.DisplayMeasure = ZMeasures.Um;
        }

        public override string getTitle()
        {
            string ret = base.getTitle();
            if (ID == ZStringConstants.ParameterIDPrepregThickness   ||
                ID == ZStringConstants.ParameterIDThickness          ||
                ID == ZStringConstants.ParameterIDOriginThickness    ||
                ID == ZStringConstants.ParameterIDZo_TraceSpacing    ||
                ID == ZStringConstants.ParameterIDZo_TraceWidth      ||
                ID == ZStringConstants.ParameterIDZdiff_TraceWidth ||
                ID == ZStringConstants.ParameterIDZdiff_TraceSpacing ||
                ID == ZStringConstants.ParameterIDFillPitch ||
                ID == ZStringConstants.ParameterIDWeavePitch         ||
                ID == ZStringConstants.ParameterIDZdiff_TracePitch   ||
                ID == ZStringConstants.ParameterIDZdiff_WeavePitch   ||
                ID == ZStringConstants.ParameterIDZdiff_FillPitch)
            {
                if (DisplayMeasure == ZMeasures.Mils)
                    ret += " (mils)";
                else if (DisplayMeasure == ZMeasures.Um)
                    ret += " (um)";
                else if (DisplayMeasure == ZMeasures.Mm)
                    ret += " (mm)";
            }
            //Console.WriteLine(ret);
            return ret;
        }

        string _description;
        public string Description 
        { 
            get 
            { 
                return _description; 
            } 
            set 
            {
                if (_description != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterDescriptionCommand(this, _description, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _description = value;
                    NotifyPropertyChanged("Description");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        ZValueType _valueType;
        public ZValueType ValueType
        {
            get
            {
                return _valueType;
            }
            set
            {
                if (_valueType != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterValueTypeCommand(this, _valueType, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _valueType = value;
                    NotifyPropertyChanged("ValueType");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        ZMeasures _measure;
        public ZMeasures Measure
        {
            get
            {
                return _measure;
            }
            set
            {
                if (_measure != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterMeasureCommand(this, _measure, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _measure = value;
                    NotifyPropertyChanged("Measure");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        ZMeasures _displayMeasure;
        public ZMeasures DisplayMeasure
        {
            get
            {
                return _displayMeasure;
            }
            set
            {
                if (_displayMeasure != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterDisplayMeasureCommand(this, _displayMeasure, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _displayMeasure = value;
                    NotifyPropertyChanged("DisplayMeasure");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        double _displayFactor;
        public double DisplayFactor
        {
            get
            {
                return _displayFactor;
            }
            set
            {
                if (_displayFactor != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterDisplayFactorCommand(this, _displayFactor, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _displayFactor = value;
                    NotifyPropertyChanged("DisplayFactor");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        SelectList _list;
        public SelectList List
        {
            get
            {
                return _list;
            }
            set
            {
                if (_list != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterListCommand(this, _list, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _list = value;
                    NotifyPropertyChanged("List");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        string _defaultValue;
        public string DefaultValue
        {
            get
            {
                return _defaultValue;
            }
            set
            {
                if (_defaultValue != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterDefaultValueCommand(this, _defaultValue, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _defaultValue = value;
                    NotifyPropertyChanged("DefaultValue");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isCustom;
        public bool IsCustom
        {
            get
            {
                return _isCustom;
            }
            set
            {
                if (_isCustom != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterIsCustomCommand(this, _isCustom, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isCustom = value;
                    NotifyPropertyChanged("IsCustom");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private bool isHidden;
        public bool IsHidden
        {
            get 
            { 
                return isHidden; 
            }
            set
            {
                if (isHidden != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterIsHiddenCommand(this, isHidden, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isHidden = value;
                    NotifyPropertyChanged("IsHidden");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isCollapsed;
        public bool IsCollapsed
        {
            get
            {
                return _isCollapsed;
            }
            set
            {
                if (_isCollapsed != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterIsCollapsedCommand(this, _isCollapsed, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isCollapsed = value;
                    NotifyPropertyChanged("IsCollapsed");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isReadOnly;
        public bool IsReadOnly
        {
            get
            {
                return _isReadOnly;
            }
            set
            {
                if (_isReadOnly != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterIsReadOnlyCommand(this, _isReadOnly, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isReadOnly = value;
                    NotifyPropertyChanged("IsReadOnly");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isNotEmpty;
        public bool IsNotEmpty
        {
            get
            {
                return _isNotEmpty;
            }
            set
            {
                if (_isNotEmpty != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterIsNotEmptyCommand(this, _isNotEmpty, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isNotEmpty = value;
                    NotifyPropertyChanged("IsNotEmpty");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isFrozen;
        public bool IsFrozen
        {
            get
            {
                return _isFrozen;
            }
            set
            {
                if (_isFrozen != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterIsFrozenCommand(this, _isFrozen, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isFrozen = value;
                    NotifyPropertyChanged("IsFrozen");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isPrivate;
        public bool IsPrivate
        {
            get
            {
                return _isPrivate;
            }
            set
            {
                if (_isPrivate != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterIsPrivateCommand(this, _isPrivate, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isPrivate = value;
                    NotifyPropertyChanged("IsPrivate");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        ZTableType _table;
        public ZTableType Table
        {
            get
            {
                return _table;
            }
            set
            {
                if (_table != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterTableCommand(this, _table, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _table = value;
                    NotifyPropertyChanged("Table");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        int _width;
        public int Width
        {
            get
            {
                return _width;
            }
            set
            {
                if (_width != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterWidthCommand(this, _width, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _width = value;
                    NotifyPropertyChanged("Width");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        int _widthToExpand;
        public int WidthToExpand
        {
            get
            {
                return _widthToExpand;
            }
            set
            {
                if (_widthToExpand != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterWidthToExpandCommand(this, _widthToExpand, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _widthToExpand = value;
                    NotifyPropertyChanged("WidthToExpand");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        int _order;
        public int Order 
        { 
            get 
            { 
                return _order; 
            } 
            set 
            {
                if (_order != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterOrderCommand(this, _order, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _order = value; 
                    NotifyPropertyChanged("Order");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            } 
        }

        string _parameterGroup = string.Empty;
        public string ParameterGroup
        {
            get
            {
                return _parameterGroup;
            }
            set
            {
                if (_parameterGroup != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeParameterGroupCommand(this, _parameterGroup, value);
                    //bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _parameterGroup = value;
                    //NotifyPropertyChanged("ParameterGroup");
                    //ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        public ZList<ZParameter> SubParameters { get; set; }

        public ZParameter MakeCopy()
        {
            ZParameter parameter = new ZParameter(ID);
            parameter.Title = Title;
            parameter.Description = Description;
            parameter.ValueType = ValueType;
            parameter.Measure = Measure;
            parameter.DisplayMeasure = DisplayMeasure;
            parameter.DisplayFactor = DisplayFactor;
            parameter.DefaultValue = DefaultValue;
            parameter.IsCustom = IsCustom;
            parameter.IsHidden = IsHidden;
            parameter.IsPrivate = IsPrivate;
            parameter.IsCollapsed = IsCollapsed;
            parameter.IsReadOnly = IsReadOnly;
            parameter.IsNotEmpty = IsNotEmpty;
            parameter.IsFrozen = IsFrozen;
            parameter.List = List;
            parameter.Table = Table;
            parameter.Width = Width;
            parameter.WidthToExpand = WidthToExpand;
            parameter.Order = 0;//must be overwritten

            return parameter;
        }
    }

    class ZMaterial : ZEntity, IOrder
    {
        public ZMaterial(ZLayerType type)
        {
            ID = GenerateID("Material");
            //Title = GenerateTitle(ZPlannerManager.Dml.Materials, "Material");
            IsCustom = true;
            IsEdited = true;
            IsFrozen = false;
            IsHidden = false;
            IsFiltered = false;
            IsReadOnly = false;
            Height = -1;
            Order = ZPlannerManager.Dml.Materials.Count;

            MaterialParameters = new ZList<ZMaterialParameter>(this);

            foreach (var parameter in ZPlannerManager.Dml.Parameters)
            {
                ZMaterialParameter materialParameter = new ZMaterialParameter(parameter, this, type);
                MaterialParameters.Add(materialParameter);
                materialParameter.Parameter.MaterialParameters.Add(materialParameter);
            }
        }

        public ZMaterial(string id)
        {
            ID = id;
            //Title = GenerateTitle(ZPlannerManager.Dml.Materials, "Material");
            MaterialParameters = new ZList<ZMaterialParameter>(this);
        }

        public ZList<ZMaterialParameter> MaterialParameters { get; private set; }

        public ZMaterial MakeCopy()
        {
            ZMaterial material = new ZMaterial(ID);
            material.Title = Title;
            material.IsCustom = IsCustom;
            material.IsEdited = IsEdited;
            material.IsFrozen = IsFrozen;
            material.IsHidden = IsHidden;
            material.IsFiltered = IsFiltered;
            material.IsReadOnly = IsReadOnly;
            material.Height = Height;
            material.Order = ZPlannerManager.Dml.Materials.Count;

            foreach (ZMaterialParameter materialParameter in MaterialParameters)
            {
                ZMaterialParameter clonedMaterialParameter = materialParameter.Clone();
                clonedMaterialParameter.Material = material;
                material.MaterialParameters.Add(clonedMaterialParameter);
            }

            return material;
        }

        public ZMaterial Clone()
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {

                ZMaterial material = new ZMaterial(ID + "_Cloned");
                material.Title = GenerateTitle(ZPlannerManager.Dml.Materials, "Material");
                material.IsCustom = IsCustom;
                material.IsEdited = true; // IsEdited;
                material.IsFrozen = IsFrozen;
                material.IsHidden = IsHidden;
                material.IsFiltered = IsFiltered;
                material.IsReadOnly = IsReadOnly;
                material.Height = Height;
                material.Order = ZPlannerManager.Dml.Materials.Count;

                foreach (ZMaterialParameter materialParameter in MaterialParameters)
                {
                    ZMaterialParameter clonedMaterialParameter = materialParameter.Clone();
                    clonedMaterialParameter.Material = material;
                    material.MaterialParameters.Add(clonedMaterialParameter);
                }

                material.SetMaterialParameterValue(ZStringConstants.DMLParameterIDCategory, ZPlannerManager.GetMeterialCategoryStringByUser());

                return material;
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public ZLibraryCategory? GetMaterialCategory()
        {
            ZLibraryCategory? libraryCategory = null;

            ZMaterialParameter materialParameter = this.MaterialParameters.Find(x => x.ID == ZStringConstants.DMLParameterIDCategory);
            if (materialParameter != null && LibraryCategoryDictionary.ContainsKey(materialParameter.Value))
            {
                libraryCategory = LibraryCategoryDictionary[materialParameter.Value];
            }

            return libraryCategory;
        }

        public void SetMaterialCategory(ZLibraryCategory libCategory)
        {
            ZMaterialParameter materialParameter = this.MaterialParameters.Find(x => x.ID == ZStringConstants.DMLParameterIDCategory);
            if (materialParameter != null)
            {
                materialParameter.Value = GetMaterialCategoryString(libCategory);
            }
        }

        public string GetMaterialCategoryString(ZLibraryCategory category)
        {
            switch (category)
            {
                case ZLibraryCategory.Local: return "Local";
                case ZLibraryCategory.Corporate: return "Corporate";
                case ZLibraryCategory.ZZero: return "Z-zero";
            }

            return "Local";
        }

        public ZLayerType? GetMaterialType()
        {
            ZLayerType? materialType = null;

            ZMaterialParameter materialParameter = this.MaterialParameters.Find(x => x.ID == ZStringConstants.DMLParameterIDType);
            if (materialParameter != null && MaterialTypeDictionary.ContainsKey(materialParameter.Value))
            {
                materialType = MaterialTypeDictionary[materialParameter.Value];
            }

            return materialType;
        }

        internal ZMaterialParameter GetMaterialParameter(string parameterName)
        {
            return MaterialParameters.Find(x => x.ID == parameterName);
        }

        internal string GetMaterialParameterValue(string parameterName)
        {
            ZMaterialParameter materialParameter = MaterialParameters.Find(x => x.ID == parameterName);
            if (materialParameter != null)
            {
                return materialParameter.Value;
            }

            return null;
        }

        internal void SetMaterialParameterValue(string parameterName, string value)
        {
            ZMaterialParameter materialParameter = MaterialParameters.Find(x => x.ID == parameterName);
            if (materialParameter != null)
            {
                materialParameter.Value = value;
            }
        }

        public static Dictionary<string, ZLayerType> MaterialTypeDictionary = new Dictionary<string, ZLayerType>();
        public static Dictionary<string, ZLibraryCategory> LibraryCategoryDictionary = new Dictionary<string, ZLibraryCategory>();
        static ZMaterial()
        {
            MaterialTypeDictionary.Add("Core", ZLayerType.Core);
            MaterialTypeDictionary.Add("Prepreg", ZLayerType.Prepreg);
            MaterialTypeDictionary.Add("Solder Mask", ZLayerType.SolderMask);

            LibraryCategoryDictionary.Add("Z-zero", ZLibraryCategory.ZZero);
            LibraryCategoryDictionary.Add("Corporate", ZLibraryCategory.Corporate);
            LibraryCategoryDictionary.Add("Local", ZLibraryCategory.Local);
        }

        bool _isCustom;
        public bool IsCustom
        {
            get
            {
                return _isCustom;
            }
            set
            {
                if (_isCustom != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeMaterialIsCustomCommand(this, _isCustom, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isCustom = value;
                    NotifyPropertyChanged("IsCustom");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isEdited;
        public bool IsEdited
        {
            get
            {
                return _isEdited;
            }
            set
            {
                if (_isEdited != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeMaterialIsEditedCommand(this, _isEdited, value);
                    _isEdited = value;
                    NotifyPropertyChanged("IsEdited");
                }

                if (value && !ZPlannerManager.DmlIsEmpty) ZPlannerManager.Dml.IsEdited = true;
            }
        }

        private bool isHidden;
        public bool IsHidden
        {
            get
            {
                return isHidden;
            }
            set
            {
                if (isHidden != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeMaterialIsHiddenCommand(this, isHidden, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isHidden = value;
                    NotifyPropertyChanged("IsHidden");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private bool isFiltered;
        public bool IsFiltered
        {
            get
            {
                return isFiltered;
            }
            set
            {
                if (isFiltered != value)
                {
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isFiltered = value;
                    NotifyPropertyChanged("IsFiltered");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isReadOnly;
        public bool IsReadOnly
        {
            get
            {
                return _isReadOnly;
            }
            set
            {
                if (_isReadOnly != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeMaterialIsReadOnlyCommand(this, _isReadOnly, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isReadOnly = value;
                    NotifyPropertyChanged("IsReadOnly");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }
        
        bool _isFrozen;
        public bool IsFrozen
        {
            get
            {
                return _isFrozen;
            }
            set
            {
                if (_isFrozen != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeMaterialIsFrozenCommand(this, _isFrozen, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isFrozen = value;
                    NotifyPropertyChanged("IsFrozen");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        int _height;
        public int Height
        {
            get
            {
                return _height;
            }
            set
            {
                if (_height != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeMaterialHeightCommand(this, _height, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _height = value;
                    NotifyPropertyChanged("Height");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        int _order;
        public int Order
        {
            get
            {
                return _order;
            }
            set
            {
                if (_order != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeMaterialOrderCommand(this, _order, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _order = value;
                    NotifyPropertyChanged("Order");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }
    }

    class ZLayer : ZEntity, IOrder
    {
        public ZLayer(ZStackup stackup, ZEntity owner)
            : this(stackup, owner, ZLayerType.Signal)
        {
        }

        public ZLayer(ZStackup stackup, ZEntity owner, ZLayerType layerType)
        {
            ID = GenerateID("Layer");
            Stackup = stackup;
            Owner = owner;
            Order = stackup.Layers.Count;
            this.Title = GenerateTitle(Stackup.Layers, "Row");
            LayerParameters = new ZList<ZLayerParameter>(this);

            foreach (var parameter in stackup.Project.Parameters)
            {
                if (parameter.Table == ZTableType.Stackup)
                {
                    ZLayerParameter layerParameter = new ZLayerParameter(parameter, this, layerType);
                    LayerParameters.Add(layerParameter);
                }
            }
        }

        public ZLayer(string id, ZStackup stackup, ZEntity owner)
        {
            ID = id;
            Stackup = stackup;
            Owner = owner;
            LayerParameters = new ZList<ZLayerParameter>(this);
        }

        public ZLayer Clone()
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                ZLayer layer = new ZLayer(GenerateID("Layer"), this.Stackup, this.Owner);
                layer.Title = GenerateTitle(Stackup.Layers, "Row");
                layer.Order = Order;
                layer.Height = Height;
                layer.IsCustom = IsCustom;
                layer.IsEdited = IsEdited;
                layer.IsFrozen = false;
                layer.IsHidden = IsHidden;
                layer.IsReadOnly = IsReadOnly;
                layer.IsMaterialAssigned = IsMaterialAssigned;

                foreach (ZLayerParameter layerParameter in LayerParameters)
                {
                    ZLayerParameter clonedLayerParameter = layerParameter.Clone();
                    clonedLayerParameter.Layer = layer;
                    layer.LayerParameters.Add(clonedLayerParameter);
                }

                return layer;
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public bool Compare(ZLayer otherLayer)
        {
            if (otherLayer == null) return false;

            foreach (ZLayerParameter layerParameter in LayerParameters)
            {
                ZLayerParameter otherLayerParameter = otherLayer.LayerParameters.Find(x => x.ID == layerParameter.ID);
                if (otherLayerParameter == null || layerParameter.Value != otherLayerParameter.Value) return false;
            }

            return true;
        }

        public ZLayerType? GetLayerType()
        {
            ZLayerType? layerType = null;

            ZLayer layer = Stackup.Layers.Find(x => x.ID == this.ID) ?? this;
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDLayerType);
                if (layerParameter != null && layerParameter.Value != null && LayerTypeDictionary.ContainsKey(layerParameter.Value))
                {
                    layerType = LayerTypeDictionary[layerParameter.Value];
                }
            }

            return layerType;
        }

        // Copper layer number
        public string GetLayerNumber()
        {
            string layerNumber = string.Empty;

            ZLayer layer = Stackup.Layers.Find(x => x.ID == this.ID);
            if (layer != null)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDLayerNumber);
                if (layerParameter != null && layerParameter.Value != null)
                {
                    layerNumber = layerParameter.Value;
                }
            }

            return layerNumber;
        }

        public double GetLayerThickness()
        {
            double layerThickness = 0;

            ZLayer layer = Stackup.Layers.Find(x => x.ID == this.ID);
            if (layer != null)
            {
                string thicknss = string.Empty;
                ZLayerParameter layerParameter;
                if (ZPlannerManager.StackupPanel != null && ZPlannerManager.IsPressedThickness)
                { 
                    layerParameter = layer.LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDPrepregThickness);
                    if (layerParameter != null && !string.IsNullOrWhiteSpace(layerParameter.Value)) thicknss = layerParameter.Value;
                }
                if (thicknss == string.Empty)
                {
                    layerParameter = layer.LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDThickness);
                    if (layerParameter != null && !string.IsNullOrWhiteSpace(layerParameter.Value)) thicknss = layerParameter.Value;
                }

                double dVal;
                if (thicknss != string.Empty && double.TryParse(thicknss, NumberStyles.Any, CultureInfo.InvariantCulture, out dVal))
                {
                    layerThickness = dVal;
                }
            }

            return layerThickness;
        }

        public string GetLayerCopperCoverage(string format = null)
        {
            string value = string.Empty;
            ZLayer layer = this; //Stackup.GetLayerOfStackup(ID);
            if (layer == null) return string.Empty;

            ZLayerType? layerType = layer.GetLayerType();
            if (layerType == ZLayerType.Plane || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed)
            {
                switch (layer.Stackup.CopperCoverageType)
                {
                    case ZCopperCoverageType.PrepregPercent:
                        //value = string.Empty;
                        break;
                    case ZCopperCoverageType.PrepregProportional:
                        if (layerType == ZLayerType.Plane) value = Stackup.ForPlane.ToString(ZPlannerManager.GetFormatByParameterID(ZStringConstants.ParameterIDCopperPercent), CultureInfo.InvariantCulture);
                        else if (layerType == ZLayerType.Signal) value = Stackup.ForSignal.ToString(ZPlannerManager.GetFormatByParameterID(ZStringConstants.ParameterIDCopperPercent), CultureInfo.InvariantCulture);
                        else if (layerType == ZLayerType.SplitMixed) value = Stackup.ForMixed.ToString(ZPlannerManager.GetFormatByParameterID(ZStringConstants.ParameterIDCopperPercent), CultureInfo.InvariantCulture);
                        break;
                    case ZCopperCoverageType.ManuallyEntered:
                    case ZCopperCoverageType.RestoredOnImport:
                        double dval;
                        if (layer.GetLayerParameterValue(ZStringConstants.ParameterIDCopperPercent, out dval))
                        {
                            value = (format != null) 
                            ? dval.ToString(format)
                            : dval.ToString();
                        }
                        else
                        {
                            if (layerType == ZLayerType.Plane) value = Stackup.ForPlane.ToString(ZPlannerManager.GetFormatByParameterID(ZStringConstants.ParameterIDCopperPercent), CultureInfo.InvariantCulture);
                            else if (layerType == ZLayerType.Signal) value = Stackup.ForSignal.ToString(ZPlannerManager.GetFormatByParameterID(ZStringConstants.ParameterIDCopperPercent), CultureInfo.InvariantCulture);
                            else if (layerType == ZLayerType.SplitMixed) value = Stackup.ForMixed.ToString(ZPlannerManager.GetFormatByParameterID(ZStringConstants.ParameterIDCopperPercent), CultureInfo.InvariantCulture);
                        }
                        break;
                }
            }

            return value;
        }

        internal ZMaterial GetMaterialFromLayer()
        {
            ZLayer layer = Stackup.GetLayerOfStackup(this.ID);
            if (layer == null) return null;

            ZLayerType? layerType;
            if (!layer.isDielectric(out layerType)) return null;

            bool isIgnoreCommands = ZPlannerManager.Commands.SuspendCommandEvent();

            List<string> layerParametersToBeIgnored = new List<string>(new string[] { ZStringConstants.ParameterIDComments });
            ZMaterial material;

            try
            {
                material = new ZMaterial((ZLayerType)layerType);
                Dictionary<ZMaterialParameter, string> newValueByMaterialParameter = new Dictionary<ZMaterialParameter, string>();


                //newValueByMaterialParameter.Add(material.GetMaterialParameter(ZStringConstants.DMLParameterIDMaterial), layer.GetLayerParameterValue(ZStringConstants.ParameterIDMaterial));
                newValueByMaterialParameter.Add(material.GetMaterialParameter(ZStringConstants.DMLParameterIDNormalizedConstruction), layer.GetLayerParameterValue(ZStringConstants.ParameterIDConstruction));
                string thickness = layer.GetLayerParameterValue(ZStringConstants.ParameterIDOriginThickness);
                if (string.IsNullOrWhiteSpace(thickness)) thickness = layer.GetLayerParameterValue(ZStringConstants.ParameterIDThickness);
                newValueByMaterialParameter.Add(material.GetMaterialParameter(ZStringConstants.DMLParameterIDH), thickness);
                //newValueByMaterialParameter.Add(material.GetMaterialParameter(ZStringConstants.DMLParameterIDWarpYarnCount), layer.GetLayerParameterValue(ZStringConstants.ParameterIDWarpYarnCount));
                //newValueByMaterialParameter.Add(material.GetMaterialParameter(ZStringConstants.DMLParameterIDFillYarnCount), layer.GetLayerParameterValue(ZStringConstants.ParameterIDFillYarnCount));
                //newValueByMaterialParameter.Add(material.GetMaterialParameter(ZStringConstants.DMLParameterIDManufacturer), layer.GetLayerParameterValue(ZStringConstants.ParameterIDManufacturer));
                newValueByMaterialParameter.Add(material.GetMaterialParameter(ZStringConstants.DMLParameterIDResin), layer.GetLayerParameterValue(ZStringConstants.ParameterIDResinContent));

                newValueByMaterialParameter.Add(material.GetMaterialParameter(ZStringConstants.DMLParameterIDDk), layer.GetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant));
                newValueByMaterialParameter.Add(material.GetMaterialParameter(ZStringConstants.DMLParameterIDDf), layer.GetLayerParameterValue(ZStringConstants.ParameterIDLossTangent));

                foreach (ZMaterialParameter mParameter in material.MaterialParameters)
                {
                    if (layerParametersToBeIgnored.Contains(mParameter.ID)) continue;
                    ZLayerParameter layerParameter = layer.GetLayerParameter(mParameter.ID);
                    if (layerParameter != null && mParameter != null && !newValueByMaterialParameter.Keys.Contains(mParameter)) newValueByMaterialParameter.Add(mParameter, layerParameter.Value);
                }

                foreach (ZMaterialParameter mParameter in newValueByMaterialParameter.Keys)
                {
                    mParameter.Value = newValueByMaterialParameter[mParameter] ?? string.Empty;
                }

                //material.IsFiltered = !ZPlannerManager.MainMenu.IsLocalLibraryEnabled();
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnoreCommands);
            }

            return material;
        }

        public bool isDummyCore()
        {
            string par = GetLayerParameterValue(ZStringConstants.ParameterIDLayerTypeDummyCore);
            if (!string.IsNullOrWhiteSpace(par))
            {
                bool bval;
                if (Boolean.TryParse(par, out bval)){
                    return bval;
                }
            }
            return false;
        }


        public bool isMetal()
        {
            ZLayerType? layerType;
            return isMetal(out layerType); 
        }

        public bool isMetal(out ZLayerType? layerType)
        {
            var metals = new List<ZLayerType?> { ZLayerType.Signal, ZLayerType.Plane, ZLayerType.SplitMixed };
            layerType = GetLayerType();
            return metals.Contains(layerType);
        }

        public bool isDielectric()
        {
            ZLayerType? layerType;
            return isDielectric(out layerType);
        }

        public bool isDielectric(out ZLayerType? layerType)
        {
            var dielectrics = new List<ZLayerType?> { ZLayerType.Core, ZLayerType.Prepreg, ZLayerType.SolderMask };
            layerType = GetLayerType();
            return dielectrics.Contains(layerType);
        }

        public bool isCoreOrPrepreg(out ZLayerType? layerType)
        {
            var dielectrics = new List<ZLayerType?> { ZLayerType.Core, ZLayerType.Prepreg };
            layerType = GetLayerType();
            return dielectrics.Contains(layerType);
        }

        internal bool HasValues()
        {
            bool hasValues = false;
            foreach (ZLayerParameter layerParameter in this.LayerParameters)
            {
                if (!string.IsNullOrEmpty(layerParameter.Value))
                {
                    hasValues = true;
                    break;
                }
            }

            return hasValues;
        }

        public class PlaneReferences
        {
            public List<ZLayer> refsUp;
            public List<ZLayer> refsDown;
            public ZLayer defUp, defDown;

            public PlaneReferences()
            {
                refsUp = new List<ZLayer>();
                refsDown = new List<ZLayer>();
                defUp = null;
                defDown = null;
            }
        }

        public bool GetPlaneReferences(ref PlaneReferences Refs)
        {
            if (!isMetal()) return false;
            if (GetLayerType() == ZLayerType.Plane) return false;


            Refs.defUp = null;
            Refs.defDown = null;

            bool bUp = true;

            foreach (ZLayer l in Stackup.Layers)
            {
                if (l.ID == ID)
                {
                    bUp = false;
                    continue;
                }

                switch (l.GetLayerType())
                {
                    case ZLayerType.Plane:
                    case ZLayerType.SplitMixed:
                        if (bUp)
                        {
                            Refs.refsUp.Add(l);
                        }
                        else
                        {
                            Refs.refsDown.Add(l);
                        }
                        break;
                }
            }
            int upCnt =Refs.refsUp.Count; 
            if (upCnt > 0){
                Refs.defUp = Refs.refsUp[upCnt - 1]; // the last
            }
            if (Refs.refsDown.Count > 0){
                Refs.defDown = Refs.refsDown[0];
            }            
            
            return true;
        }

        public bool GetReferences(out ZLayer lUp, out ZLayer lDown)
        {
            lUp = null;
            lDown = null;

            bool bUp = true;
            if (GetLayerType() == ZLayerType.Signal || GetLayerType() == ZLayerType.SplitMixed)
            {
                foreach (ZLayer l in Stackup.Layers)
                {
                    if (l.ID == ID)
                    {
                        bUp = false;
                        continue;
                    }
                    bool bRef = false;
                    switch (l.GetLayerType())
                    {
                        case ZLayerType.Plane:
                            bRef = true;
                            break;
                        case ZLayerType.SplitMixed:
                            string planeRef = "";
                            if (ZPlannerProject.GetLayerParameterValue(l, ZStringConstants.ParameterIDPlaneReference, ref planeRef))
                            {
                                if (planeRef == ZStringConstants.PlaneReference)
                                {
                                    bRef = true;
                                }
                            }
                            break;
                    }
                    if (bRef)
                    {
                        if (bUp)
                        {
                            lUp = l;
                        }
                        else
                        {
                            lDown = l;
                            break;
                        }
                    }
                }
            }
            return lUp != null || lDown != null;
        }

        internal bool HasValues(string[] arrayOfLayerParameterID)
        {
            bool hasValues = false;
            foreach (ZLayerParameter layerParameter in this.LayerParameters)
            {
                if (arrayOfLayerParameterID.Contains(layerParameter.ID))
                {
                    if (!string.IsNullOrEmpty(layerParameter.Value))
                    {
                        hasValues = true;
                        break;
                    }
                }
            }

            return hasValues;
        }

        internal ZLayerParameter GetLayerParameter(string parameterName)
        {
            ZLayerParameter layerParameter = LayerParameters.Find(x => x.ID == parameterName);
            System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, Owner.GetType().Name));
            return layerParameter;
        }

        internal string GetLayerParameterValue(string parameterName)
        {
            ZLayerParameter layerParameter = LayerParameters.Find(x => x.ID == parameterName);
            System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, Owner.GetType().Name));
            if (layerParameter != null)
            {
                return layerParameter.Value;
            }

            return null;
        }

        internal bool GetLayerParameterValue(string parameterName, out double value)
        {
            value = double.NaN;
            ZLayerParameter layerParameter = LayerParameters.Find(x => x.ID == parameterName);
            System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, Owner.GetType().Name));
            if (layerParameter != null && !string.IsNullOrWhiteSpace(layerParameter.Value))
            {
                return double.TryParse(layerParameter.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out value);
            }

            return false;
        }

        internal void SetLayerParameterValue(string parameterName, string value, bool isEdited = false)
        {
            ZLayerParameter layerParameter = LayerParameters.Find(x => x.ID == parameterName);
            System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, Owner.GetType().Name));
            if (layerParameter != null)
            {
                if (isEdited) layerParameter.SetEditedValue(value);
                else layerParameter.Value = value;
            }
        }

        internal void SetDefaultLayerParameterValue(string parameterName, bool isEdited = false)
        {
            ZLayerType? layerType = GetLayerType();
            ZLayerParameter layerParameter = LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDLayerType);
            if (layerParameter != null && layerParameter.Value != null && LayerTypeDictionary.ContainsKey(layerParameter.Value))
            {
                layerType = LayerTypeDictionary[layerParameter.Value];
            }
            else
            {
                layerType = null;
            }

            SetDefaultLayerParameterValue(parameterName, layerType, isEdited);
        }

        internal void SetDefaultLayerParameterValue(string parameterName, ZLayerType? layerType, bool isEdited = false)
        {
            ZLayerParameter layerParameter = LayerParameters.Find(x => x.ID == parameterName);
            System.Diagnostics.Debug.Assert(layerParameter != null, string.Format("{0} layer parameter is missing in {1}", parameterName, Owner.GetType().Name));
            if (layerParameter != null)
            {
                string value = (layerType == null) ? string.Empty : layerParameter.GetDefaulLayerParameterValue((ZLayerType)layerType);
                if (isEdited) layerParameter.SetEditedValue(value);
                else layerParameter.Value = value;
            }
        }

        //calculates Dk(f), where f is the current layer frequency
        internal bool GetDk(out double val)
        {
            val = Options.TheOptions.Dk;

            ZLayerParameter layerParameter = LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDZo_DielectricConstant || x.ID == ZStringConstants.ParameterIDZdiff_DielectricConstant);//table function
            if (layerParameter != null)
            {
                if (ZPlannerManager.TryParseDependedValue(layerParameter.Value, GetFrequency(), out val))
                {
                    if (double.IsNaN(val)) return false;
                    return true;
                }
            }

            return false;
        }

        //calculates Df(f), where f is the current layer frequency
        internal bool GetDf(out double val)
        {
            val = Options.TheOptions.Df;

            ZLayerParameter layerParameter = LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDZo_LossTangent || x.ID == ZStringConstants.ParameterIDZdiff_LossTangent);//table function
            if (layerParameter != null)
            {
                if (ZPlannerManager.TryParseDependedValue(layerParameter.Value, GetFrequency(), out val))
                {
                    if (double.IsNaN(val)) return false;
                    return true;
                }
            }

            return false;
        }

        public void CopyLayerParameter(string id)
        {
            ZPlannerManager.Clipboard.Clear();

            // Stackup Layer.
            ZLayerParameter stackupLayerParameter = this.LayerParameters.Find(x => x.ID == id);
            if (stackupLayerParameter != null) throw new IndexOutOfRangeException("The row with specified id was not found.");
            List<ZEntity> stackupLayerParameters = new List<ZEntity>();
            stackupLayerParameters.Add(stackupLayerParameter.Clone());
            ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeyStackupLayerParameters, stackupLayerParameters);
        }

        public static string GetLayerTypeString(ZLayerType? layerType)
        {
            if (layerType == null) return string.Empty;

            return GetLayerTypeString((ZLayerType)layerType);
        }

        public static string GetLayerTypeString(ZLayerType layerType)
        {
            switch (layerType)
            {
                case ZLayerType.Core: return ZStringConstants.LayerTypeCore;
                case ZLayerType.Prepreg: return ZStringConstants.LayerTypePrepreg;
                case ZLayerType.SolderMask: return ZStringConstants.LayerTypeSolderMask;
                case ZLayerType.Signal: return ZStringConstants.LayerTypeSignal;
                case ZLayerType.SplitMixed: return ZStringConstants.LayerTypeSplitMixed;
                case ZLayerType.Plane: return ZStringConstants.LayerTypePlane;
            }

            return string.Empty;
        }

        public static Dictionary<string, ZLayerType> LayerTypeDictionary = new Dictionary<string, ZLayerType>();
        static ZLayer()
        {
            LayerTypeDictionary.Add("Core", ZLayerType.Core);
            LayerTypeDictionary.Add("Prepreg", ZLayerType.Prepreg);
            LayerTypeDictionary.Add("Solder Mask", ZLayerType.SolderMask);
            LayerTypeDictionary.Add("Plane", ZLayerType.Plane);
            LayerTypeDictionary.Add("Signal", ZLayerType.Signal);
            LayerTypeDictionary.Add("Split/Mixed", ZLayerType.SplitMixed);
        }

        public ZStackup Stackup { get; private set; }

        public ZEntity Owner { get; private set; }

        public ZList<ZLayerParameter> LayerParameters { get; private set; }

        bool _isCustom;
        public bool IsCustom
        {
            get
            {
                return _isCustom;
            }
            set
            {
                if (_isCustom != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerIsCustomCommand(this, _isCustom, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isCustom = value;
                    NotifyPropertyChanged("IsCustom");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isEdited;
        public bool IsEdited
        {
            get
            {
                return _isEdited;
            }
            set
            {
                if (_isEdited != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerIsEditedCommand(this, _isEdited, value);
                    _isEdited = value;
                    NotifyPropertyChanged("IsEdited");
                }
            }
        }

        private bool isHidden;
        public bool IsHidden
        {
            get
            {
                return isHidden;
            }
            set
            {
                if (isHidden != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerIsHiddenCommand(this, isHidden, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isHidden = value;
                    NotifyPropertyChanged("IsHidden");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isReadOnly;
        public bool IsReadOnly
        {
            get
            {
                return _isReadOnly;
            }
            set
            {
                if (_isReadOnly != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerIsReadOnlyCommand(this, _isReadOnly, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isReadOnly = value;
                    NotifyPropertyChanged("IsReadOnly");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isFrozen;
        public bool IsFrozen
        {
            get
            {
                return _isFrozen;
            }
            set
            {
                if (_isFrozen != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerIsFrozenCommand(this, _isFrozen, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isFrozen = value;
                    NotifyPropertyChanged("IsFrozen");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isMaterialAssigned;
        public bool IsMaterialAssigned
        {
            get
            {
                return _isMaterialAssigned;
            }
            set
            {
                if (_isMaterialAssigned != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerIs_isMaterialAssignedCommand(this, _isMaterialAssigned, value);
                    //bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isMaterialAssigned = value;
                    //NotifyPropertyChanged("IsMaterialAssignedn");
                    //ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        int _height;
        public int Height
        {
            get
            {
                return _height;
            }
            set
            {
                if (_height != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerHeightCommand(this, _height, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _height = value;
                    NotifyPropertyChanged("Height");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        int _order;
        public int Order
        {
            get
            {
                return _order;
            }
            set
            {
                if (_order != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerOrderCommand(this, _order, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _order = value;
                    NotifyPropertyChanged("Order");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        public double GetFrequency()
        {
            string stringValue = GetLayerParameterValue(ZStringConstants.ParameterIDZo_Frequency) ?? GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_Frequency);
            double doubleValue;
            if (string.IsNullOrWhiteSpace(stringValue) || !double.TryParse(stringValue, NumberStyles.Any, CultureInfo.InvariantCulture, out doubleValue)) return Stackup.GetFrequencyByLayer(this);
            return doubleValue;
        }

        public bool isGround()
        {
            //first check layer parameter
            string stringValue = GetLayerParameterValue(ZStringConstants.ParameterIDPlaneType);
            if (stringValue != null && stringValue.Length > 0 && stringValue == ZStringConstants.LayerTypePlaneGround)
            {
                return true;
            }

            string[] substrings = {"GND", "GROUND"};
            string layerName = GetLayerParameterValue(ZStringConstants.ParameterIDLayerName).ToUpper();
            foreach (string substring in substrings)
            {
                if (layerName.IndexOf(substring) >= 0)
                    return true;
            }
            return false; // not found
        }

        public bool isPower()
        {
            //first check layer parameter
            string stringValue = GetLayerParameterValue(ZStringConstants.ParameterIDPlaneType);
            if (stringValue != null && stringValue.Length > 0 && stringValue == ZStringConstants.LayerTypePlanePower)
            {
                return true;
            }

            string[] substrings = { "VDD", "VCC", "VSS" };
            string layerName = GetLayerParameterValue(ZStringConstants.ParameterIDLayerName).ToUpper();
            foreach (string substring in substrings)
            {
                if (layerName.IndexOf(substring) >= 0)
                    return true;
            }
            return false; // not found
        }
    }

    class ZLayerParameter : ZEntity
    {
        public ZLayerParameter(ZParameter parameter, ZLayer layer)
        {
            ID = parameter.ID;
            Parameter = parameter;
            Layer = layer;
        }

        public ZLayerParameter(ZParameter parameter, ZLayer layer, ZLayerType layerType)
        {
            ID = parameter.ID;
            Parameter = parameter;
            Layer = layer;
            Value = GetDefaulLayerParameterValue(layerType);
        }

        public ZLayerParameter(string id)
        {
            ID = id;
        }

        public ZLayerParameter Clone()
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                ZLayerParameter layerParameter = new ZLayerParameter(ID);
                layerParameter.IsEdited = IsEdited;
                layerParameter.Layer = Layer;
                layerParameter.Parameter = Parameter;
                layerParameter.Title = Title;
                layerParameter.Value = Value;

                return layerParameter;
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public ZParameter Parameter { get; set; }

        public ZLayer Layer { get; set; }

        string _value;
        internal string InternalValue { set { _value = value; } }
        public string Value 
        { 
            get 
            {
                ZPlannerManager.StatusMenu.UpdateProgress();
                return _value; 
            }
            set 
            {
                ZPlannerManager.StatusMenu.UpdateProgress();
                if (value != _value) 
                {
                    //if (IsReadOnly(true)) { _value = string.Empty; return; }

                    if (!ZPlannerManager.Commands.IsIgnoreCommands && ID == ZStringConstants.ParameterIDLayerType && Layer != null && Layer.Stackup != null)
                    {
                        Layer.Stackup.ChangeLayerType(this, value);                        
                    }
                    else if (!ZPlannerManager.Commands.IsIgnoreCommands && ID == ZStringConstants.ParameterIDPlaneReference && Layer != null && Layer.Stackup != null && Layer.GetLayerType() == ZLayerType.Signal && value == ZStringConstants.PlaneReference)
                    {
                        Layer.Stackup.ChangeSignalLayerReference(this, value);
                    }
                    else if (!ZPlannerManager.Commands.IsIgnoreCommands && ID == ZStringConstants.ParameterIDPlaneReference && Layer != null && Layer.Stackup != null && Layer.GetLayerType() == ZLayerType.Plane && value == ZStringConstants.NonPlaneReference)
                    {
                        Layer.Stackup.ChangePlaneLayerReference(this, value);
                    }
                    else
                    {
                        if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerParameterValueCommand(this, _value, value, IsEdited, false);
                        bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                        _value = value;
                        NotifyPropertyChanged("Value");
                        IsEdited = false;
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }
                }
            }
        }

        public bool NeedToUpdate(string value)
        {
            return (value != _value);
        }

        public void SetEditedValue(string value)
        {
            if (ID == ZStringConstants.ParameterIDCopperThickness && Parameter != null && Parameter.List != null)
            {
                value = Parameter.List.AddValue(value);

                if (!ZPlannerManager.ProjectIsEmpty)
                {
                    SelectList listCopperThickness = ZPlannerManager.Project.Lists.Find(x => x.ID == ZStringConstants.ListIDCopperThickness);
                    if (listCopperThickness != null) listCopperThickness.AddValue(value);
                }
            }

            if (ID == ZStringConstants.ParameterIDFoilTreatment && Parameter != null && Parameter.List != null && !Parameter.List.GetValues().Contains(value))
            {
                value = Parameter.List.AddValue(value);

                if (!ZPlannerManager.ProjectIsEmpty)
                {
                    SelectList listCopperFoil = ZPlannerManager.Project.Lists.Find(x => x.ID == ZStringConstants.ListIDCopperFoil);
                    if (listCopperFoil != null) listCopperFoil.AddValue(value);
                }
            }

            if (value != _value)
            {
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerParameterValueCommand(this, _value, value, IsEdited, true);
                bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                _value = value;
                NotifyPropertyChanged("Value");
                IsEdited = true;
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        public void SetUnEditedValue(string value)
        {
            if (NeedToUpdate(value))
            {
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerParameterValueCommand(this, _value, value, IsEdited, false);
                bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                _value = value;
                NotifyPropertyChanged("Value");
                IsEdited = false;
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        public bool IsReadOnly(bool isIgnoreLayerAndParameter = false, bool isIgnoreMaterialAssigned = false, bool isIgnoreCustomParameters = false)
        {
            if (Layer == null || Parameter == null) return false;
            if (isIgnoreCustomParameters && Parameter.IsCustom) return false;
            ZLayerType? layerTypeOrNull = Layer.GetLayerType();
            if (layerTypeOrNull == null) return false;
            ZLayerType layerType = (ZLayerType)layerTypeOrNull;
            switch (this.ID)
            {
                case ZStringConstants.ParameterIDManufacturer:
                case ZStringConstants.ParameterIDConstruction:
                case ZStringConstants.ParameterIDResinContent:
                    return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || !(layerType == ZLayerType.Core || layerType == ZLayerType.Prepreg || layerType == ZLayerType.SolderMask) || Layer.IsMaterialAssigned && !isIgnoreMaterialAssigned;
                case ZStringConstants.ParameterIDZo_DielectricConstant:
                case ZStringConstants.ParameterIDZdiff_DielectricConstant:
                case ZStringConstants.ParameterIDZo_LossTangent:
                case ZStringConstants.ParameterIDZdiff_LossTangent:
                    return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || (layerType == ZLayerType.Plane) || Layer.IsMaterialAssigned && !isIgnoreMaterialAssigned;
                //return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || !(layerType == ZLayerType.Core || layerType == ZLayerType.Prepreg || layerType == ZLayerType.SolderMask) || Layer.IsMaterialAssigned && !isIgnoreMaterialAssigned;
                case ZStringConstants.ParameterIDWarpYarnCount:
                case ZStringConstants.ParameterIDFillYarnCount:
                    return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || !(layerType == ZLayerType.Core || layerType == ZLayerType.Prepreg) || Layer.IsMaterialAssigned && !isIgnoreMaterialAssigned;
                case ZStringConstants.ParameterIDCopperThickness:
                case ZStringConstants.ParameterIDFoilTreatment:
                case ZStringConstants.ParameterIDPlaneReference:
                case ZStringConstants.ParameterIDRoughTop:
                case ZStringConstants.ParameterIDRoughBottom:
                case ZStringConstants.ParameterIDZdiff_RoughTop:
                case ZStringConstants.ParameterIDZdiff_RoughBottom:
                    return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || !(layerType == ZLayerType.Plane || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed);
                case ZStringConstants.ParameterIDCopperPercent:
                    return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || !((layerType == ZLayerType.Plane || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed) && Layer.Stackup.CopperCoverageType == ZCopperCoverageType.ManuallyEntered && !ZPlannerManager.IsKeepImportedPressedThickness) && !isIgnoreCustomParameters || Layer.Stackup.CopperCoverageType == ZCopperCoverageType.RestoredOnImport;
                case ZStringConstants.ParameterIDComments:
                    return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter;
                case ZStringConstants.ParameterIDZo_Frequency:
                case ZStringConstants.ParameterIDZdiff_Frequency:
                case ZStringConstants.ParameterIDZo_TopReference:
                case ZStringConstants.ParameterIDZo_BottomReference:
                case ZStringConstants.ParameterIDZdiff_TopReference:
                case ZStringConstants.ParameterIDZdiff_BottomReference:
                    return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || !(layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed);
                case ZStringConstants.ParameterIDZo_InsertionLoss:
                case ZStringConstants.ParameterIDZdiff_InsertionLoss:
                case ZStringConstants.ParameterIDZo_TraceLength:
                case ZStringConstants.ParameterIDZdiff_TraceLength:
                case ZStringConstants.ParameterIDZo_TotalLoss:
                case ZStringConstants.ParameterIDZdiff_TotalLoss:
                case ZStringConstants.ParameterIDZo_LossViewer:
                case ZStringConstants.ParameterIDZdiff_LossViewer:
                case ZStringConstants.ParameterIDZo_PropagationVelocity:
                case ZStringConstants.ParameterIDZdiff_PropagationVelocity:
                case ZStringConstants.ParameterIDZo_PropagationDelay:
                case ZStringConstants.ParameterIDZdiff_PropagationDelay:
                    return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || !(layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed);
                case ZStringConstants.ParameterIDZo_TraceWidth:
                case ZStringConstants.ParameterIDZo_TraceSpacing:
                case ZStringConstants.ParameterIDZdiff_TraceWidth:
                case ZStringConstants.ParameterIDZdiff_IsUsed:
                case ZStringConstants.ParameterIDZo_IsUsed:
                case ZStringConstants.ParameterIDZdiff_TraceSpacing:
                case ZStringConstants.ParameterIDZo_Zo:
                case ZStringConstants.ParameterIDZdiff_Zo:
                case ZStringConstants.ParameterIDZdiff_Zdiff:
                    return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || !(layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed);
                case ZStringConstants.ParameterIDThickness:
                    return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || (layerType == ZLayerType.Prepreg && ZPlannerManager.StackupPanel != null && ZPlannerManager.IsPressedThickness) || Layer.IsMaterialAssigned && !isIgnoreMaterialAssigned;
                case ZStringConstants.ParameterIDFillPitch:
                case ZStringConstants.ParameterIDWeavePitch:
                case ZStringConstants.ParameterIDZdiff_FillPitch:
                case ZStringConstants.ParameterIDZdiff_WeavePitch:
                    return ((Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || Layer.IsMaterialAssigned && !isIgnoreMaterialAssigned) && !isIgnoreCustomParameters; ;
            }

            return (Layer.IsReadOnly || Parameter.IsReadOnly) && !isIgnoreLayerAndParameter || Layer.IsMaterialAssigned && !isIgnoreMaterialAssigned;
        }

        bool _isEdited;
        public bool IsEdited
        {
            get
            {
                return _isEdited;
            }
            set
            {
                if (value != _isEdited)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerParameterIsEditedCommand(this, _isEdited, value);
                    _isEdited = value;
                    NotifyPropertyChanged("IsEdited");
                }
            }
        }

        public string GetDefaulLayerParameterValue(ZLayerType layerType) //? 
        {
            if (this.Layer.Stackup == null) return string.Empty;

            int layerCount = this.Layer.Stackup.GetMetallLayerCount();
            string[] materials = new string[] { ZStringConstants.DefaultDielectricMaterial, ZStringConstants.DefaultDielectricMaterial, ZStringConstants.DefaultDielectricMaterial, 
                ZStringConstants.DefaultCopperMaterial, ZStringConstants.DefaultCopperMaterial, ZStringConstants.DefaultCopperMaterial };
            //string[] resincontent = new string[] { "40", "56", "", "", "", "" };
            //string[] dielectricconstant = new string[] { "3.7", "3.7", "3.3", "", "", "" };

            switch(Parameter.ID)
            {
                case ZStringConstants.ParameterIDViaSpan:
                    return string.Empty;
                case ZStringConstants.ParameterIDLayerNumber:
                    return (layerType == ZLayerType.Plane || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed) ? (layerCount + 1).ToString(CultureInfo.InvariantCulture) : string.Empty;
                case ZStringConstants.ParameterIDLayerType:
                    return ZStringConstants.LayerType[(int)layerType];
                case ZStringConstants.ParameterIDPlaneReference:
                    switch (layerType)
                    {
                        case ZLayerType.Plane:
                            return ZStringConstants.PlaneReference;
                        case ZLayerType.Signal:
                            return ZStringConstants.NonPlaneReference;
                        case ZLayerType.SplitMixed:
                            return ZStringConstants.PlaneReference;
                        default: return ZStringConstants.PlaneReferenceNA;
                    }
                    
                case ZStringConstants.ParameterIDLayerName:
                    return string.Empty;
                case ZStringConstants.ParameterIDMaterial:
                    return materials[(int)layerType];
                case ZStringConstants.ParameterIDConstruction:
                    return string.Empty;
                case ZStringConstants.ParameterIDResinContent:
                    return ""; //resincontent[(int)layerType]; //#185: Generic Stackups should not include resin content
                case ZStringConstants.ParameterIDThickness:
                        switch (layerType){
                            case ZLayerType.Plane:
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                return ZPlannerManager.GetThicknesStringByCopperWeight(Options.TheOptions.copper_foil_thickness); //oz inner layer

                            //dielectric
                            case ZLayerType.SolderMask: 
                                return Options.TheOptions.soldermask_height.ToString();
                            case ZLayerType.Prepreg:
                            case ZLayerType.Core:
                                return Options.TheOptions.height.ToString();
                        }
                        break;
                case ZStringConstants.ParameterIDCopperThickness:
                        /*switch (layerType)
                        {
                            case ZLayerType.Plane:
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                return Options.TheOptions.copper_foil_thickness.ToString(); //oz inner layer

                            //dielectric
                            default:*/
                                return string.Empty;
                        //}
                case ZStringConstants.ParameterIDCopperPercent:
                        /*switch (layerType){
                            case ZLayerType.Plane:
                                return Layer.Stackup.ForPlane.ToString();
                            case ZLayerType.Signal:
                                return Layer.Stackup.ForSignal.ToString();
                            case ZLayerType.SplitMixed:
                                return Layer.Stackup.ForMixed.ToString();

                            //dielectric
                            default:*/
                                return string.Empty;
                        //}
                case ZStringConstants.ParameterIDDielectricConstant:
                case ZStringConstants.ParameterIDZo_DielectricConstant:
                case ZStringConstants.ParameterIDZdiff_DielectricConstant:
                        switch (layerType)
                        {
                            //dielectric
                            case ZLayerType.SolderMask: 
                                return Options.TheOptions.soldermask_Dk.ToString();
                            case ZLayerType.Prepreg:
                            case ZLayerType.Core:
                                return Options.TheOptions.Dk.ToString();
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                return Options.TheOptions.resin_Dk.ToString();
                            default:
                                return "";
                        }
                        //break;

                    //-- return dielectricconstant[(int)layerType];
                case ZStringConstants.ParameterIDLossTangent:
                case ZStringConstants.ParameterIDZo_LossTangent:
                case ZStringConstants.ParameterIDZdiff_LossTangent:
                        switch (layerType)
                        {
                            //dielectric
                            case ZLayerType.SolderMask:
                                return Options.TheOptions.soldermask_Df.ToString();
                            case ZLayerType.Prepreg:
                            case ZLayerType.Core:
                                return Options.TheOptions.Df.ToString();
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                return Options.TheOptions.resin_Df.ToString();
                            default:
                                return "";
                        }
                case ZStringConstants.ParameterIDZo_TraceWidth:
                        switch (layerType){
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                return Options.TheOptions.inner_trace_width.ToString();  // inner layer
                            default:
                                return "";
                        }

                    //-- return "14";
                case ZStringConstants.ParameterIDZo_TraceSpacing:
                        switch (layerType)
                        {
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                return Options.TheOptions.trace_spacing.ToString();

                            default:
                                return "";
                        }

                case ZStringConstants.ParameterIDZo_Zo: 
                    return "";
                    //return "25";
                case ZStringConstants.ParameterIDZdiff_TraceWidth:
                    switch (layerType)
                    {
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            return Options.TheOptions.inner_diff_trace_width.ToString();    // inner layer

                        default:
                            return "";
                    }
                case ZStringConstants.ParameterIDZdiff_IsUsed:
                case ZStringConstants.ParameterIDZo_IsUsed:
                    switch (layerType)
                    {
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            return "true";    // inner layer

                        default:
                            return "false";
                    }
                //--return "14";
                case ZStringConstants.ParameterIDZdiff_TraceSpacing:
                    switch (layerType)
                    {
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            return Options.TheOptions.inner_diff_trace_spacing.ToString();  // inner layer

                        default:
                            return "";
                    }
                case ZStringConstants.ParameterIDZdiff_TracePitch:
                    switch (layerType)
                    {
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            return (Options.TheOptions.inner_diff_trace_spacing + Options.TheOptions.inner_diff_trace_width).ToString();    // inner layer

                        default:
                            return "";
                    }
                //--return "10";
                case ZStringConstants.ParameterIDBulkRes:
                    switch (layerType)
                    {
                        case ZLayerType.Plane:
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            return Options.TheOptions.bulkRes.ToString();// "1.724e-008";//copper

                        default:
                            return "";
                    }
                case ZStringConstants.ParameterIDZdiff_Zo:
                    return "";    
                //return "25";
                case ZStringConstants.ParameterIDZdiff_Zdiff:
                    return "";
                    //return "89.48";
                
                //etch
                case ZStringConstants.ParameterIDEtchFactor:
                    switch (layerType)
                    {
                        case ZLayerType.Plane:
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            return Layer.Stackup.Etch.ToString();
                        default:
                            return "";
                    }
                /*case ZStringConstants.ParameterIDRoughTop:
                case ZStringConstants.ParameterIDRoughBottom:
                    switch (layerType)
                    {
                        case ZLayerType.Plane:
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            return Layer.Stackup.OuterLayerRoughness.ToString();

                        default:
                            return "";
                    }*/
                case ZStringConstants.ParameterIDCalcRoughTop:
                case ZStringConstants.ParameterIDCalcRoughBottom:
                    switch (layerType)
                    {
                        case ZLayerType.Plane:
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            return Layer.Stackup.OuterLayerRoughness.ToString();

                        default:
                            return "";
                    }
                //...
                default:
                    return string.Empty;
            }
            return "";
        }
    }

    class ZMaterialParameter : ZEntity
    {
        public ZMaterialParameter(string value, ZParameter parameter, ZMaterial material)
        {
            //ID = parameter.ID;
            Parameter = parameter;
            Parameter.MaterialParameters.Add(this);
            Material = material;
            Value = value;
        }

        public ZMaterialParameter(string id)
        {
            ID = id;
        }

        public ZMaterialParameter(ZParameter parameter, ZMaterial material, ZLayerType type)
        {
            ID = parameter.ID;
            Parameter = parameter;
            Material = material;
            Value = GetDefaulMaterialParameterValue(type);
        }

        public ZMaterialParameter Clone()
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                ZMaterialParameter materialParameter = new ZMaterialParameter(ID);
                materialParameter.IsEdited = true; //IsEdited;
                materialParameter.Material = Material;
                materialParameter.Parameter = Parameter;
                materialParameter.Title = Title;
                materialParameter.Value = Value;

                return materialParameter;
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }


        public ZParameter Parameter { get; set; }
        public ZMaterial Material { get; set; }
        
        string _value;
        public string Value
        {
            get 
            {
                ZPlannerManager.StatusMenu.UpdateProgress();
                return _value; 
            }
            set 
            {
                ZPlannerManager.StatusMenu.UpdateProgress();
                if (value != _value) 
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeMaterialParameterValueCommand(this, _value, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _value = value;
                    NotifyPropertyChanged("Value"); 
                    IsEdited = true;
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    //if (Parameter != null && Parameter.ID == ZStringConstants.DMLParameterIDNormalizedConstruction) ZPlannerManager.MainMenu.UpdateGlassStyles();
                } 
            }
        }

        bool _isEdited;
        public bool IsEdited
        {
            get
            {
                return _isEdited;
            }
            set
            {
                if (value != _isEdited)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeMaterialParameterIsEditedCommand(this, _isEdited, value);
                    _isEdited = value;
                    NotifyPropertyChanged("IsEdited");
                }

                if (value && Material != null) Material.IsEdited = true;
            }
        }

        public string GetDefaulMaterialParameterValue(ZLayerType type) //? 
        {
            switch (Parameter.ID)
            {
                case ZStringConstants.DMLParameterIDCategory:
                    return ZPlannerManager.GetMeterialCategoryStringByUser();
                case ZStringConstants.DMLParameterIDQualified:
                    return "false";
                case ZStringConstants.DMLParameterIDManufacturer:
                    return string.Empty;
                case ZStringConstants.DMLParameterIDMaterial:
                    return string.Empty;
                case ZStringConstants.DMLParameterIDType:
                    return (type == ZLayerType.Core) ? "Core" : (type == ZLayerType.Prepreg) ? "Prepreg" : "Solder Mask";
                case ZStringConstants.DMLParameterIDConstruction:
                    return "106";
                case ZStringConstants.DMLParameterIDNormalizedConstruction:
                    return "106";
                case ZStringConstants.DMLParameterIDResin:
                    return "0";
                case ZStringConstants.DMLParameterIDH:
                    return "0";
                case ZStringConstants.DMLParameterIDDk_1GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDDf_1GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDTg:
                    return "0";
                case ZStringConstants.DMLParameterIDTd:
                    return "0";
                case ZStringConstants.DMLParameterIDHalogenFree:
                    return "false";
                case ZStringConstants.DMLParameterIDPrimaryIPCSlashSheet:
                    return "/26";
                case ZStringConstants.DMLParameterIDZCTE:
                    return "0%";
                case ZStringConstants.DMLParameterIDZCTE_alpha1:
                    return "0";
                case ZStringConstants.DMLParameterIDZCTE_alpha2:
                    return "0";
                case ZStringConstants.DMLParameterIDToleranceH:
                    return "0%";
                case ZStringConstants.DMLParameterIDDk_3GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDDk_5GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDDk_7GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDDk_10GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDDk_20GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDDf_3GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDDf_5GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDDf_7GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDDf_10GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDDf_20GHz:
                    return "0";
                case ZStringConstants.DMLParameterIDHide:
                    return "false";
                case ZStringConstants.DMLParameterIDCost:
                    return "0";
                case ZStringConstants.DMLParameterIDDfType:
                    return string.Empty;
                case ZStringConstants.DMLParameterIDT:
                    return "0";
                case ZStringConstants.DMLParameterIDT260:
                    return "0";
                case ZStringConstants.DMLParameterIDT280:
                    return "0";
                case ZStringConstants.DMLParameterIDWA:
                    return "0%";
                case ZStringConstants.DMLParameterIDDatasheetURL:
                    return string.Empty;
                case ZStringConstants.DMLParameterIDThermalConductivity:
                    return "0";
                case ZStringConstants.DMLParameterIDCAFResistant:
                    return "false";
                case ZStringConstants.DMLParameterIDLaserDrillable:
                    return "false";
                case ZStringConstants.DMLParameterIDYoungsModulus:
                    return "0";
                case ZStringConstants.DMLParameterIDPoissonsRatio:
                    return "0";
                case ZStringConstants.DMLParameterIDWarpYarnCount:
                    return "0";
                case ZStringConstants.DMLParameterIDFillYarnCount:
                    return "0";
                case ZStringConstants.DMLParameterIDSquareWeave:
                    return "false";
                case ZStringConstants.DMLParameterIDLeadTime:
                    return "0";
                case ZStringConstants.DMLParameterIDDescription:
                    return string.Empty;
                case ZStringConstants.DMLParameterIDComments:
                    return string.Empty;

                default:
                    return string.Empty;
            }
        }
    }

    class ZSpan : ZEntity
    {
        public ZSpan(ZStackup stackup) 
        {
            ID = GenerateID("ViaSpan");
            Title = GenerateTitle(stackup.Spans, "ViaSpan");
            Stackup = stackup;
            Size = Options.TheOptions.drill;

            List<ZLayer> collection = stackup.GetMetallLayerCollection();
            if (collection.Count == 0) throw new Exception("Via Span cannot be created. There are no Plane or Signal layers.");
            FirstLayer = collection[0];
            LastLayer = collection[collection.Count - 1];
        }

        public ZSpan(string id, ZStackup stackup) 
        {
            ID = id;
            Stackup = stackup; 
        }

        public ZSpan Clone(bool keepIDAndTitle = false)
        {
            string id = (keepIDAndTitle) ? this.ID : GenerateID("ViaSpan");
            string title = (keepIDAndTitle) ? this.Title : GenerateTitle(this.Stackup.Spans, "ViaSpan");

            ZSpan span = new ZSpan(id, this.Stackup);
            span.Title = title;
            span.IsEnable = this.IsEnable;
            span.FirstLayer = this.FirstLayer;
            span.LastLayer = this.LastLayer;
            span.Size = this.Size;
            span.AspectRatio = this.AspectRatio;
            span.IsBackDrilled = this.IsBackDrilled;
            span.Filled = this.Filled;

            return span;
        }

        public ZStackup Stackup { get; private set; }
        //public ZParameter Parameter { get; set; }

        ZLayer _firstLayer;
        public ZLayer FirstLayer
        {
            get 
            { 
                return _firstLayer;
            }
            set 
            {
                if (value != _firstLayer && (_lastLayer == null || value.Order <= _lastLayer.Order)) 
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeSpanFirstLayerCommand(this, _firstLayer, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _firstLayer = value; 
                    NotifyPropertyChanged("FirstLayer");
                    CalculateAspectRatio();
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                } 
            }
        }
        
        ZLayer _lastLayer;
        public ZLayer LastLayer
        {
            get 
            { 
                return _lastLayer;
            }
            set 
            {
                if (value != _lastLayer && (_firstLayer == null || value.Order >= _firstLayer.Order)) 
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeSpanLastLayerCommand(this, _lastLayer, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _lastLayer = value;
                    NotifyPropertyChanged("LastLayer");
                    CalculateAspectRatio();
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        double _size;
        public double Size
        {
            get 
            { 
                return _size; 
            }
            set 
            { 
                if (value != _size) 
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeSpanSizeCommand(this, _size, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _size = value; 
                    NotifyPropertyChanged("Size");
                    CalculateAspectRatio();
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                } 
            }
        }

        double _aspectRatio;
        public double AspectRatio
        {
            get 
            {
                return _aspectRatio; 
            }
            set 
            {
                if (value != _aspectRatio) 
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeSpanAspectRatioCommand(this, _aspectRatio, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _aspectRatio = value;
                    NotifyPropertyChanged("AspectRatio"); 
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                } 
            }
        }

        bool _isEnable;
        public bool IsEnable
        {
            get
            {
                return _isEnable;
            }
            set
            {
                if (value != _isEnable)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeSpanEnableCommand(this, _isEnable, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isEnable = value;
                    NotifyPropertyChanged("Enable");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        string _filled;
        public string Filled
        {
            get
            {
                return _filled;
            }
            set
            {
                if (value != _filled)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeSpanFilledCommand(this, _filled, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _filled = value;
                    NotifyPropertyChanged("Filled");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        bool _isBackDrilled;
        public bool IsBackDrilled
        {
            get
            {
                return _isBackDrilled;
            }
            set
            {
                if (value != _isBackDrilled)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeSpanBackDrilledCommand(this, _isBackDrilled, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _isBackDrilled = value;
                    NotifyPropertyChanged("BackDrilled");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        internal void CalculateAspectRatio()
        {
            if (FirstLayer == null || LastLayer == null || double.IsNaN(Size) || Size == 0)
            {
                AspectRatio = 0;
                return;
            }

            double thikness = 0;
            for (int i = FirstLayer.Order; i <= LastLayer.Order; ++i)
            {
                thikness += Stackup.Layers[i].GetLayerThickness();
            }

            AspectRatio = thikness / Size;
        }

        internal double CalculateHoleLength()
        {
            double thikness = 0;
            if (FirstLayer != null && LastLayer != null)
            {


                for (int i = FirstLayer.Order; i <= LastLayer.Order; ++i)
                {
                    thikness += Stackup.Layers[i].GetLayerThickness();
                }
            }

            return thikness;
        }
    }

    class ZPair : ZEntity
    {
        public ZPair(ZStackup stackup) 
        {
            ID = GenerateID("Pair");
            Title = GenerateTitle(stackup.Pairs, "Zdiff");
            Stackup = stackup;
            Layers = new ZList<ZLayer>(this);

            foreach (ZLayer stackupLayer in stackup.Layers)
            {
                ZLayer layer = new ZLayer(stackupLayer.ID, stackup, this);
                layer.IsFrozen = stackupLayer.IsFrozen;
                layer.IsCustom = stackupLayer.IsCustom;
                layer.IsEdited = stackupLayer.IsEdited;
                layer.IsHidden = stackupLayer.IsHidden;
                layer.IsReadOnly = stackupLayer.IsReadOnly;
                layer.Height = stackupLayer.Height;
                layer.IsMaterialAssigned = stackupLayer.IsMaterialAssigned;

                List<ZParameter> pairSubparameters = new List<ZParameter>();
                foreach (ZParameter pairParameter in stackup.Project.Parameters.FindAll(x => x.Table == ZTableType.Pair))
                {
                    if (pairParameter.SubParameters != null && pairParameter.SubParameters.Count > 0) pairSubparameters.AddRange(pairParameter.SubParameters);
                    else pairSubparameters.Add(pairParameter);
                }

                foreach (ZParameter parameter in pairSubparameters)
                {
                    ZLayerType? layerType = stackupLayer.GetLayerType();
                    ZLayerParameter layerParameter = (layerType != null) ? new ZLayerParameter(parameter, layer, (ZLayerType)layerType) : new ZLayerParameter(parameter, layer);
                    if (layer.IsMaterialAssigned)
                    {
                        if (layerParameter.ID == ZStringConstants.ParameterIDZdiff_DielectricConstant) layerParameter.Value = stackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant);
                        if (layerParameter.ID == ZStringConstants.ParameterIDZdiff_LossTangent) layerParameter.Value = stackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLossTangent);
                    }

                    layer.LayerParameters.Add(layerParameter);
                }

                Layers.Add(layer);
            }
        }

        public ZPair(ZStackup stackup, ZPair activePair)
        {
            ID = GenerateID("Pair");
            Title = GenerateTitle(stackup.Pairs, "Zdiff");
            Stackup = stackup;
            Layers = new ZList<ZLayer>(this);

            foreach (ZLayer stackupLayer in stackup.Layers)
            {
                ZLayer layer = new ZLayer(stackupLayer.ID, stackup, this);
                layer.IsFrozen = stackupLayer.IsFrozen;
                layer.IsCustom = stackupLayer.IsCustom;
                layer.IsEdited = stackupLayer.IsEdited;
                layer.IsHidden = stackupLayer.IsHidden;
                layer.IsReadOnly = stackupLayer.IsReadOnly;
                layer.Height = stackupLayer.Height;
                layer.IsMaterialAssigned = stackupLayer.IsMaterialAssigned;

                List<ZParameter> pairSubparameters = new List<ZParameter>();
                foreach (ZParameter pairParameter in stackup.Project.Parameters.FindAll(x => x.Table == ZTableType.Pair))
                {
                    if (pairParameter.SubParameters != null && pairParameter.SubParameters.Count > 0) pairSubparameters.AddRange(pairParameter.SubParameters);
                    else pairSubparameters.Add(pairParameter);
                }

                foreach (ZParameter parameter in pairSubparameters)
                {
                    ZLayerParameter layerParameter;
                    ZLayerType? layerType = stackupLayer.GetLayerType();
                    ZLayer activePairLayer = activePair.Layers.GetByID(stackupLayer.ID);
                    ZLayerParameter activePairLayerParameter = (activePairLayer != null) ? activePairLayer.LayerParameters.GetByID(parameter.ID) : null;
                    if (activePairLayer != null && activePairLayerParameter != null)
                    {
                        layerParameter = activePairLayerParameter.Clone();
                        layerParameter.Layer = layer;
                    }
                    else
                    {
                        layerParameter = (layerType != null) ? new ZLayerParameter(parameter, layer, (ZLayerType)layerType) : new ZLayerParameter(parameter, layer);
                    }

                    layer.LayerParameters.Add(layerParameter);
                }

                Layers.Add(layer);
            }
        }

        public ZPair(string id, ZStackup stackup)
        {
            ID = id;
            Stackup = stackup;
            Layers = new ZList<ZLayer>(this);
        }

        public ZLayer GetLayerOfPairImpedance(int index)
        {
            bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            return this.Layers[index];
        }

        public ZLayer GetLayerOfPairImpedance(string id)
        {
            ZLayer layer = this.Layers.Find(x => x.ID == id);
            if (layer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");
            return layer;
        }

        internal bool HasValues()
        {
            bool hasValues = false;
            foreach (ZLayer layer in this.Layers)
            {
                if (layer.HasValues())
                {
                    hasValues = true;
                    break;
                }
            }

            return hasValues;
        }

        internal bool HasValues(string[] arrayOfLayerParameterID)
        {
            bool hasValues = false;
            foreach (ZLayer layer in this.Layers)
            {
                if (layer.HasValues(arrayOfLayerParameterID))
                {
                    hasValues = true;
                    break;
                }
            }

            return hasValues;
        }

        internal bool IsUsed()
        {
            bool isUsed = false;
            foreach (ZLayer layer in this.Layers)
            {
                if (layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_IsUsed) == "true")
                {
                    isUsed = true;
                    break;
                }
            }

            return isUsed;
        }

        public ZStackup Stackup { get; private set; }
        public ZList<ZLayer> Layers { get; private set; }

        private double impedanceTarget = 0.0;
        public double ImpedanceTarget
        {
            get { return impedanceTarget; }
            set { if (value != impedanceTarget) { new ChangePairImpedanceTargetCommand(this, impedanceTarget, value); impedanceTarget = value; NotifyPropertyChanged("ImpedanceTarget"); } }
        }
    }

    class ZSingle : ZEntity
    {
        /*public ZSingle(ZStackup stackup)
        {
            ID = "Single";
            Title = "Single Ended";
            Stackup = stackup;
            Layers = new ZList<ZLayer>(this);

            foreach (ZLayer stackupLayer in stackup.Layers)
            {
                ZLayer layer = new ZLayer(stackupLayer.ID, stackup, this);
                layer.IsFrozen = stackupLayer.IsFrozen;
                layer.IsCustom = stackupLayer.IsCustom;
                layer.IsEdited = stackupLayer.IsEdited;
                layer.IsHidden = stackupLayer.IsHidden;
                layer.IsReadOnly = stackupLayer.IsReadOnly;
                layer.Height = stackupLayer.Height;
                layer.IsMaterialAssigned = stackupLayer.IsMaterialAssigned;

                List<ZParameter> singleSubparameters = new List<ZParameter>();
                foreach (ZParameter singleParameter in stackup.Project.Parameters.FindAll(x => x.Table == ZTableType.Single))
                {
                    if (singleParameter.SubParameters != null && singleParameter.SubParameters.Count > 0) singleSubparameters.AddRange(singleParameter.SubParameters);
                    else singleSubparameters.Add(singleParameter);
                }

                foreach (ZParameter parameter in singleSubparameters)
                {
                    ZLayerType? layerType = stackupLayer.GetLayerType();
                    ZLayerParameter layerParameter = (layerType != null) ? new ZLayerParameter(parameter, layer, (ZLayerType)layerType) : new ZLayerParameter(parameter, layer);
                    layer.LayerParameters.Add(layerParameter);
                }

                Layers.Add(layer);
            }
        }*/

        public ZSingle(ZStackup stackup)
        {
            ID = GenerateID("Single");
            Title = GenerateTitle(stackup.Singles, "Zo");
            Stackup = stackup;
            Layers = new ZList<ZLayer>(this);

            foreach (ZLayer stackupLayer in stackup.Layers)
            {
                ZLayer layer = new ZLayer(stackupLayer.ID, stackup, this);
                layer.IsFrozen = stackupLayer.IsFrozen;
                layer.IsCustom = stackupLayer.IsCustom;
                layer.IsEdited = stackupLayer.IsEdited;
                layer.IsHidden = stackupLayer.IsHidden;
                layer.IsReadOnly = stackupLayer.IsReadOnly;
                layer.Height = stackupLayer.Height;
                layer.IsMaterialAssigned = stackupLayer.IsMaterialAssigned;

                List<ZParameter> singleSubparameters = new List<ZParameter>();
                foreach (ZParameter singleParameter in stackup.Project.Parameters.FindAll(x => x.Table == ZTableType.Single))
                {
                    if (singleParameter.SubParameters != null && singleParameter.SubParameters.Count > 0) singleSubparameters.AddRange(singleParameter.SubParameters);
                    else singleSubparameters.Add(singleParameter);
                }

                foreach (ZParameter parameter in singleSubparameters)
                {
                    ZLayerType? layerType = stackupLayer.GetLayerType();
                    ZLayerParameter layerParameter = (layerType != null) ? new ZLayerParameter(parameter, layer, (ZLayerType)layerType) : new ZLayerParameter(parameter, layer);
                    if (layer.IsMaterialAssigned)
                    {
                        if (layerParameter.ID == ZStringConstants.ParameterIDZo_DielectricConstant) layerParameter.Value = stackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant);
                        if (layerParameter.ID == ZStringConstants.ParameterIDZo_LossTangent) layerParameter.Value = stackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLossTangent);
                    }

                    layer.LayerParameters.Add(layerParameter);
                }

                Layers.Add(layer);
            }
        }

        public ZSingle(ZStackup stackup, ZSingle activeSingle)
        {
            ID = GenerateID("Single");
            Title = GenerateTitle(stackup.Singles, "Zo");
            Stackup = stackup;
            Layers = new ZList<ZLayer>(this);

            foreach (ZLayer stackupLayer in stackup.Layers)
            {
                ZLayer layer = new ZLayer(stackupLayer.ID, stackup, this);
                layer.IsFrozen = stackupLayer.IsFrozen;
                layer.IsCustom = stackupLayer.IsCustom;
                layer.IsEdited = stackupLayer.IsEdited;
                layer.IsHidden = stackupLayer.IsHidden;
                layer.IsReadOnly = stackupLayer.IsReadOnly;
                layer.Height = stackupLayer.Height;
                layer.IsMaterialAssigned = stackupLayer.IsMaterialAssigned;

                List<ZParameter> singleSubparameters = new List<ZParameter>();
                foreach (ZParameter singleParameter in stackup.Project.Parameters.FindAll(x => x.Table == ZTableType.Single))
                {
                    if (singleParameter.SubParameters != null && singleParameter.SubParameters.Count > 0) singleSubparameters.AddRange(singleParameter.SubParameters);
                    else singleSubparameters.Add(singleParameter);
                }

                foreach (ZParameter parameter in singleSubparameters)
                {
                    ZLayerParameter layerParameter;
                    ZLayerType? layerType = stackupLayer.GetLayerType();
                    ZLayer activeSingleLayer = activeSingle.Layers.GetByID(stackupLayer.ID);
                    ZLayerParameter activeSingleLayerParameter = (activeSingleLayer != null) ? activeSingleLayer.LayerParameters.GetByID(parameter.ID) : null;
                    if (activeSingleLayer != null && activeSingleLayerParameter != null)
                    {
                        layerParameter = activeSingleLayerParameter.Clone();
                        layerParameter.Layer = layer;
                    }
                    else
                    {
                        layerParameter = (layerType != null) ? new ZLayerParameter(parameter, layer, (ZLayerType)layerType) : new ZLayerParameter(parameter, layer);
                    }

                    layer.LayerParameters.Add(layerParameter);
                }

                Layers.Add(layer);
            }
        }

        public ZSingle(string id, ZStackup stackup)
        {
            ID = id;
            Stackup = stackup;
            Layers = new ZList<ZLayer>(this);
        }

        public ZLayer GetLayerOfSingleImpedance(int index)
        {
            bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            return this.Layers[index];
        }

        public ZLayer GetLayerOfSingleImpedance(string id)
        {
            ZLayer layer = this.Layers.Find(x => x.ID == id);
            if (layer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");
            return layer;
        }

        internal bool HasValues()
        {
            bool hasValues = false;
            foreach (ZLayer layer in this.Layers)
            {
                if (layer.HasValues())
                {
                    hasValues = true;
                    break;
                }
            }

            return hasValues;
        }

        internal bool HasValues(string[] arrayOfLayerParameterID)
        {
            bool hasValues = false;
            foreach (ZLayer layer in this.Layers)
            {
                if (layer.HasValues(arrayOfLayerParameterID))
                {
                    hasValues = true;
                    break;
                }
            }

            return hasValues;
        }

        internal bool IsUsed()
        {
            bool isUsed = false;
            foreach (ZLayer layer in this.Layers)
            {
                if (layer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_IsUsed) == "true")
                {
                    isUsed = true;
                    break;
                }
            }

            return isUsed;
        }

        public ZStackup Stackup { get; private set; }
        public ZList<ZLayer> Layers { get; private set; }

        private double impedanceTarget = 0.0;
        public double ImpedanceTarget
        {
            get { return impedanceTarget; }
            set { if (value != impedanceTarget) { new ChangeSingleImpedanceTargetCommand(this, impedanceTarget, value); impedanceTarget = value; NotifyPropertyChanged("ImpedanceTarget"); } }
        }
    }

    class ZStackup : ZEntity
    {
        public ZStackup(ZPlannerProject project)
        {
            Project = project;
            ID = GenerateID("Stackup");
            Title = "Stackup";
            Frequency = Options.TheOptions.Frequency;
            Layers = new ZList<ZLayer>(this);
            Layers.CollectionChanged += Layers_CollectionChanged;
            Layers.BeforeCollectionChanged += Layers_BeforeCollectionChanged;
            Singles = new ZList<ZSingle>(this);
            Singles.CollectionChanged += Singles_CollectionChanged;
            Singles.Add(new ZSingle(this));
            Pairs = new ZList<ZPair>(this);
            Pairs.CollectionChanged += Pairs_CollectionChanged;
            Pairs.Add(new ZPair(this));
            Spans = new ZList<ZSpan>(this);

            SetDefaultProperties();
            ExportOptions.TheOptions.ProjectName = "";
        }

        public ZStackup(ZPlannerProject project, string id)
        {
            Project = project;
            ID = id;
            Frequency = Options.TheOptions.Frequency;
            Layers = new ZList<ZLayer>(this);
            Layers.CollectionChanged += Layers_CollectionChanged;
            Layers.BeforeCollectionChanged += Layers_BeforeCollectionChanged;
            Singles = new ZList<ZSingle>(this);
            Singles.CollectionChanged += Singles_CollectionChanged;
            Pairs = new ZList<ZPair>(this);
            Pairs.CollectionChanged += Pairs_CollectionChanged;
            Spans = new ZList<ZSpan>(this);

            SetDefaultProperties();
            ExportOptions.TheOptions.ProjectName = "";
        }

        private void SetDefaultProperties()
        {
            //roughness
            OuterLayerRoughness = Options.TheOptions.roughness;
            LaminateSideRoughness = Options.TheOptions.laminate_side_roughness;
            PrepregSideRoughness = Options.TheOptions.prepreg_side_roughness;

            //core-side roughness
            CoreSideRoughnessHTE = Options.TheOptions.core_side_roughness_hte;
            CoreSideRoughnessRTF = Options.TheOptions.core_side_roughness_rtf;
            CoreSideRoughnessVLP = Options.TheOptions.core_side_roughness_vlp;
            CoreSideRoughnessVLP2 = Options.TheOptions.core_side_roughness_vlp2;
            CoreSideRoughnessHVLP = Options.TheOptions.core_side_roughness_hvlp;

            //prepreg-side roughness
            PrepregSideRoughnessMB100LE = Options.TheOptions.prepreg_side_roughness_mb100le;
            PrepregSideRoughnessAlphaPrep = Options.TheOptions.prepreg_side_roughness_alphaprep;
            PrepregSideRoughnessMB100ZK = Options.TheOptions.prepreg_side_roughness_mb100zk;
            PrepregSideRoughnessBF = Options.TheOptions.prepreg_side_roughness_bf;
            PrepregSideRoughnessCZ8100 = Options.TheOptions.prepreg_side_roughness_cz8100;
            PrepregSideRoughnessMB100HP = Options.TheOptions.prepreg_side_roughness_mb100hp;

            //pressed thickness
            CopperCoverageType = Options.TheOptions.ByCopperCoverage ? ZCopperCoverageType.PrepregProportional : ZCopperCoverageType.PrepregPercent;
            ForSignal = Options.TheOptions.forSignal;
            ForMixed = Options.TheOptions.forMixed;
            ForPlane = Options.TheOptions.forPlane;
            ForSignalSignal = Options.TheOptions.signal_signal;
            ForSignalMixed = Options.TheOptions.signal_mixed;
            ForSignalPlane = Options.TheOptions.signal_plane;
            ForMixedMixed = Options.TheOptions.mixed_mixed;
            ForMixedPlane = Options.TheOptions.mixed_plane;
            ForPlanePlane = Options.TheOptions.plane_plane;

            // Etch Effects
            Etch = Options.TheOptions.etch;
        }

        void Layers_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (ZPlannerManager.IsIgnoreCollectionChanged) return;
            int index = 0;
            foreach (ZLayer layer in Layers)
            {
                layer.Order = index++;
                layer.SetTitle("Row " + (index < 10 ? " " : "") + index);
            }
            CalculateLayerNumber();
        }

        void Layers_BeforeCollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (e.Action == NotifyCollectionChangedAction.Add && ZPlannerManager.rights.MaxLayersNumber > 0)
            {
                int metallLayersNumber = 0;
                foreach (ZLayer layer in e.NewItems)
                {
                    ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDLayerType);
                    if (layerParameter == null || !ZLayer.LayerTypeDictionary.ContainsKey(layerParameter.Value)) continue;

                    ZLayerType layerType = ZLayer.LayerTypeDictionary[layerParameter.Value];
                    if (layerType == ZLayerType.Plane || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed) ++metallLayersNumber;
                }

                if (GetMetallLayerCount() + metallLayersNumber > ZPlannerManager.rights.MaxLayersNumber) throw new LicenseEcxeption("License limitation. You cannot create more then " + ZPlannerManager.rights.MaxLayersNumber + " rows");
            }
        }

        void Singles_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            //if (ZPlannerManager.IsIgnoreCollectionChanged) return;
            if (this.Singles.Count == 0)
                Singles.Add(new ZSingle(this));
            if (ActiveSingle != null && !Singles.Contains(ActiveSingle) || ActiveSingle == null) ActiveSingle = Singles[0];
            ZPlannerManager.MainMenu.UpdateSingles(Singles);
        }

        void Pairs_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            //if (ZPlannerManager.IsIgnoreCollectionChanged) return;
            if (this.Pairs.Count == 0) 
                Pairs.Add(new ZPair(this));
            if (ActivePair != null && !Pairs.Contains(ActivePair) || ActivePair == null) ActivePair = Pairs[0];
            ZPlannerManager.MainMenu.UpdatePairs(Pairs);
        }

        public ZLayer GetLayerOfStackup(int index)
        {
            bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            ZLayer layer = this.Layers[index];
            return this.GetLayerOfStackup(layer.ID);
        }

        public ZLayer GetLayerOfStackup(string id)
        {
            return this.Layers.Find(x => x.ID == id);
        }

        public int GetLayerOfStackupIndex(string id)
        {
            return this.Layers.FindIndex(0, x => x.ID == id);
        }

        public ZLayer GetLayerOfSingleImpedance(int index, string singleID)
        {
            bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            ZLayer layer = this.Layers[index];
            return this.GetLayerOfSingleImpedance(layer.ID, singleID);
        }

        public ZLayer GetLayerOfSingleImpedance(string id, string singleID)
        {
            ZSingle single = this.Singles.Find(x => x.ID == singleID);
            if (single == null) return null;
            return single.Layers.Find(x => x.ID == id);
        }

        public ZLayer GetLayerOfPairImpedance(int index, string pairID)
        {
            bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            ZLayer layer = this.Layers[index];
            return this.GetLayerOfPairImpedance(layer.ID, pairID);
        }

        public ZLayer GetLayerOfPairImpedance(string id, string pairID)
        {
            ZPair pair = this.Pairs.Find(x => x.ID == pairID);
            if (pair == null) return null;
            return pair.Layers.Find(x => x.ID == id);
        }

        public string AddLayer(int index)
        { return AddLayer(index, ZLayerType.Signal); }

        public string AddLayer(ZLayerType layerType)
        { return AddLayer(this.Layers.Count, layerType); }

        public LayersCommandStructure CreateLayersCommandStructureByType(int index, ZLayerType layerType)
        {
            LayersCommandStructure currentLayerStructure = new LayersCommandStructure();

            currentLayerStructure.Index = index;

            // Stackup Layers.
            currentLayerStructure.StackupLayer = new ZLayer(this, this, layerType);
            currentLayerStructure.Id = currentLayerStructure.StackupLayer.ID;

            // Single Layers.
            currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
            List<ZParameter> singleSubparameters = new List<ZParameter>();
            foreach (ZParameter singleParameter in Project.Parameters.FindAll(x => x.Table == ZTableType.Single))
            {
                if (singleParameter.SubParameters != null && singleParameter.SubParameters.Count > 0) singleSubparameters.AddRange(singleParameter.SubParameters);
                else singleSubparameters.Add(singleParameter);
            }

            foreach (ZSingle single in this.Singles)
            {
                ZLayer singleLayer = new ZLayer(currentLayerStructure.StackupLayer.ID, this, single);
                singleLayer.IsFrozen = currentLayerStructure.StackupLayer.IsFrozen;
                singleLayer.IsCustom = currentLayerStructure.StackupLayer.IsCustom;
                singleLayer.IsEdited = currentLayerStructure.StackupLayer.IsEdited;
                singleLayer.IsHidden = currentLayerStructure.StackupLayer.IsHidden;
                singleLayer.IsReadOnly = currentLayerStructure.StackupLayer.IsReadOnly;
                singleLayer.Height = currentLayerStructure.StackupLayer.Height;

                foreach (ZParameter parameter in singleSubparameters)
                {
                    ZLayerParameter layerParameter = new ZLayerParameter(parameter, singleLayer, layerType);
                    singleLayer.LayerParameters.Add(layerParameter);
                }

                currentLayerStructure.SingleLayers.Add(single.ID, singleLayer);
            }

            // Pair Layers.
            currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
            List<ZParameter> pairSubparameters = new List<ZParameter>();
            foreach (ZParameter pairParameter in Project.Parameters.FindAll(x => x.Table == ZTableType.Pair))
            {
                if (pairParameter.SubParameters != null && pairParameter.SubParameters.Count > 0) pairSubparameters.AddRange(pairParameter.SubParameters);
                else pairSubparameters.Add(pairParameter);
            }

            foreach (ZPair pair in this.Pairs)
            {
                ZLayer pairLayer = new ZLayer(currentLayerStructure.StackupLayer.ID, this, pair);
                pairLayer.IsFrozen = currentLayerStructure.StackupLayer.IsFrozen;
                pairLayer.IsCustom = currentLayerStructure.StackupLayer.IsCustom;
                pairLayer.IsEdited = currentLayerStructure.StackupLayer.IsEdited;
                pairLayer.IsHidden = currentLayerStructure.StackupLayer.IsHidden;
                pairLayer.IsReadOnly = currentLayerStructure.StackupLayer.IsReadOnly;
                pairLayer.Height = currentLayerStructure.StackupLayer.Height;

                foreach (ZParameter parameter in pairSubparameters)
                {
                    ZLayerParameter layerParameter = new ZLayerParameter(parameter, pairLayer, layerType);
                    pairLayer.LayerParameters.Add(layerParameter);
                }

                currentLayerStructure.PairLayers.Add(pair.ID, pairLayer);
            }

            return currentLayerStructure;
        }

        public LayersCommandStructure CreateLayersCommandStructureByLayer(int index, ZLayer layer)
        {
            LayersCommandStructure currentLayerStructure = new LayersCommandStructure();

            currentLayerStructure.Index = index;

            // Stackup Layers.
            currentLayerStructure.StackupLayer = layer.Clone();
            currentLayerStructure.Id = currentLayerStructure.StackupLayer.ID;

            // Single Layers.
            currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
            foreach (ZSingle single in this.Singles)
            {
                ZLayer singleLayer = GetLayerOfSingleImpedance(layer.ID, single.ID).Clone();
                singleLayer.ID = currentLayerStructure.StackupLayer.ID;

                currentLayerStructure.SingleLayers.Add(single.ID, singleLayer);
            }

            // Pair Layers.
            currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
            foreach (ZPair pair in this.Pairs)
            {
                ZLayer pairLayer = GetLayerOfPairImpedance(layer.ID, pair.ID).Clone();
                pairLayer.ID = currentLayerStructure.StackupLayer.ID;

                currentLayerStructure.PairLayers.Add(pair.ID, pairLayer);
            }

            return currentLayerStructure;
        }

        public string AddLayer(int index, ZLayerType layerType, bool isCoreLocked = false)
        {
            int count = this.Layers.Count;
            // Uncomment middleLayer to create new row using the middle row
            //ZLayer middleLayer = (count > 0) ? Layers[(int)(count * 0.5)] : null;
            //if (middleLayer != null &&  middleLayer.GetLayerType() != layerType) middleLayer = null;
            // Clear Layer Selection
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            ZPlannerManager.SuspendCollectionChangedEvent();
            try
            {
                Dictionary<string, LayersCommandStructure> underList = new Dictionary<string, LayersCommandStructure>();
                Dictionary<string, LayersCommandStructure> overList = new Dictionary<string, LayersCommandStructure>();

                // LockingCore
                if (isCoreLocked)
                {
                    CollapseLockedLayers(underList, overList);
                }

                bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);

                foreach (ZSingle single in this.Singles)
                {
                    isIndexIncorrect = isIndexIncorrect || index > single.Layers.Count;
                }

                foreach (ZPair pair in this.Pairs)
                {
                    isIndexIncorrect = isIndexIncorrect || index > pair.Layers.Count;
                }

                if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

                int mirrorIndex = Layers.Count - index;

                List<LayersCommandStructure> layerStructureList = new List<LayersCommandStructure>();
                LayersCommandStructure currentLayerStructure;
                LayersCommandStructure mirrorLayerStructure;

                if(isCoreLocked && layerType == ZLayerType.Core)
                {
                    LayersCommandStructure copperLayerStructure = CreateLayersCommandStructureByType(index, ZLayerType.Signal);
                    InternalInsertLayer(copperLayerStructure);
                    layerStructureList.Add(copperLayerStructure);
                }

                //currentLayerStructure = (middleLayer != null) ? CreateLayersCommandStructureByLayer(index, middleLayer) : CreateLayersCommandStructureByType(index, layerType);
                currentLayerStructure = CreateLayersCommandStructureByType(index, layerType);
                InternalInsertLayer(currentLayerStructure);
                layerStructureList.Add(currentLayerStructure);

                if (isCoreLocked && layerType == ZLayerType.Core)
                {
                    LayersCommandStructure copperLayerStructure = CreateLayersCommandStructureByType(index, ZLayerType.Signal);
                    InternalInsertLayer(copperLayerStructure);
                    layerStructureList.Add(copperLayerStructure);
                }

                if (mirrorIndex >= index) mirrorIndex += layerStructureList.Count;



                if (ZPlannerManager.StackupPanel != null && ZPlannerManager.IsAutoMirror && !IsMirroredByTypesAndValues())
                {
                    if (isCoreLocked && layerType == ZLayerType.Core)
                    {
                        LayersCommandStructure copperLayerStructure = CreateLayersCommandStructureByType(mirrorIndex, ZLayerType.Signal);
                        InternalInsertLayer(copperLayerStructure);
                        layerStructureList.Add(copperLayerStructure);
                    }

                    mirrorLayerStructure = CreateLayersCommandStructureByType(mirrorIndex, layerType);
                    InternalInsertLayer(mirrorLayerStructure);
                    layerStructureList.Add(mirrorLayerStructure);

                    if (isCoreLocked && layerType == ZLayerType.Core)
                    {
                        LayersCommandStructure copperLayerStructure = CreateLayersCommandStructureByType(mirrorIndex, ZLayerType.Signal);
                        InternalInsertLayer(copperLayerStructure);
                        layerStructureList.Add(copperLayerStructure);
                    }
                }

                // LockingCore
                if (isCoreLocked)
                {
                    ExpandLockedLayers(underList, overList);
                }

                ZPlannerManager.ResumeCollectionChangedEvent();
                Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new AddLayerCommand(this, layerStructureList);

                return currentLayerStructure.StackupLayer.ID;
            }
            finally
            {
                ZPlannerManager.ResumeCollectionChangedEvent();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        internal void InternalInsertLayer(LayersCommandStructure layerStructure, bool isSetSelection = true)
        {
            bool isIndexIncorrect = (layerStructure.Index < 0 || layerStructure.Index > this.Layers.Count);
            
            foreach (ZSingle single in this.Singles)
            {
                isIndexIncorrect = isIndexIncorrect || layerStructure.Index > single.Layers.Count;
            }

            foreach (ZPair pair in this.Pairs)
            {
                isIndexIncorrect = isIndexIncorrect || layerStructure.Index > pair.Layers.Count;
            }

            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            // Stackup Layers.
            if (layerStructure.StackupLayer == null) return;

            // Insert Stackup Layer at the end of the code block, because Grid redraw after insert of Stackup Layer.

            // Single Layers.
            List<ZParameter> singleSubparameters = new List<ZParameter>();
            foreach (ZParameter singleParameter in Project.Parameters.FindAll(x => x.Table == ZTableType.Single))
            {
                if (singleParameter.SubParameters != null && singleParameter.SubParameters.Count > 0) singleSubparameters.AddRange(singleParameter.SubParameters);
                else singleSubparameters.Add(singleParameter);
            }

            if (layerStructure.RemovedSingles != null)
            {
                foreach (ZSingle single in layerStructure.RemovedSingles.Keys)
                {
                    this.Singles.Insert(layerStructure.RemovedSingles[single], single); //.RemoveAt(i);
                }
            }

            foreach (ZSingle single in this.Singles)
            {
                ZLayer singleLayer = (layerStructure.SingleLayers.ContainsKey(single.ID)) ? layerStructure.SingleLayers[single.ID] : null;
                if (singleLayer == null)
                {
                    singleLayer = new ZLayer(layerStructure.StackupLayer.ID, this, single);
                    singleLayer.IsFrozen = layerStructure.StackupLayer.IsFrozen;
                    singleLayer.IsCustom = layerStructure.StackupLayer.IsCustom;
                    singleLayer.IsEdited = layerStructure.StackupLayer.IsEdited;
                    singleLayer.IsHidden = layerStructure.StackupLayer.IsHidden;
                    singleLayer.IsReadOnly = layerStructure.StackupLayer.IsReadOnly;
                    singleLayer.Height = layerStructure.StackupLayer.Height;

                    foreach (ZParameter parameter in singleSubparameters)
                    {
                        ZLayerType? layerType = layerStructure.StackupLayer.GetLayerType();
                        ZLayerParameter layerParameter = (layerType != null) ? new ZLayerParameter(parameter, singleLayer, (ZLayerType)layerType) : new ZLayerParameter(parameter, singleLayer);
                        singleLayer.LayerParameters.Add(layerParameter);
                    }
                }
                else
                {
                    singleLayer.ID = layerStructure.StackupLayer.ID;
                }

                single.Layers.Insert(layerStructure.Index, singleLayer);
            }

            // Pair Layers.
            List<ZParameter> pairSubparameters = new List<ZParameter>();
            foreach (ZParameter pairParameter in Project.Parameters.FindAll(x => x.Table == ZTableType.Pair))
            {
                if (pairParameter.SubParameters != null && pairParameter.SubParameters.Count > 0) pairSubparameters.AddRange(pairParameter.SubParameters);
                else pairSubparameters.Add(pairParameter);
            }

            if (layerStructure.RemovedPairs != null)
            {
                foreach (ZPair pair in layerStructure.RemovedPairs.Keys)
                {
                    this.Pairs.Insert(layerStructure.RemovedPairs[pair], pair); //.RemoveAt(i);
                }
            }

            foreach (ZPair pair in this.Pairs)
            {
                ZLayer pairLayer = (layerStructure.PairLayers.ContainsKey(pair.ID)) ? layerStructure.PairLayers[pair.ID] : null;
                if (pairLayer == null)
                {
                    pairLayer = new ZLayer(layerStructure.StackupLayer.ID, this, pair);
                    pairLayer.IsFrozen = layerStructure.StackupLayer.IsFrozen;
                    pairLayer.IsCustom = layerStructure.StackupLayer.IsCustom;
                    pairLayer.IsEdited = layerStructure.StackupLayer.IsEdited;
                    pairLayer.IsHidden = layerStructure.StackupLayer.IsHidden;
                    pairLayer.IsReadOnly = layerStructure.StackupLayer.IsReadOnly;
                    pairLayer.Height = layerStructure.StackupLayer.Height;

                    foreach (ZParameter parameter in pairSubparameters)
                    {
                        ZLayerType? layerType = layerStructure.StackupLayer.GetLayerType();
                        ZLayerParameter layerParameter = (layerType != null) ? new ZLayerParameter(parameter, pairLayer, (ZLayerType)layerType) : new ZLayerParameter(parameter, pairLayer);
                        pairLayer.LayerParameters.Add(layerParameter);
                    }
                }
                else
                {
                    pairLayer.ID = layerStructure.StackupLayer.ID;
                }

                pair.Layers.Insert(layerStructure.Index, pairLayer);
            }

            // ViaSpans.
            if (layerStructure.RemovedSpans != null)
            {
                foreach (ZSpan span in layerStructure.RemovedSpans.Keys)
                {
                    this.Spans.Insert(layerStructure.RemovedSpans[span], span); //.RemoveAt(i);
                }
            }

            // Stackup Layer.
            this.Layers.Insert(layerStructure.Index, layerStructure.StackupLayer);

            // Select Layer
            if (ZPlannerManager.StackupPanel != null && isSetSelection) ZPlannerManager.StackupPanel.AddRowSelection(layerStructure.Index);
        }

        internal void InternalReplaceLayer(LayersCommandStructure layerStructure, bool isSetSelection = true)
        {
            bool isIndexIncorrect = (layerStructure.Index < 0 || layerStructure.Index > this.Layers.Count);
            
            foreach (ZSingle single in this.Singles)
            {
                isIndexIncorrect = isIndexIncorrect || layerStructure.Index > single.Layers.Count;
            }

            foreach (ZPair pair in this.Pairs)
            {
                isIndexIncorrect = isIndexIncorrect || layerStructure.Index > pair.Layers.Count;
            }

            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            // Stackup Layers.
            if (layerStructure.StackupLayer == null || layerStructure.SingleLayers == null || layerStructure.PairLayers == null) return;

            // Insert Stackup Layer at the end of the code block, because Grid redraw after insert of Stackup Layer.


            ZLayer stackupLayer = ZPlannerManager.Project.Stackup.GetLayerOfStackup(layerStructure.StackupLayer.ID);
            stackupLayer.IsMaterialAssigned = layerStructure.StackupLayer.IsMaterialAssigned;
            foreach(ZLayerParameter layerParameter in layerStructure.StackupLayer.LayerParameters)
            {
                stackupLayer.SetLayerParameterValue(layerParameter.ID, layerParameter.Value);
            }

            // Single Layers.
            foreach (ZSingle single in this.Singles)
            {
                ZLayer singleLayer = ZPlannerManager.Project.Stackup.GetLayerOfSingleImpedance(layerStructure.StackupLayer.ID, single.ID);
                if (layerStructure.SingleLayers.ContainsKey(single.ID))
                {
                    layerStructure.SingleLayers[single.ID].ID = layerStructure.StackupLayer.ID;
                    singleLayer.IsMaterialAssigned = layerStructure.SingleLayers[single.ID].IsMaterialAssigned;
                    foreach (ZLayerParameter layerParameter in layerStructure.SingleLayers[single.ID].LayerParameters)
                    {
                        singleLayer.SetLayerParameterValue(layerParameter.ID, layerParameter.Value);
                    }
                }
            }

            // Pair Layers.
            foreach (ZPair pair in this.Pairs)
            {
                ZLayer pairLayer = ZPlannerManager.Project.Stackup.GetLayerOfPairImpedance(layerStructure.StackupLayer.ID, pair.ID);
                if (layerStructure.PairLayers.ContainsKey(pair.ID))
                {
                    layerStructure.PairLayers[pair.ID].ID = layerStructure.StackupLayer.ID;
                    pairLayer.IsMaterialAssigned = layerStructure.PairLayers[pair.ID].IsMaterialAssigned;
                    foreach (ZLayerParameter layerParameter in layerStructure.PairLayers[pair.ID].LayerParameters)
                    {
                        pairLayer.SetLayerParameterValue(layerParameter.ID, layerParameter.Value);
                    }
                }
            }

            CalculateLayerNumber();

            // Select Layer
            if (ZPlannerManager.StackupPanel != null && isSetSelection) ZPlannerManager.StackupPanel.AddRowSelection(layerStructure.Index);
        }

        private LayersCommandStructure GetLayersCommandStructureByID(string id)
        {
            LayersCommandStructure currentLayerStructure = new LayersCommandStructure();

            currentLayerStructure.Id = id;
            // Stackup Layer.
            currentLayerStructure.StackupLayer = this.Layers.Find(x => x.ID == currentLayerStructure.Id);
            if (currentLayerStructure.StackupLayer == null) return currentLayerStructure;
            currentLayerStructure.Index = currentLayerStructure.StackupLayer.Order;

            // Single Layer.
            currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
            currentLayerStructure.RemovedSingles = new Dictionary<ZSingle, int>();
            for (int i = this.Singles.Count - 1; i >= 0; --i)
            {
                ZSingle single = this.Singles[i];
                ZLayer singleLayer = single.Layers.Find(x => x.ID == currentLayerStructure.Id);
                if (singleLayer == null) return currentLayerStructure;

                currentLayerStructure.SingleLayers.Add(single.ID, singleLayer);

                if (singleLayer.isMetal())
                {
                    foreach (ZLayerParameter layerParameter in singleLayer.LayerParameters)
                    {
                        if (layerParameter.ID != ZStringConstants.ParameterIDZo_IsUsed && !string.IsNullOrEmpty(layerParameter.Value))
                        {
                            currentLayerStructure.RemovedSingles.Add(single, Singles.IndexOf(single));
                            break;
                        }
                    }
                }
            }

            // Pair Layers
            currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
            currentLayerStructure.RemovedPairs = new Dictionary<ZPair, int>();
            for (int i = this.Pairs.Count - 1; i >= 0; --i)
            {
                ZPair pair = this.Pairs[i];
                ZLayer pairLayer = pair.Layers.Find(x => x.ID == currentLayerStructure.Id);
                if (pairLayer == null) return currentLayerStructure;

                currentLayerStructure.PairLayers.Add(pair.ID, pairLayer);

                if (pairLayer.isMetal())
                {
                    foreach (ZLayerParameter layerParameter in pairLayer.LayerParameters)
                    {
                        if (layerParameter.ID != ZStringConstants.ParameterIDZdiff_IsUsed && !string.IsNullOrEmpty(layerParameter.Value))
                        {
                            currentLayerStructure.RemovedPairs.Add(pair, Pairs.IndexOf(pair));
                            break;
                        }
                    }
                }
            }

            // ViaSpans.
            /*currentLayerStructure.RemovedSpans = new Dictionary<ZSpan, int>();
            for (int i = this.Spans.Count - 1; i >= 0; --i)
            {
                ZSpan span = this.Spans[i];
                if (span.FirstLayer.ID == currentLayerStructure.Id || span.LastLayer.ID == currentLayerStructure.Id)
                {
                    currentLayerStructure.RemovedSpans.Add(span, Spans.IndexOf(span));
                }
            }*/

            return currentLayerStructure;
        }

        private LayersCommandStructure GetLayersCommandStructureByLayerParts(int index, ZLayer stackupLayer, Dictionary<string, ZLayer> singlesDict, Dictionary<string, ZLayer> pairsDict)
        {
            LayersCommandStructure currentLayerStructure = new LayersCommandStructure();

            currentLayerStructure.Id = stackupLayer.ID;
            currentLayerStructure.Index = index;
            // Stackup Layer.
            currentLayerStructure.StackupLayer = stackupLayer;
            currentLayerStructure.StackupLayer.ID = currentLayerStructure.Id;

            // Single Layer.
            currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
            currentLayerStructure.RemovedSingles = new Dictionary<ZSingle, int>();
            foreach (ZSingle single in Singles)
            {
                if (singlesDict.Keys.Contains(single.ID))
                    currentLayerStructure.SingleLayers.Add(single.ID, singlesDict[single.ID]);
                else if (singlesDict.Count > 0)
                    currentLayerStructure.SingleLayers.Add(single.ID, singlesDict.FirstOrDefault().Value);
            }

            // Pair Layers
            currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
            currentLayerStructure.RemovedPairs = new Dictionary<ZPair, int>();
            foreach (ZPair pair in Pairs)
            {
                if (pairsDict.Keys.Contains(pair.ID))
                    currentLayerStructure.PairLayers.Add(pair.ID, pairsDict[pair.ID]);
                else if (pairsDict.Count > 0)
                    currentLayerStructure.PairLayers.Add(pair.ID, pairsDict.FirstOrDefault().Value);
            }

            // ViaSpans.
            currentLayerStructure.RemovedSpans = new Dictionary<ZSpan, int>();
            for (int i = this.Spans.Count - 1; i >= 0; --i)
            {
                ZSpan span = this.Spans[i];
                if (span.FirstLayer.ID == currentLayerStructure.Id || span.LastLayer.ID == currentLayerStructure.Id)
                {
                    currentLayerStructure.RemovedSpans.Add(span, Spans.IndexOf(span));
                }
            }

            return currentLayerStructure;
        }

        public void RemoveLayer(int index)
        {
            bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            ZLayer layer = this.Layers[index];
            this.RemoveLayer(layer.ID);
        }

        public void RemoveLayer(string id, bool isCoreLocked = false)
        {
            ZLayer layer = this.Layers.Find(x => x.ID == id);
            if (layer == null) return;

            ZLayer mirrorLayer = this.Layers[Layers.Count - 1 - layer.Order];

            // Clear Layer Selection
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            ZPlannerManager.SuspendCollectionChangedEvent();
            try
            {
                List<LayersCommandStructure> layerStructureList = new List<LayersCommandStructure>();
                LayersCommandStructure currentLayerStructure = new LayersCommandStructure();
                LayersCommandStructure mirrorLayerStructure = new LayersCommandStructure();

                // LockingCore
                if (isCoreLocked && layer.GetLayerType() == ZLayerType.Core)
                {                    
                    if (layer.Order < Layers.Count - 1)
                    {
                        ZLayer copperLayer = Layers[layer.Order + 1];
                        if (copperLayer != null && ZConstants.CopperTypeList.Contains(copperLayer.GetLayerType()))
                        {
                            LayersCommandStructure copperLayerStructure = GetLayersCommandStructureByID(copperLayer.ID);
                            if (copperLayerStructure.StackupLayer != null && !copperLayerStructure.SingleLayers.ContainsValue(null) && !copperLayerStructure.PairLayers.ContainsValue(null))
                            {
                                InternalRemoveLayer(copperLayerStructure);
                                layerStructureList.Add(copperLayerStructure);
                            }
                        }
                        
                    }

                    if (layer.Order > 0)
                    {
                        ZLayer copperLayer = Layers[layer.Order - 1];
                        if (copperLayer != null && ZConstants.CopperTypeList.Contains(copperLayer.GetLayerType()))
                        {
                            LayersCommandStructure copperLayerStructure = GetLayersCommandStructureByID(copperLayer.ID);
                            if (copperLayerStructure.StackupLayer != null && !copperLayerStructure.SingleLayers.ContainsValue(null) && !copperLayerStructure.PairLayers.ContainsValue(null))
                            {
                                InternalRemoveLayer(copperLayerStructure);
                                layerStructureList.Add(copperLayerStructure);
                            }
                        }

                    }
                }

                currentLayerStructure = GetLayersCommandStructureByID(layer.ID);
                if (currentLayerStructure.StackupLayer != null && !currentLayerStructure.SingleLayers.ContainsValue(null) && !currentLayerStructure.PairLayers.ContainsValue(null))
                {
                    InternalRemoveLayer(currentLayerStructure);
                    layerStructureList.Add(currentLayerStructure);
                }

                if (ZPlannerManager.StackupPanel != null && ZPlannerManager.IsAutoMirror && layer.ID != mirrorLayer.ID && mirrorLayer != null && !IsMirroredByTypesAndValues())
                {
                    if (isCoreLocked && mirrorLayer.GetLayerType() == ZLayerType.Core)
                    {
                        if (mirrorLayer.Order < Layers.Count - 1)
                        {
                            ZLayer copperLayer = Layers[mirrorLayer.Order + 1];
                            if (copperLayer != null && ZConstants.CopperTypeList.Contains(copperLayer.GetLayerType()))
                            {
                                LayersCommandStructure copperLayerStructure = GetLayersCommandStructureByID(copperLayer.ID);
                                if (copperLayerStructure.StackupLayer != null && !copperLayerStructure.SingleLayers.ContainsValue(null) && !copperLayerStructure.PairLayers.ContainsValue(null))
                                {
                                    InternalRemoveLayer(copperLayerStructure);
                                    layerStructureList.Add(copperLayerStructure);
                                }
                            }

                        }

                        if (mirrorLayer.Order > 0)
                        {
                            ZLayer copperLayer = Layers[mirrorLayer.Order - 1];
                            if (copperLayer != null && ZConstants.CopperTypeList.Contains(copperLayer.GetLayerType()))
                            {
                                LayersCommandStructure copperLayerStructure = GetLayersCommandStructureByID(copperLayer.ID);
                                if (copperLayerStructure.StackupLayer != null && !copperLayerStructure.SingleLayers.ContainsValue(null) && !copperLayerStructure.PairLayers.ContainsValue(null))
                                {
                                    InternalRemoveLayer(copperLayerStructure);
                                    layerStructureList.Add(copperLayerStructure);
                                }
                            }

                        }
                    }

                    mirrorLayerStructure = GetLayersCommandStructureByID(mirrorLayer.ID);
                    if (mirrorLayerStructure.StackupLayer != null && !mirrorLayerStructure.SingleLayers.ContainsValue(null) && !mirrorLayerStructure.PairLayers.ContainsValue(null))
                    {
                        InternalRemoveLayer(mirrorLayerStructure);
                        layerStructureList.Add(mirrorLayerStructure);
                    }
                }

                ZPlannerManager.ResumeCollectionChangedEvent();
                Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveLayerCommand(this, layerStructureList);
            }
            finally
            {
                ZPlannerManager.ResumeCollectionChangedEvent();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public void RemoveLayers(List<string> ids)
        {
            // Clear Layer Selection
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            ZPlannerManager.SuspendCollectionChangedEvent();
            try
            {
                List<LayersCommandStructure> layerStructureList = new List<LayersCommandStructure>();

                foreach (string id in ids)
                {
                    ZLayer layer = this.Layers.Find(x => x.ID == id);
                    if (layer == null) return;

                    ZLayer mirrorLayer = this.Layers[Layers.Count - 1 - layer.Order];

                    LayersCommandStructure currentLayerStructure = new LayersCommandStructure();
                    LayersCommandStructure mirrorLayerStructure = new LayersCommandStructure();

                    currentLayerStructure = GetLayersCommandStructureByID(layer.ID);
                    if (currentLayerStructure.StackupLayer != null && !currentLayerStructure.SingleLayers.ContainsValue(null) && !currentLayerStructure.PairLayers.ContainsValue(null))
                    {
                        InternalRemoveLayer(currentLayerStructure, false);
                        layerStructureList.Add(currentLayerStructure);
                    }

                    if (ZPlannerManager.StackupPanel != null && ZPlannerManager.IsAutoMirror && layer.ID != mirrorLayer.ID && mirrorLayer != null && !IsMirroredByTypesAndValues())
                    {
                        mirrorLayerStructure = GetLayersCommandStructureByID(mirrorLayer.ID);
                        if (mirrorLayerStructure.StackupLayer != null && !mirrorLayerStructure.SingleLayers.ContainsValue(null) && !mirrorLayerStructure.PairLayers.ContainsValue(null))
                        {
                            InternalRemoveLayer(mirrorLayerStructure, false);
                            layerStructureList.Add(mirrorLayerStructure);
                        }
                    }
                }

                ZPlannerManager.ResumeCollectionChangedEvent();
                Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveLayerCommand(this, layerStructureList);
            }
            finally
            {
                ZPlannerManager.ResumeCollectionChangedEvent();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        internal void InternalRemoveLayer(LayersCommandStructure layerStructure, bool isSetSelection = true)
        {
            // Stackup Layer.
            if (layerStructure.StackupLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");
            Layers.Remove(layerStructure.StackupLayer);

            // Single Layer.
            foreach (ZSingle single in Singles)
            {
                ZLayer singleLayer = layerStructure.SingleLayers.ContainsKey(single.ID) ? layerStructure.SingleLayers[single.ID] : null;
                if (singleLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                single.Layers.Remove(singleLayer);
            }

            if (layerStructure.RemovedSingles != null)
            {
                foreach (ZSingle single in layerStructure.RemovedSingles.Keys)
                {
                    this.Singles.Remove(single); //.RemoveAt(i);
                }
            }

            // Pair Layer.
            foreach(ZPair pair in Pairs)
            {
                ZLayer pairLayer = layerStructure.PairLayers.ContainsKey(pair.ID) ? layerStructure.PairLayers[pair.ID] : null;
                if (pairLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");
                
                pair.Layers.Remove(pairLayer);
            }

            if (layerStructure.RemovedPairs != null)
            {
                foreach (ZPair pair in layerStructure.RemovedPairs.Keys)
                {
                    this.Pairs.Remove(pair); //.RemoveAt(i);
                }
            }

            // ViaSpans.
            if (layerStructure.RemovedSpans != null)
            {
                foreach (ZSpan span in layerStructure.RemovedSpans.Keys)
                {
                    this.Spans.Remove(span); //.RemoveAt(i);
                }
            }

            // Select Layer
            if (ZPlannerManager.StackupPanel != null && isSetSelection) ZPlannerManager.StackupPanel.AddRowSelection(layerStructure.Index);
        }

        public void MoveLayer(int sourceIndex, int destinationIndex)
        {
            bool isIndexIncorrect = (sourceIndex < 0 || sourceIndex > this.Layers.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified source index is out of range.");

            ZLayer layer = this.Layers[sourceIndex];
            this.MoveLayer(layer.ID, destinationIndex);
        }

        public void MoveLayer(string id, int index, bool isCoreLocked = false)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            ZPlannerManager.SuspendCollectionChangedEvent();
            try
            {
                List<MoveCommandStructure> moveStructureList = new List<MoveCommandStructure>();
                MoveCommandStructure currentMoveStructure = new MoveCommandStructure();
                MoveCommandStructure mirrorMoveStructure = new MoveCommandStructure();

                // Stackup Layer.
                ZLayer layer = this.Layers.Find(x => x.ID == id);
                if (layer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                Dictionary<string, LayersCommandStructure> underList = new Dictionary<string,LayersCommandStructure>();
                Dictionary<string, LayersCommandStructure> overList = new Dictionary<string,LayersCommandStructure>();
                
                // LockingCore
                if (isCoreLocked)
                {
                    CollapseLockedLayers(underList, overList);
                }

                bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);
                
                foreach (ZSingle single in this.Singles)
                {
                    isIndexIncorrect = isIndexIncorrect || index > single.Layers.Count;
                }
                
                foreach (ZPair pair in this.Pairs)
                {
                    isIndexIncorrect = isIndexIncorrect || index > pair.Layers.Count;
                }

                if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified destination index is out of range.");

                currentMoveStructure.SourceIndex = layer.Order;
                currentMoveStructure.DestinIndex = index;
                currentMoveStructure.Id = id;

                bool isMoveUp = layer.Order > index;

                InternalMoveLayer(currentMoveStructure);
                moveStructureList.Add(currentMoveStructure);

                if (ZPlannerManager.StackupPanel != null && ZPlannerManager.IsAutoMirror)
                {
                    mirrorMoveStructure.SourceIndex = Layers.Count - 1 - currentMoveStructure.SourceIndex;
                    if (mirrorMoveStructure.SourceIndex > currentMoveStructure.SourceIndex && mirrorMoveStructure.SourceIndex <= currentMoveStructure.DestinIndex) --mirrorMoveStructure.SourceIndex;
                    if (mirrorMoveStructure.SourceIndex < currentMoveStructure.SourceIndex && mirrorMoveStructure.SourceIndex >= currentMoveStructure.DestinIndex) ++mirrorMoveStructure.SourceIndex;

                    mirrorMoveStructure.DestinIndex = Layers.Count - 1 - currentMoveStructure.DestinIndex;
                    if (mirrorMoveStructure.SourceIndex > currentMoveStructure.DestinIndex && mirrorMoveStructure.DestinIndex <= currentMoveStructure.DestinIndex) --mirrorMoveStructure.DestinIndex;
                    if (mirrorMoveStructure.SourceIndex < currentMoveStructure.DestinIndex && mirrorMoveStructure.DestinIndex >= currentMoveStructure.DestinIndex) ++mirrorMoveStructure.DestinIndex;

                    if (mirrorMoveStructure.SourceIndex >= 0 && mirrorMoveStructure.SourceIndex < Layers.Count && mirrorMoveStructure.SourceIndex != mirrorMoveStructure.DestinIndex)
                    {
                        ZLayer layerToMirror = this.Layers[mirrorMoveStructure.SourceIndex];
                        if (layerToMirror != null)
                        {
                            mirrorMoveStructure.Id = layerToMirror.ID;

                            InternalMoveLayer(mirrorMoveStructure);
                            moveStructureList.Add(mirrorMoveStructure);
                        }
                    }
                }

                // LockingCore
                if (isCoreLocked)
                {
                    ExpandLockedLayers(underList, overList);
                }

                ZPlannerManager.ResumeCollectionChangedEvent();
                Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new MoveLayerCommand(this, moveStructureList, isCoreLocked);
            }
            finally
            {
                ZPlannerManager.ResumeCollectionChangedEvent();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        internal void CollapseLockedLayers(Dictionary<string, LayersCommandStructure> underList, Dictionary<string, LayersCommandStructure> overList)
        {
            for (int i = Layers.Count - 1; i >= 0; --i)
            {
                if (Layers[i].GetLayerType() != ZLayerType.Core) continue;

                string id = Layers[i].ID;

                int index = i + 1;
                if (Layers.Count > index && ZConstants.CopperTypeList.Contains(Layers[index].GetLayerType()))
                {
                    LayersCommandStructure currentLayerStructure = new LayersCommandStructure();

                    currentLayerStructure.Id = Layers[index].ID;
                    currentLayerStructure.Index = index;
                    // Stackup Layer.
                    currentLayerStructure.StackupLayer = Layers[index];
                    if (currentLayerStructure.StackupLayer == null) continue;
                    Layers.Remove(currentLayerStructure.StackupLayer);
                    // Single Layer.
                    currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                    for (int s = this.Singles.Count - 1; s >= 0; --s)
                    {
                        ZSingle single = this.Singles[s];
                        ZLayer singleLayer = single.Layers[index];
                        if (singleLayer == null) continue;
                        single.Layers.Remove(singleLayer);
                        currentLayerStructure.SingleLayers.Add(single.ID, singleLayer);
                    }
                    // Pair Layers
                    currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                    for (int p = this.Pairs.Count - 1; p >= 0; --p)
                    {
                        ZPair pair = this.Pairs[p];
                        ZLayer pairLayer = pair.Layers[index];
                        if (pairLayer == null) continue;
                        pair.Layers.Remove(pairLayer);
                        currentLayerStructure.PairLayers.Add(pair.ID, pairLayer);                        
                    }

                    overList.Add(id, currentLayerStructure);
                }

                index = i - 1;
                if (index >= 0 && ZConstants.CopperTypeList.Contains(Layers[index].GetLayerType()))
                {
                    LayersCommandStructure currentLayerStructure = new LayersCommandStructure();

                    currentLayerStructure.Id = Layers[index].ID;
                    currentLayerStructure.Index = index;
                    // Stackup Layer.
                    currentLayerStructure.StackupLayer = Layers[index];
                    if (currentLayerStructure.StackupLayer == null) continue;
                    Layers.Remove(currentLayerStructure.StackupLayer);
                    // Single Layer.
                    currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                    for (int s = this.Singles.Count - 1; s >= 0; --s)
                    {
                        ZSingle single = this.Singles[s];
                        ZLayer singleLayer = single.Layers[index];
                        if (singleLayer == null) continue;
                        single.Layers.Remove(singleLayer);
                        currentLayerStructure.SingleLayers.Add(single.ID, singleLayer);
                    }
                    // Pair Layers
                    currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                    for (int p = this.Pairs.Count - 1; p >= 0; --p)
                    {
                        ZPair pair = this.Pairs[p];
                        ZLayer pairLayer = pair.Layers[index];
                        if (pairLayer == null) continue;
                        pair.Layers.Remove(pairLayer);
                        currentLayerStructure.PairLayers.Add(pair.ID, pairLayer);
                    }

                    underList.Add(id, currentLayerStructure);
                    --i;
                }
            }
        }

        internal void ExpandLockedLayers(Dictionary<string, LayersCommandStructure> underList, Dictionary<string, LayersCommandStructure> overList)
        {
            for (int i = Layers.Count - 1; i >= 0; --i)
            {
                if (Layers[i].GetLayerType() != ZLayerType.Core) continue;

                string id = Layers[i].ID;
                int index = Layers[i].Order;

                if (overList.ContainsKey(id))
                {
                    LayersCommandStructure currentLayerStructure = overList[id];
                    currentLayerStructure.Index = index + 1;
                    InternalInsertLayer(currentLayerStructure, false);
                    overList.Remove(id);
                }

                if (underList.ContainsKey(id))
                {
                    LayersCommandStructure currentLayerStructure = underList[id];
                    currentLayerStructure.Index = index;
                    InternalInsertLayer(currentLayerStructure, false);
                    underList.Remove(id);
                }
            }            
        }

        internal void InternalMoveLayer(MoveCommandStructure moveStructure)
        {
            bool isIndexIncorrect = (moveStructure.DestinIndex < 0 || moveStructure.DestinIndex > this.Layers.Count);
            
            foreach (ZSingle single in this.Singles)
            {
                isIndexIncorrect = isIndexIncorrect || moveStructure.DestinIndex > single.Layers.Count;
            }

            foreach (ZPair pair in this.Pairs)
            {
                isIndexIncorrect = isIndexIncorrect || moveStructure.DestinIndex > pair.Layers.Count;
            }

            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified destination index is out of range.");

            // Stackup Layer.
            ZLayer layer = this.Layers.Find(x => x.ID == moveStructure.Id);
            if (layer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

            this.Layers.MoveAt(layer, moveStructure.DestinIndex);

            // Single Layer.
            for (int i = this.Singles.Count - 1; i >= 0; --i)
            {
                ZSingle single = this.Singles[i];
                layer = single.Layers.Find(x => x.ID == moveStructure.Id);
                if (layer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                single.Layers.MoveAt(layer, moveStructure.DestinIndex);
            }

            // Pair Layer.
            for (int i = this.Pairs.Count - 1; i >= 0; --i)
            {
                ZPair pair = this.Pairs[i];
                layer = pair.Layers.Find(x => x.ID == moveStructure.Id);
                if (layer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                pair.Layers.MoveAt(layer, moveStructure.DestinIndex);
            }

            // ViaSpans.
            for (int i = this.Spans.Count - 1; i >= 0; --i)
            {
                ZSpan span = this.Spans[i];
                if (span.FirstLayer.ID == moveStructure.Id || span.LastLayer.ID == moveStructure.Id)
                {
                    int firstIndex = this.Layers.FindIndex(x => x.ID == span.FirstLayer.ID);
                    int lastIndex = this.Layers.FindIndex(x => x.ID == span.LastLayer.ID);
                    if (firstIndex > lastIndex)
                    {
                        ZLayer tempLayer = span.FirstLayer;
                        span.FirstLayer = span.LastLayer;
                        span.LastLayer = tempLayer;
                    }
                }
            }
        }

        public void SwapLayers(string id1, string id2, bool isCoreLocked)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            ZPlannerManager.SuspendCollectionChangedEvent();
            try
            {
                List<SwapCommandStructure> swapStructureList = new List<SwapCommandStructure>();
                SwapCommandStructure currentSwapStructure = new SwapCommandStructure();
                SwapCommandStructure mirrorSwapStructure = new SwapCommandStructure();
            
                Dictionary<string, LayersCommandStructure> underList = new Dictionary<string,LayersCommandStructure>();
                Dictionary<string, LayersCommandStructure> overList = new Dictionary<string,LayersCommandStructure>();
                
                // LockingCore
                if (isCoreLocked)
                {
                    CollapseLockedLayers(underList, overList);
                }

                // Stackup Layer.
                ZLayer layer1 = this.Layers.Find(x => x.ID == id1);
                if (layer1 == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                ZLayer layer2 = this.Layers.Find(x => x.ID == id2);
                if (layer2 == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                currentSwapStructure.Id1 = id1;
                currentSwapStructure.Id2 = id2;
                InternalSwapLayers(currentSwapStructure);
                swapStructureList.Add(currentSwapStructure);

                if (ZPlannerManager.StackupPanel != null && ZPlannerManager.IsAutoMirror)
                {
                    int mirrorIndex1 = Layers.Count - 1 - layer2.Order;
                    int mirrorIndex2 = Layers.Count - 1 - layer1.Order;


                    if (mirrorIndex1 >= 0 && mirrorIndex1 < Layers.Count && mirrorIndex1 != layer1.Order && 
                        mirrorIndex2 >= 0 && mirrorIndex2 < Layers.Count && mirrorIndex2 != layer2.Order)
                    {
                        ZLayer layer1ToMirror = this.Layers[mirrorIndex1];
                        ZLayer layer2ToMirror = this.Layers[mirrorIndex2];
                        if (layer1ToMirror != null && layer2ToMirror != null)
                        {
                            mirrorSwapStructure.Id1 = layer1ToMirror.ID;
                            mirrorSwapStructure.Id2 = layer2ToMirror.ID;
                            InternalSwapLayers(mirrorSwapStructure);
                            swapStructureList.Add(mirrorSwapStructure);
                        }
                    }
                }

                // LockingCore
                if (isCoreLocked)
                {
                    ExpandLockedLayers(underList, overList);
                }

                ZPlannerManager.ResumeCollectionChangedEvent();
                Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new SwapLayersCommand(this, swapStructureList, isCoreLocked);
            }
            finally
            {
                ZPlannerManager.ResumeCollectionChangedEvent();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        internal void InternalSwapLayers(SwapCommandStructure swapStructure)
        {
            // Stackup Layer.
            ZLayer layer1 = this.Layers.Find(x => x.ID == swapStructure.Id1);
            if (layer1 == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

            ZLayer layer2 = this.Layers.Find(x => x.ID == swapStructure.Id2);
            if (layer2 == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

            this.Layers.Swap(layer1, layer2);

            // Single Layer.
            for (int i = this.Singles.Count - 1; i >= 0; --i)
            {
                ZSingle single = this.Singles[i];
                layer1 = single.Layers.Find(x => x.ID == swapStructure.Id1);
                if (layer1 == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                layer2 = single.Layers.Find(x => x.ID == swapStructure.Id2);
                if (layer2 == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                single.Layers.Swap(layer1, layer2);
            }

            // Pair Layer.
            for (int i = this.Pairs.Count - 1; i >= 0; --i)
            {
                ZPair pair = this.Pairs[i];
                layer1 = pair.Layers.Find(x => x.ID == swapStructure.Id1);
                if (layer1 == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                layer2 = pair.Layers.Find(x => x.ID == swapStructure.Id2);
                if (layer2 == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                pair.Layers.Swap(layer1, layer2);
            }

            // ViaSpans.
            for (int i = this.Spans.Count - 1; i >= 0; --i)
            {
                ZSpan span = this.Spans[i];
                if (span.FirstLayer.ID == swapStructure.Id1 || span.LastLayer.ID == swapStructure.Id1 || span.FirstLayer.ID == swapStructure.Id2 || span.LastLayer.ID == swapStructure.Id2)
                {
                    int firstIndex = this.Layers.FindIndex(x => x.ID == span.FirstLayer.ID);
                    int lastIndex = this.Layers.FindIndex(x => x.ID == span.LastLayer.ID);
                    if (firstIndex > lastIndex)
                    {
                        ZLayer tempLayer = span.FirstLayer;
                        span.FirstLayer = span.LastLayer;
                        span.LastLayer = tempLayer;
                    }
                }
            }
        }

        public void DuplicateLayer(string id)
        {
            List<string> ids = new List<string>();
            ids.Add(id);
            CopyLayers(ids);
            PasteLayers();
        }

        public void CopyLayer(int index)
        {
            bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            ZLayer layer = this.Layers[index];
            this.CopyLayer(layer.ID);
        }

        public void CopyLayer(string id)
        {
            List<string> ids = new List<string>();
            ids.Add(id);
            CopyLayers(ids);
        }

        public void CopyLayers(List<string> ids)
        {
            ZPlannerManager.Clipboard.Clear();

            List<ZEntity> stackupLayers = new List<ZEntity>();
            Dictionary<string, List<ZEntity>> singlesDict = new Dictionary<string, List<ZEntity>>();
            Dictionary<string, List<ZEntity>> pairsDict = new Dictionary<string, List<ZEntity>>();
            
            foreach (string id in ids)
            {
                // Stackup Layer.
                ZLayer stackupLayer = this.Layers.Find(x => x.ID == id);
                if (stackupLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                stackupLayers.Add(stackupLayer.Clone());
            }

            foreach (ZSingle single in this.Singles)
            {
                List<ZEntity> singleLayers = new List<ZEntity>();
                foreach (string id in ids)
                {
                    // Single Layer.
                    ZLayer singleLayer = single.Layers.Find(x => x.ID == id);
                    if (singleLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                    singleLayers.Add(singleLayer.Clone());
                }

                singlesDict.Add(single.ID, singleLayers);
            }

            foreach (ZPair pair in this.Pairs)
            {
                List<ZEntity> pairLayers = new List<ZEntity>();
                foreach (string id in ids)
                {
                    // Pair Layer.
                    ZLayer pairLayer = pair.Layers.Find(x => x.ID == id);
                    if (pairLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                    pairLayers.Add(pairLayer.Clone());
                }

                pairsDict.Add(pair.ID, pairLayers);
            }

            if (stackupLayers.Count > 0)
            {
                ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeyStackupLayers, stackupLayers);

                foreach (string singleId in singlesDict.Keys)
                {
                    ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeySingleLayers + singleId, singlesDict[singleId]);
                }

                foreach(string pairId in pairsDict.Keys)
                {
                    ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeyPairLayers + pairId, pairsDict[pairId]);
                }
            }
        }

        public void CutLayer(int index)
        {
            bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            ZLayer layer = this.Layers[index];
            this.CutLayer(layer.ID);
        }

        public void CutLayer(string id)
        {
            List<string> ids = new List<string>();
            ids.Add(id);
            CutLayers(ids);
        }

        public void CutLayers(List<string> ids)
        {

            ZPlannerManager.Clipboard.Clear();

            List<ZEntity> stackupLayers = new List<ZEntity>();
            Dictionary<string, List<ZEntity>> singlesDict = new Dictionary<string, List<ZEntity>>();
            Dictionary<string, List<ZEntity>> pairsDict = new Dictionary<string, List<ZEntity>>();

            foreach (string id in ids)
            {
                // Stackup Layer.
                ZLayer stackupLayer = this.Layers.Find(x => x.ID == id);
                if (stackupLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");
                stackupLayers.Add(stackupLayer);
            }

            foreach (ZSingle single in this.Singles)
            {
                List<ZEntity> singleLayers = new List<ZEntity>();
                foreach (string id in ids)
                {
                    // Single Layer.
                    ZLayer singleLayer = single.Layers.Find(x => x.ID == id);
                    if (singleLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                    singleLayers.Add(singleLayer);
                }

                singlesDict.Add(single.ID, singleLayers);
            }

            foreach (ZPair pair in this.Pairs)
            {
                List<ZEntity> pairLayers = new List<ZEntity>();
                foreach (string id in ids)
                {
                    // Pair Layer.
                    ZLayer pairLayer = pair.Layers.Find(x => x.ID == id);
                    if (pairLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                    pairLayers.Add(pairLayer);
                }

                pairsDict.Add(pair.ID, pairLayers);
            }

            this.RemoveLayers(ids);

            if (stackupLayers.Count > 0)
            {
                ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeyStackupLayers, stackupLayers);

                foreach (string singleId in singlesDict.Keys)
                {
                    ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeySingleLayers + singleId, singlesDict[singleId]);
                }

                foreach (string pairId in pairsDict.Keys)
                {
                    ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeyPairLayers + pairId, pairsDict[pairId]);
                }
            }
        }

        public void PasteLayers(int index = -1, bool isCoreLocked = false)
        {
            if (!ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyStackupLayers)) return;
            List<ZEntity> stackupLayers = ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyStackupLayers] as List<ZEntity>;
            Dictionary<string, List<ZEntity>> singlesDict = new Dictionary<string, List<ZEntity>>();
            Dictionary<string, List<ZEntity>> pairsDict = new Dictionary<string, List<ZEntity>>();

            foreach (string key in ZPlannerManager.Clipboard.Keys)
            {
                if (key.StartsWith(ZStringConstants.ClipbordKeySingleLayers)) singlesDict.Add(key.Substring(ZStringConstants.ClipbordKeySingleLayers.Length), ZPlannerManager.Clipboard[key] as List<ZEntity>);
                else if (key.StartsWith(ZStringConstants.ClipbordKeyPairLayers)) pairsDict.Add(key.Substring(ZStringConstants.ClipbordKeyPairLayers.Length), ZPlannerManager.Clipboard[key] as List<ZEntity>);
            }

            if (stackupLayers == null || stackupLayers.Count <= 0)
            {
                ZPlannerManager.Clipboard.Clear();
                return;
            }

            foreach (List<ZEntity> singleLayers in singlesDict.Values)
            {
                if (singleLayers == null || stackupLayers.Count != singleLayers.Count)
                {
                    ZPlannerManager.Clipboard.Clear();
                    return;
                }
            }

            foreach (List<ZEntity> pairLayers in pairsDict.Values)
            {
                if (pairLayers == null || stackupLayers.Count != pairLayers.Count)
                {
                    ZPlannerManager.Clipboard.Clear();
                    return;
                }
            }

            if (index < 0) 
            {
                ZLayer lastStackupLayer = stackupLayers[stackupLayers.Count - 1] as ZLayer;
                if (lastStackupLayer != null) index = lastStackupLayer.Order + 1;
            }

            // Clear Layer Selection
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            ZPlannerManager.SuspendCollectionChangedEvent();
            try
            {
                int count = this.Layers.Count;
                ZLayer middleLayer = (count > 0) ? Layers[(int)(count * 0.5)] : null;
                ZLayerType? middleLayerType = (middleLayer != null) ? middleLayer.GetLayerType() : null;
                List<LayersCommandStructure> layerStructureList = new List<LayersCommandStructure>();

                Dictionary<string, LayersCommandStructure> underList = new Dictionary<string, LayersCommandStructure>();
                Dictionary<string, LayersCommandStructure> overList = new Dictionary<string, LayersCommandStructure>();

                // LockingCore
                if (isCoreLocked)
                {
                    CollapseLockedLayers(underList, overList);
                }

                bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);
                
                foreach (ZSingle single in this.Singles)
                {
                    isIndexIncorrect = isIndexIncorrect || index > single.Layers.Count;
                }

                foreach (ZPair pair in this.Pairs)
                {
                    isIndexIncorrect = isIndexIncorrect || index > pair.Layers.Count;
                }

                if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

                for (int i = 0; i < stackupLayers.Count; ++i)
                {   
                    LayersCommandStructure currentLayerStructure = new LayersCommandStructure();
                    LayersCommandStructure mirrorLayerStructure = new LayersCommandStructure();

                    currentLayerStructure.Index = index;

                    // Stackup Layers.
                    currentLayerStructure.StackupLayer = stackupLayers[i] as ZLayer;
                    if (currentLayerStructure.StackupLayer == null) continue;
                    currentLayerStructure.StackupLayer = currentLayerStructure.StackupLayer.Clone();
                    currentLayerStructure.Id = currentLayerStructure.StackupLayer.ID;

                    // Insert Stackup Layer at the end of the code block, because Grid redraw after insert of Stackup Layer.

                    // Single Layers.
                    currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();

                    List<ZParameter> singleSubparameters = new List<ZParameter>();
                    foreach (ZParameter singleParameter in Project.Parameters.FindAll(x => x.Table == ZTableType.Single))
                    {
                        if (singleParameter.SubParameters != null && singleParameter.SubParameters.Count > 0) singleSubparameters.AddRange(singleParameter.SubParameters);
                        else singleSubparameters.Add(singleParameter);
                    }

                    foreach (ZSingle single in this.Singles)
                    {
                        ZLayer singleLayer = null;

                        if (singlesDict.Keys.Contains(single.ID))
                        {
                            singleLayer = (singlesDict[single.ID])[i] as ZLayer;
                        }
                        else
                        {
                            if (singlesDict.Count > 0)
                                singleLayer = (singlesDict.First().Value)[i] as ZLayer;
                        }

                        if (singleLayer != null)
                        {
                            singleLayer = singleLayer.Clone();
                            singleLayer.ID = currentLayerStructure.StackupLayer.ID;
                        }
                        else
                        {
                            if (middleLayer != null && middleLayerType == currentLayerStructure.StackupLayer.GetLayerType())
                            {
                                singleLayer = GetLayerOfSingleImpedance(middleLayer.ID, single.ID).Clone();
                                singleLayer.ID = currentLayerStructure.StackupLayer.ID;
                            }
                            else
                            {
                                singleLayer = new ZLayer(currentLayerStructure.StackupLayer.ID, this, single);
                                singleLayer.IsFrozen = currentLayerStructure.StackupLayer.IsFrozen;
                                singleLayer.IsCustom = currentLayerStructure.StackupLayer.IsCustom;
                                singleLayer.IsEdited = currentLayerStructure.StackupLayer.IsEdited;
                                singleLayer.IsHidden = currentLayerStructure.StackupLayer.IsHidden;
                                singleLayer.IsReadOnly = currentLayerStructure.StackupLayer.IsReadOnly;
                                singleLayer.Height = currentLayerStructure.StackupLayer.Height;

                                foreach (ZParameter parameter in singleSubparameters)
                                {
                                    ZLayerType? layerType = currentLayerStructure.StackupLayer.GetLayerType();
                                    ZLayerParameter layerParameter = (layerType != null) ? new ZLayerParameter(parameter, singleLayer, (ZLayerType)layerType) : new ZLayerParameter(parameter, singleLayer);

                                    layerParameter.Layer = singleLayer;
                                    layerParameter.Parameter = parameter;

                                    singleLayer.LayerParameters.Add(layerParameter);
                                }
                            }
                        }

                        currentLayerStructure.SingleLayers.Add(single.ID, singleLayer);
                    }

                    // Pair Layers.
                    currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();

                    List<ZParameter> pairSubparameters = new List<ZParameter>();
                    foreach (ZParameter pairParameter in Project.Parameters.FindAll(x => x.Table == ZTableType.Pair))
                    {
                        if (pairParameter.SubParameters != null && pairParameter.SubParameters.Count > 0) pairSubparameters.AddRange(pairParameter.SubParameters);
                        else pairSubparameters.Add(pairParameter);
                    }

                    foreach (ZPair pair in this.Pairs)
                    {
                        ZLayer pairLayer = null;

                        if (pairsDict.Keys.Contains(pair.ID))
                        {
                            pairLayer = (pairsDict[pair.ID])[i] as ZLayer;
                        }
                        else
                        {
                            if (pairsDict.Count > 0)
                                pairLayer = (pairsDict.First().Value)[i] as ZLayer;
                        }

                        if (pairLayer != null)
                        {
                            pairLayer = pairLayer.Clone();
                            pairLayer.ID = currentLayerStructure.StackupLayer.ID;
                        }
                        else
                        {
                            if (middleLayer != null && middleLayerType == currentLayerStructure.StackupLayer.GetLayerType())
                            {
                                pairLayer = GetLayerOfPairImpedance(middleLayer.ID, pair.ID).Clone();
                                pairLayer.ID = currentLayerStructure.StackupLayer.ID;
                            }
                            else
                            {
                                pairLayer = new ZLayer(currentLayerStructure.StackupLayer.ID, this, pair);
                                pairLayer.IsFrozen = currentLayerStructure.StackupLayer.IsFrozen;
                                pairLayer.IsCustom = currentLayerStructure.StackupLayer.IsCustom;
                                pairLayer.IsEdited = currentLayerStructure.StackupLayer.IsEdited;
                                pairLayer.IsHidden = currentLayerStructure.StackupLayer.IsHidden;
                                pairLayer.IsReadOnly = currentLayerStructure.StackupLayer.IsReadOnly;
                                pairLayer.Height = currentLayerStructure.StackupLayer.Height;

                                foreach (ZParameter parameter in pairSubparameters)
                                {
                                    ZLayerType? layerType = currentLayerStructure.StackupLayer.GetLayerType();
                                    ZLayerParameter layerParameter = (layerType != null) ? new ZLayerParameter(parameter, pairLayer, (ZLayerType)layerType) : new ZLayerParameter(parameter, pairLayer);

                                    layerParameter.Layer = pairLayer;
                                    layerParameter.Parameter = parameter;

                                    pairLayer.LayerParameters.Add(layerParameter);
                                }
                            }
                        }

                        currentLayerStructure.PairLayers.Add(pair.ID, pairLayer);
                    }

                    int layersCount = Layers.Count;
                    mirrorLayerStructure.Index = layersCount - index;

                    InternalInsertLayer(currentLayerStructure, i == stackupLayers.Count - 1);
                    layerStructureList.Add(currentLayerStructure);

                    if (ZPlannerManager.StackupPanel != null && ZPlannerManager.IsAutoMirror &&  !IsMirroredByTypesAndValues())
                    {
                        if (mirrorLayerStructure.Index >= currentLayerStructure.Index) mirrorLayerStructure.Index += Layers.Count - layersCount;

                        if (mirrorLayerStructure.Index >= 0 && mirrorLayerStructure.Index != currentLayerStructure.Index)
                        {
                            mirrorLayerStructure.StackupLayer = currentLayerStructure.StackupLayer.Clone();
                            mirrorLayerStructure.Id = mirrorLayerStructure.StackupLayer.ID;
                            
                            mirrorLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                            foreach (string id in currentLayerStructure.SingleLayers.Keys)
                            {
                                mirrorLayerStructure.SingleLayers.Add(id, currentLayerStructure.SingleLayers[id].Clone());
                            }

                            mirrorLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                            foreach (string id in currentLayerStructure.PairLayers.Keys)
                            {
                                mirrorLayerStructure.PairLayers.Add(id, currentLayerStructure.PairLayers[id].Clone());
                            }

                            InternalInsertLayer(mirrorLayerStructure, i == stackupLayers.Count - 1);
                            layerStructureList.Add(mirrorLayerStructure);
                        }

                        if (mirrorLayerStructure.Index < currentLayerStructure.Index) ++index; // Move index after mirror layer paste. 
                    }

                    ++index;
                }

                // LockingCore
                if (isCoreLocked)
                {
                    ExpandLockedLayers(underList, overList);
                }

                ZPlannerManager.ResumeCollectionChangedEvent();
                Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new PasteLayerCommand(this, layerStructureList);
            }
            finally
            {
                ZPlannerManager.ResumeCollectionChangedEvent();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }

            //ZPlannerManager.Clipboard.Clear();
        }

        /*public void ReplaceLayers(int index = -1, bool isCoreLocked = false)
        {
            if (!ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyStackupLayers)) return;
            List<ZEntity> stackupLayers = ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyStackupLayers] as List<ZEntity>;
            Dictionary<string, List<ZEntity>> singlesDict = new Dictionary<string, List<ZEntity>>();
            Dictionary<string, List<ZEntity>> pairsDict = new Dictionary<string, List<ZEntity>>();

            foreach (string key in ZPlannerManager.Clipboard.Keys)
            {
                if (key.StartsWith(ZStringConstants.ClipbordKeySingleLayers)) singlesDict.Add(key.Substring(ZStringConstants.ClipbordKeySingleLayers.Length), ZPlannerManager.Clipboard[key] as List<ZEntity>);
                else if (key.StartsWith(ZStringConstants.ClipbordKeyPairLayers)) pairsDict.Add(key.Substring(ZStringConstants.ClipbordKeyPairLayers.Length), ZPlannerManager.Clipboard[key] as List<ZEntity>);
            }

            if (stackupLayers == null || stackupLayers.Count < 1)
            {
                ZPlannerManager.Clipboard.Clear();
                return;
            }

            foreach (List<ZEntity> singleLayers in singlesDict.Values)
            {
                if (singleLayers == null || stackupLayers.Count != singleLayers.Count)
                {
                    ZPlannerManager.Clipboard.Clear();
                    return;
                }
            }

            foreach (List<ZEntity> pairLayers in pairsDict.Values)
            {
                if (pairLayers == null || stackupLayers.Count != pairLayers.Count)
                {
                    ZPlannerManager.Clipboard.Clear();
                    return;
                }
            }

            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            ZPlannerManager.SuspendCollectionChangedEvent();
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            bool isAutoMirror = ZPlannerManager.IsAutoMirror;
            ZPlannerManager.IsAutoMirror = false;

            try
            {
                // Clear Layer Selection
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

                int count = this.Layers.Count;
                ZLayer middleLayer = (count > 0) ? Layers[(int)(count * 0.5)] : null;
                ZLayerType? middleLayerType = (middleLayer != null) ? middleLayer.GetLayerType() : null;
                Dictionary<LayersCommandStructure, LayersCommandStructure> layerStructureDictionary = new Dictionary<LayersCommandStructure,LayersCommandStructure>();
                List<LayersCommandStructure> layerStructureList = new List<LayersCommandStructure>();

                Dictionary<string, LayersCommandStructure> underList = new Dictionary<string, LayersCommandStructure>();
                Dictionary<string, LayersCommandStructure> overList = new Dictionary<string, LayersCommandStructure>();

                // LockingCore
                if (isCoreLocked)
                {
                    CollapseLockedLayers(underList, overList);
                }

                bool isIndexIncorrect = (index < 0 || index > this.Layers.Count);

                foreach (ZSingle single in this.Singles)
                {
                    isIndexIncorrect = isIndexIncorrect || index > single.Layers.Count;
                }

                foreach (ZPair pair in this.Pairs)
                {
                    isIndexIncorrect = isIndexIncorrect || index > pair.Layers.Count;
                }

                if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

                ZLayer currentStackupLayer = null;
                ZLayer mirrorStackupLayer = null;

                int i = 0;
                
                LayersCommandStructure currentNewLayerStructure = new LayersCommandStructure();
                LayersCommandStructure mirrorNewLayerStructure = new LayersCommandStructure();
                LayersCommandStructure currentOldLayerStructure = new LayersCommandStructure();
                LayersCommandStructure mirrorOldLayerStructure = new LayersCommandStructure();

                currentStackupLayer = Layers[index];
                if (currentStackupLayer == null) return;

                currentNewLayerStructure.Index = index;
                currentOldLayerStructure.Index = index;

                // Stackup Layers.
                currentNewLayerStructure.StackupLayer = stackupLayers[i] as ZLayer;
                if (currentNewLayerStructure.StackupLayer == null) return;

                bool isLayerTypeChanged = (currentStackupLayer.GetLayerType() != currentNewLayerStructure.StackupLayer.GetLayerType());

                currentNewLayerStructure.StackupLayer = currentNewLayerStructure.StackupLayer.Clone();
                currentNewLayerStructure.StackupLayer.ID = currentStackupLayer.ID;
                currentNewLayerStructure.Id = currentStackupLayer.ID;

                currentOldLayerStructure.StackupLayer = currentStackupLayer.Clone();
                currentOldLayerStructure.StackupLayer.ID = currentStackupLayer.ID;
                currentOldLayerStructure.Id = currentStackupLayer.ID;

                if (isLayerTypeChanged)
                    currentNewLayerStructure.StackupLayer.SetDefaultLayerParameterValue(ZStringConstants.ParameterIDLayerName);
                else
                    currentNewLayerStructure.StackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, currentStackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName));

                // Insert Stackup Layer at the end of the code block, because Grid redraw after insert of Stackup Layer.

                // Single Layers.
                currentNewLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                currentOldLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();

                List<ZParameter> singleSubparameters = new List<ZParameter>();
                foreach (ZParameter singleParameter in Project.Parameters.FindAll(x => x.Table == ZTableType.Single))
                {
                    if (singleParameter.SubParameters != null && singleParameter.SubParameters.Count > 0) singleSubparameters.AddRange(singleParameter.SubParameters);
                    else singleSubparameters.Add(singleParameter);
                }

                foreach (ZSingle single in this.Singles)
                {
                    ZLayer currentSingleLayer = single.Layers[index];
                    if (currentSingleLayer == null) return;

                    ZLayer singleLayer = null;

                    if (singlesDict.Keys.Contains(single.ID))
                    {
                        singleLayer = (singlesDict[single.ID])[i] as ZLayer;
                    }
                    else
                    {
                        if (singlesDict.Count > 0)
                            singleLayer = (singlesDict.First().Value)[i] as ZLayer;
                    }

                    if (singleLayer == null) return;
                        
                    singleLayer = singleLayer.Clone();
                    singleLayer.ID = currentSingleLayer.ID;
                    currentNewLayerStructure.SingleLayers.Add(single.ID, singleLayer);

                    currentOldLayerStructure.SingleLayers.Add(single.ID, currentSingleLayer.Clone());
                    currentOldLayerStructure.SingleLayers[single.ID].ID = currentSingleLayer.ID;
                }

                // Pair Layers.
                currentNewLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                currentOldLayerStructure.PairLayers = new Dictionary<string, ZLayer>();

                List<ZParameter> pairSubparameters = new List<ZParameter>();
                foreach (ZParameter pairParameter in Project.Parameters.FindAll(x => x.Table == ZTableType.Pair))
                {
                    if (pairParameter.SubParameters != null && pairParameter.SubParameters.Count > 0) pairSubparameters.AddRange(pairParameter.SubParameters);
                    else pairSubparameters.Add(pairParameter);
                }

                foreach (ZPair pair in this.Pairs)
                {
                    ZLayer currentPairLayer = pair.Layers[index];
                    if (currentPairLayer == null) return;

                    ZLayer pairLayer = null;

                    if (pairsDict.Keys.Contains(pair.ID))
                    {
                        pairLayer = (pairsDict[pair.ID])[i] as ZLayer;
                    }
                    else
                    {
                        if (pairsDict.Count > 0)
                            pairLayer = (pairsDict.First().Value)[i] as ZLayer;
                    }

                    if (pairLayer == null) return;
                        
                    pairLayer = pairLayer.Clone();
                    pairLayer.ID = currentPairLayer.ID;
                    currentNewLayerStructure.PairLayers.Add(pair.ID, pairLayer);

                    currentOldLayerStructure.PairLayers.Add(pair.ID, currentPairLayer.Clone());
                    currentOldLayerStructure.PairLayers[pair.ID].ID = currentPairLayer.ID;
                }

                int layersCount = Layers.Count;
                int mirrorIndex = layersCount - 1 - index;

                InternalReplaceLayer(currentNewLayerStructure, true);

                if (ZPlannerManager.StackupPanel != null && isAutoMirror && !IsMirroredByTypesAndValues())
                {
                    mirrorNewLayerStructure.Index = mirrorIndex;
                    mirrorOldLayerStructure.Index = mirrorIndex;

                    if (mirrorNewLayerStructure.Index >= 0 && mirrorNewLayerStructure.Index != currentNewLayerStructure.Index)
                    {
                        mirrorStackupLayer = Layers[mirrorIndex];
                        if (mirrorStackupLayer == null) return;

                        mirrorNewLayerStructure.StackupLayer = currentNewLayerStructure.StackupLayer.Clone();
                        mirrorNewLayerStructure.StackupLayer.ID = mirrorStackupLayer.ID;
                        mirrorNewLayerStructure.Id = mirrorStackupLayer.ID;

                        mirrorOldLayerStructure.StackupLayer = mirrorStackupLayer.Clone();
                        mirrorOldLayerStructure.StackupLayer.ID = mirrorStackupLayer.ID;
                        mirrorOldLayerStructure.Id = mirrorStackupLayer.ID;

                        if (isLayerTypeChanged)
                            mirrorNewLayerStructure.StackupLayer.SetDefaultLayerParameterValue(ZStringConstants.ParameterIDLayerName);
                        else
                            mirrorNewLayerStructure.StackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, mirrorStackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName));

                        mirrorNewLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                        mirrorOldLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                        foreach (ZSingle single in this.Singles)
                        {
                            if (!currentNewLayerStructure.SingleLayers.ContainsKey(single.ID)) return;

                            ZLayer mirrorSingleLayer = single.Layers[mirrorIndex];
                            if (mirrorSingleLayer == null) return;

                            mirrorNewLayerStructure.SingleLayers.Add(single.ID, currentNewLayerStructure.SingleLayers[single.ID].Clone());
                            mirrorNewLayerStructure.SingleLayers[single.ID].ID = mirrorSingleLayer.ID;

                            mirrorOldLayerStructure.SingleLayers.Add(single.ID, mirrorSingleLayer.Clone());
                            mirrorOldLayerStructure.SingleLayers[single.ID].ID = mirrorSingleLayer.ID;
                        }

                        mirrorNewLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                        mirrorOldLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                        foreach (ZPair pair in this.Pairs)
                        {
                            if (!currentNewLayerStructure.PairLayers.ContainsKey(pair.ID)) return;

                            ZLayer mirrorPairLayer = pair.Layers[mirrorIndex];
                            if (mirrorPairLayer == null) return;

                            mirrorNewLayerStructure.PairLayers.Add(pair.ID, currentNewLayerStructure.PairLayers[pair.ID].Clone());
                            mirrorNewLayerStructure.PairLayers[pair.ID].ID = mirrorPairLayer.ID;

                            mirrorOldLayerStructure.PairLayers.Add(pair.ID, mirrorPairLayer.Clone());
                            mirrorOldLayerStructure.PairLayers[pair.ID].ID = mirrorPairLayer.ID;
                        }

                        InternalReplaceLayer(mirrorNewLayerStructure, false);
                    }
                }

                // LockingCore
                if (isCoreLocked)
                {
                    ExpandLockedLayers(underList, overList);

                    layerStructureList.AddRange(underList.Values);
                    layerStructureList.AddRange(overList.Values);
                    layerStructureList = new List<LayersCommandStructure>(layerStructureList.OrderBy(x => x.Index));
                }

                for (int n = 0; n < Layers.Count; ++n)
                {
                    // It is required to store the correct row Index when isCoreLocked=true
                    if (currentStackupLayer != null && currentStackupLayer.ID == Layers[n].ID)
                    {
                        currentNewLayerStructure.Index = n;
                        currentOldLayerStructure.Index = n;
                        layerStructureDictionary.Add(currentOldLayerStructure, currentNewLayerStructure);
                    }

                    if (mirrorStackupLayer != null && mirrorStackupLayer.ID == Layers[n].ID)
                    {
                        mirrorNewLayerStructure.Index = n;
                        mirrorOldLayerStructure.Index = n;
                        layerStructureDictionary.Add(mirrorOldLayerStructure, mirrorNewLayerStructure);
                    }
                }

                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                //if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.UpdateReferences();
                //ZPlannerManager.UpdateActiveStackup();
                ZPlannerManager.ResumeCollectionChangedEvent();
                Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new ReplaceLayerCommand(this, layerStructureDictionary, layerStructureList);
            }
            finally
            {
                ZPlannerManager.IsAutoMirror = isAutoMirror;
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.ResumeCollectionChangedEvent();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }*/

        public void ReplaceLayers(string currentID, bool isCoreLocked = false)
        {
            if (!ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyStackupLayers)) return;
            List<ZEntity> stackupLayers = ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyStackupLayers] as List<ZEntity>;
            Dictionary<string, List<ZEntity>> singlesDict = new Dictionary<string, List<ZEntity>>();
            Dictionary<string, List<ZEntity>> pairsDict = new Dictionary<string, List<ZEntity>>();

            foreach (string key in ZPlannerManager.Clipboard.Keys)
            {
                if (key.StartsWith(ZStringConstants.ClipbordKeySingleLayers)) singlesDict.Add(key.Substring(ZStringConstants.ClipbordKeySingleLayers.Length), ZPlannerManager.Clipboard[key] as List<ZEntity>);
                else if (key.StartsWith(ZStringConstants.ClipbordKeyPairLayers)) pairsDict.Add(key.Substring(ZStringConstants.ClipbordKeyPairLayers.Length), ZPlannerManager.Clipboard[key] as List<ZEntity>);
            }

            if (stackupLayers == null || stackupLayers.Count < 1)
            {
                ZPlannerManager.Clipboard.Clear();
                return;
            }

            foreach (List<ZEntity> singleLayers in singlesDict.Values)
            {
                if (singleLayers == null || stackupLayers.Count != singleLayers.Count)
                {
                    ZPlannerManager.Clipboard.Clear();
                    return;
                }
            }

            foreach (List<ZEntity> pairLayers in pairsDict.Values)
            {
                if (pairLayers == null || stackupLayers.Count != pairLayers.Count)
                {
                    ZPlannerManager.Clipboard.Clear();
                    return;
                }
            }

            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            ZPlannerManager.SuspendCollectionChangedEvent();
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            bool isAutoMirror = ZPlannerManager.IsAutoMirror;
            ZPlannerManager.IsAutoMirror = false;

            try
            {
                // Clear Layer Selection
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

                bool isMirror = (ZPlannerManager.StackupPanel != null && isAutoMirror && IsMirroredByTypesAndValues());                
                bool isCore = false;

                List<LayersCommandStructure> newLayerStructureList = new List<LayersCommandStructure>();
                List<LayersCommandStructure> oldLayerStructureList = new List<LayersCommandStructure>();

                ZLayer currentLayer =  Layers.Find(x => x.ID == currentID);
                if (currentLayer == null) return;
                int currentIndex = currentLayer.Order;
                ZLayerType? layerType = currentLayer.GetLayerType();

                if (isCoreLocked)
                {
                    if (layerType == ZLayerType.Core) isCore = true;
                    else if (layerType == ZLayerType.Signal || layerType == ZLayerType.Plane || layerType == ZLayerType.SplitMixed)
                    {
                        if (!isCore && currentIndex > 0)
                        {
                            ZLayer layer = Layers[currentIndex - 1];
                            if (layer.GetLayerType() == ZLayerType.Core)
                            {
                                currentLayer = layer;
                                currentIndex -= 1;
                                isCore = true;
                            }
                        }

                        if (!isCore && currentIndex < Layers.Count - 1)
                        {
                            ZLayer layer = Layers[currentIndex + 1];
                            if (layer.GetLayerType() == ZLayerType.Core)
                            {
                                currentLayer = layer;
                                currentIndex += 1;
                                isCore = true;
                            }
                        }
                    }
                }

                ZLayer currentPrevLayer = null;
                ZLayer mirrorPrevLayer = null;
                ZLayerType? prevLayerType = null;
                ZLayer currentNextLayer = null;
                ZLayer mirrorNextLayer = null;
                ZLayerType? nextLayerType = null;

                if (isCoreLocked && isCore)
                {
                    if (currentIndex > 0)
                    {
                        ZLayer layer = Layers[currentIndex - 1];
                        if (layer.isMetal())
                        {
                            oldLayerStructureList.Add(GetLayersCommandStructureByID(layer.ID));
                            currentPrevLayer = layer;
                            prevLayerType = layer.GetLayerType();
                        }
                    }

                    if (currentIndex > 0)
                    {
                        ZLayer layer = Layers[currentIndex + 1];
                        if (layer.isMetal())
                        {
                            oldLayerStructureList.Add(GetLayersCommandStructureByID(layer.ID));
                            currentNextLayer = layer;
                            nextLayerType = layer.GetLayerType();
                        }
                    }
                }

                oldLayerStructureList.Add(GetLayersCommandStructureByID(currentLayer.ID));

                int mirrorIndex = Layers.Count - 1 - currentIndex;
                ZLayer mirrorLayer = Layers[mirrorIndex];
                if (isMirror && mirrorIndex != currentIndex)
                {
                    if (isCoreLocked && isCore)
                    {
                        if (mirrorIndex > 0)
                        {
                            ZLayer layer = Layers[mirrorIndex - 1];
                            if (layer.isMetal())
                            {
                                oldLayerStructureList.Add(GetLayersCommandStructureByID(layer.ID));
                                mirrorNextLayer = layer;
                            }
                        }

                        if (mirrorIndex > 0)
                        {
                            ZLayer layer = Layers[mirrorIndex + 1];
                            if (layer.isMetal())
                            {
                                oldLayerStructureList.Add(GetLayersCommandStructureByID(layer.ID));
                                mirrorPrevLayer = layer;
                            }
                        }
                    }

                    oldLayerStructureList.Add(GetLayersCommandStructureByID(mirrorLayer.ID));
                }

                oldLayerStructureList = new List<LayersCommandStructure>(oldLayerStructureList.OrderBy(x => x.Index));

                int minIndex = Layers.Count;
                foreach (LayersCommandStructure layerStructure in oldLayerStructureList)
                {
                    minIndex = Math.Min(minIndex, layerStructure.Index);
                    InternalRemoveLayer(layerStructure, false);
                }

                // Recalculate Indexes
                if (currentIndex > mirrorIndex && isMirror)
                {
                    mirrorIndex = minIndex;
                    currentIndex = Layers.Count - mirrorIndex;
                }
                else
                {
                    currentIndex = minIndex;
                    mirrorIndex = Layers.Count - currentIndex;
                }

                // Insert copied rows
                for (int i = 0; i < stackupLayers.Count; ++i)
                {
                    // Stackup Layers.
                    ZLayer currentStackupLayer = stackupLayers[i] as ZLayer;
                    if (currentStackupLayer == null) continue;

                    string stringLayerType = currentStackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerType);
                    ZLayerType? currentLayerType = (ZLayer.LayerTypeDictionary.Keys.Contains(stringLayerType)) ? ZLayer.LayerTypeDictionary[stringLayerType] : (ZLayerType?)null;
                    bool isLayerTypeChanged = (currentLayerType != layerType);

                    currentStackupLayer = currentStackupLayer.Clone();

                    if (isLayerTypeChanged)
                    {
                        currentStackupLayer.SetDefaultLayerParameterValue(ZStringConstants.ParameterIDLayerName, currentLayerType);
                    }
                    else
                    {
                        currentStackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, currentLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName));
                        //currentStackupLayer.ID = currentLayer.ID;
                    }

                    if (currentPrevLayer != null && currentLayerType == prevLayerType)
                    {
                        currentStackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, currentPrevLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName));
                        //currentStackupLayer.ID = currentPrevLayer.ID;
                        currentPrevLayer = null;
                    }
                    else if (currentNextLayer != null && currentLayerType == nextLayerType)
                    {
                        currentStackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, currentNextLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName));
                        //currentStackupLayer.ID = currentNextLayer.ID;
                        currentNextLayer = null;
                    }

                    // Insert Stackup Layer at the end of the code block, because Grid redraw after insert of Stackup Layer.

                    // Single Layers.
                    Dictionary<string, ZLayer> currentSingleLayersDict = new Dictionary<string, ZLayer>();
                    foreach (ZSingle single in this.Singles)
                    {
                        ZLayer currentSingleLayer = null;

                        if (singlesDict.Keys.Contains(single.ID))
                        {
                            currentSingleLayer = (singlesDict[single.ID])[i] as ZLayer;
                        }
                        else
                        {
                            if (singlesDict.Count > 0)
                                currentSingleLayer = (singlesDict.First().Value)[i] as ZLayer;
                        }

                        if (currentSingleLayer == null) continue;

                        currentSingleLayer = currentSingleLayer.Clone();
                        currentSingleLayer.ID = currentStackupLayer.ID;
                        currentSingleLayersDict.Add(single.ID, currentSingleLayer);
                    }

                    // Pair Layers.
                    Dictionary<string, ZLayer> currentPairLayersDict = new Dictionary<string, ZLayer>();
                    foreach (ZPair pair in this.Pairs)
                    {
                        ZLayer currentPairLayer = null;
                        
                        if (pairsDict.Keys.Contains(pair.ID))
                        {
                            currentPairLayer = (pairsDict[pair.ID])[i] as ZLayer;
                        }
                        else
                        {
                            if (pairsDict.Count > 0)
                                currentPairLayer = (pairsDict.First().Value)[i] as ZLayer;
                        }

                        if (currentPairLayer == null) continue;

                        currentPairLayer = currentPairLayer.Clone();
                        currentPairLayer.ID = currentStackupLayer.ID;
                        currentPairLayersDict.Add(pair.ID, currentPairLayer);
                    }

                    newLayerStructureList.Add(GetLayersCommandStructureByLayerParts(currentIndex, currentStackupLayer, currentSingleLayersDict, currentPairLayersDict));

                    if (mirrorIndex > currentIndex) ++mirrorIndex;
                    ++currentIndex;
                }

                if (isMirror && currentIndex != mirrorIndex)
                {
                    for (int i = stackupLayers.Count - 1; i >= 0; --i)
                    {
                        // Stackup Layers.
                        ZLayer mirrorStackupLayer = stackupLayers[i] as ZLayer;
                        if (mirrorStackupLayer == null) continue;

                        string stringLayerType = mirrorStackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerType);
                        ZLayerType? mirrorLayerType = (ZLayer.LayerTypeDictionary.Keys.Contains(stringLayerType)) ? ZLayer.LayerTypeDictionary[stringLayerType] : (ZLayerType?)null;
                        bool isLayerTypeChanged = (mirrorLayerType != layerType);

                        mirrorStackupLayer = mirrorStackupLayer.Clone();

                        if (isLayerTypeChanged)
                        {
                            mirrorStackupLayer.SetDefaultLayerParameterValue(ZStringConstants.ParameterIDLayerName, mirrorLayerType);
                        }
                        else
                        {
                            mirrorStackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, mirrorLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName));
                            //mirrorStackupLayer.ID = mirrorLayer.ID;
                        }

                        if (mirrorPrevLayer != null && mirrorLayerType == prevLayerType)
                        {
                            mirrorStackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, mirrorPrevLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName));
                            //mirrorStackupLayer.ID = mirrorPrevLayer.ID;
                            mirrorPrevLayer = null;
                        }
                        else if (mirrorNextLayer != null && mirrorLayerType == nextLayerType)
                        {
                            mirrorStackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, mirrorNextLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName));
                            //mirrorStackupLayer.ID = mirrorNextLayer.ID;
                            mirrorNextLayer = null;
                        }

                        // Insert Stackup Layer at the end of the code block, because Grid redraw after insert of Stackup Layer.

                        // Single Layers.
                        Dictionary<string, ZLayer> mirrorSingleLayersDict = new Dictionary<string, ZLayer>();
                        foreach (ZSingle single in this.Singles)
                        {
                            ZLayer mirrorSingleLayer = null;

                            if (singlesDict.Keys.Contains(single.ID))
                            {
                                mirrorSingleLayer = (singlesDict[single.ID])[i] as ZLayer;
                            }
                            else
                            {
                                if (singlesDict.Count > 0)
                                    mirrorSingleLayer = (singlesDict.First().Value)[i] as ZLayer;
                            }

                            if (mirrorSingleLayer == null) continue;

                            mirrorSingleLayer = mirrorSingleLayer.Clone();
                            mirrorSingleLayer.ID = mirrorStackupLayer.ID;
                            mirrorSingleLayersDict.Add(single.ID, mirrorSingleLayer);
                        }

                        // Pair Layers.
                        Dictionary<string, ZLayer> mirrorPairLayersDict = new Dictionary<string, ZLayer>();
                        foreach (ZPair pair in this.Pairs)
                        {
                            ZLayer mirrorPairLayer = null;
                        
                            if (pairsDict.Keys.Contains(pair.ID))
                            {
                                mirrorPairLayer = (pairsDict[pair.ID])[i] as ZLayer;
                            }
                            else
                            {
                                if (pairsDict.Count > 0)
                                    mirrorPairLayer = (pairsDict.First().Value)[i] as ZLayer;
                            }

                            if (mirrorPairLayer == null) continue;

                            mirrorPairLayer = mirrorPairLayer.Clone();
                            mirrorPairLayer.ID = mirrorStackupLayer.ID;
                            mirrorPairLayersDict.Add(pair.ID, mirrorPairLayer);
                        }

                        newLayerStructureList.Add(GetLayersCommandStructureByLayerParts(mirrorIndex, mirrorStackupLayer, mirrorSingleLayersDict, mirrorPairLayersDict));

                        ++mirrorIndex;
                    }
                }

                int n = 0;
                foreach (LayersCommandStructure layerStructure in newLayerStructureList)
                {
                    InternalInsertLayer(layerStructure, n==0);
                    ++n;
                }

                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                //if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.UpdateReferences();
                //ZPlannerManager.UpdateActiveStackup();
                ZPlannerManager.ResumeCollectionChangedEvent();
                Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new ReplaceLayerCommand(this, oldLayerStructureList, newLayerStructureList);
            }
            finally
            {
                ZPlannerManager.IsAutoMirror = isAutoMirror;
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.ResumeCollectionChangedEvent();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public void ChangeSignalLayerReference(ZLayerParameter layerParameter, string layerReference)
        {
            if (ZPlannerManager.ConfirmOKCancel("Change layer type", "This will require changing the type of layer from Signal to Split/Mixed."))
            {
                ZLayerParameter typeLayerParameter = layerParameter.Layer.GetLayerParameter(ZStringConstants.ParameterIDLayerType);
                if (typeLayerParameter != null)
                {
                    ChangeLayerType(typeLayerParameter, ZStringConstants.LayerTypeSplitMixed, layerReference);
                }
            }
            else
            {
                layerParameter.NotifyPropertyChanged("Value");
            }
        }

        public void ChangePlaneLayerReference(ZLayerParameter layerParameter, string layerReference)
        {
            if (ZPlannerManager.ConfirmOKCancel("Change layer type", "This will require changing the type of layer from Plane to Split/Mixed."))
            {
                ZLayerParameter typeLayerParameter = layerParameter.Layer.GetLayerParameter(ZStringConstants.ParameterIDLayerType);
                if (typeLayerParameter != null)
                {
                    ChangeLayerType(typeLayerParameter, ZStringConstants.LayerTypeSplitMixed, layerReference);
                }
            }
            else
            {
                layerParameter.NotifyPropertyChanged("Value");
            }
        }

        public void ChangeLayerType(ZLayerParameter layerParameter,  string layerType, string reference = null)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                List<LayerParameterCommmandStructure> layerParameterStructureList = new List<LayerParameterCommmandStructure>();
                LayerParameterCommmandStructure currentLayerParameterStructure = new LayerParameterCommmandStructure();
                LayerParameterCommmandStructure mirrorLayerParameterStructure = new LayerParameterCommmandStructure();

                ZLayerType lType = ZLayer.LayerTypeDictionary[layerType];
                ZLayer currentLayer = layerParameter.Layer;
                string currentLayerId = currentLayer.ID;

                currentLayerParameterStructure.LayerParameter = layerParameter;
                currentLayerParameterStructure.OldValue = layerParameter.Value;
                currentLayerParameterStructure.NewValue = layerType;
                currentLayerParameterStructure.RemovedSpans = new Dictionary<ZSpan, int>();

                if ((currentLayerParameterStructure.OldValue == "Plane" || currentLayerParameterStructure.OldValue == "Signal" || currentLayerParameterStructure.OldValue == "Split/Mixed") && 
                    (currentLayerParameterStructure.NewValue == "Solder Mask" || currentLayerParameterStructure.NewValue == "Prepreg" || currentLayerParameterStructure.NewValue == "Core"))
                {
                    // ViaSpans.
                    foreach(ZSpan span in Spans)
                    {
                        if (span.FirstLayer.ID == currentLayerId || span.LastLayer.ID == currentLayerId) currentLayerParameterStructure.RemovedSpans.Add(span, Spans.IndexOf(span));
                    }
                }

                InternalChangeLayerType(currentLayerParameterStructure, true);
                layerParameterStructureList.Add(currentLayerParameterStructure);
                                
                // Pair Layer.
                foreach (ZPair pair in Pairs)
                {
                    ZLayer currentPairLayer = pair.Layers.Find(x => x.ID == currentLayerId);
                    if (currentPairLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                    foreach (ZLayerParameter lParameter in currentPairLayer.LayerParameters)
                    {
                        currentLayerParameterStructure = new LayerParameterCommmandStructure();

                        currentLayerParameterStructure.LayerParameter = lParameter;
                        currentLayerParameterStructure.OldValue = lParameter.Value;
                        if (lParameter.ID == ZStringConstants.ParameterIDZdiff_IsUsed)
                            currentLayerParameterStructure.NewValue = "false";
                        else
                            currentLayerParameterStructure.NewValue = lParameter.GetDefaulLayerParameterValue(lType);
                        currentLayerParameterStructure.RemovedSpans = new Dictionary<ZSpan, int>();

                        InternalChangeLayerType(currentLayerParameterStructure, true);
                        layerParameterStructureList.Add(currentLayerParameterStructure);
                    }
                }

                // Single Layer.
                foreach (ZSingle single in Singles)
                {
                    ZLayer currentSingleLayer = single.Layers.Find(x => x.ID == currentLayerId);
                    if (currentSingleLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                    foreach (ZLayerParameter lParameter in currentSingleLayer.LayerParameters)
                    {
                        currentLayerParameterStructure = new LayerParameterCommmandStructure();

                        currentLayerParameterStructure.LayerParameter = lParameter;
                        currentLayerParameterStructure.OldValue = lParameter.Value;
                        if (lParameter.ID == ZStringConstants.ParameterIDZo_IsUsed)
                            currentLayerParameterStructure.NewValue = "false";
                        else
                            currentLayerParameterStructure.NewValue = lParameter.GetDefaulLayerParameterValue(lType);
                        currentLayerParameterStructure.RemovedSpans = new Dictionary<ZSpan, int>();

                        InternalChangeLayerType(currentLayerParameterStructure, true);
                        layerParameterStructureList.Add(currentLayerParameterStructure);
                    }
                }

                string[] ignoreIDs = new string[] { ZStringConstants.ParameterIDLayerNumber, ZStringConstants.ParameterIDLayerType };
                List<ZLayerParameter> currentLayerParameters = new List<ZLayerParameter>(currentLayer.LayerParameters);
                foreach (ZLayerParameter lParameter in currentLayerParameters)
                {
                    if (ignoreIDs.Contains(lParameter.ID)) continue;
                    if (lParameter.ID == ZStringConstants.ParameterIDLayerName && !string.IsNullOrWhiteSpace(lParameter.Value)) continue;

                    currentLayerParameterStructure = new LayerParameterCommmandStructure();

                    currentLayerParameterStructure.LayerParameter = lParameter;
                    currentLayerParameterStructure.OldValue = lParameter.Value;
                    if (reference != null && lParameter.ID == ZStringConstants.ParameterIDPlaneReference)
                        currentLayerParameterStructure.NewValue = reference;
                    else
                        currentLayerParameterStructure.NewValue = lParameter.GetDefaulLayerParameterValue(lType);
                    currentLayerParameterStructure.RemovedSpans = new Dictionary<ZSpan, int>();

                    InternalChangeLayerType(currentLayerParameterStructure, true);
                    layerParameterStructureList.Add(currentLayerParameterStructure);
                }

                if (ZPlannerManager.StackupPanel != null && ZPlannerManager.IsAutoMirror)
                {
                    ZLayer mirrorLayer = Layers[Layers.Count - 1 - layerParameter.Layer.Order];
                    string mirrorLayerId = mirrorLayer.ID;
                    mirrorLayerParameterStructure.LayerParameter = mirrorLayer.LayerParameters.Find(x => x.ID == layerParameter.ID);
                    if (mirrorLayerParameterStructure.LayerParameter != null && mirrorLayerParameterStructure.LayerParameter.Value != layerType)
                    {
                        mirrorLayerParameterStructure.OldValue = mirrorLayerParameterStructure.LayerParameter.Value;
                        mirrorLayerParameterStructure.NewValue = layerType;
                        mirrorLayerParameterStructure.RemovedSpans = new Dictionary<ZSpan, int>();

                        if ((mirrorLayerParameterStructure.OldValue == "Plane" || mirrorLayerParameterStructure.OldValue == "Signal" || mirrorLayerParameterStructure.OldValue == "Split/Mixed") &&
                            (mirrorLayerParameterStructure.NewValue == "Solder Mask" || mirrorLayerParameterStructure.NewValue == "Prepreg" || mirrorLayerParameterStructure.NewValue == "Core"))
                        {
                            // ViaSpans.
                            foreach (ZSpan span in Spans)
                            {
                                if (span.FirstLayer.ID == mirrorLayerId || span.LastLayer.ID == mirrorLayerId) mirrorLayerParameterStructure.RemovedSpans.Add(span, Spans.IndexOf(span));
                            }
                        }

                        InternalChangeLayerType(mirrorLayerParameterStructure, true);
                        layerParameterStructureList.Add(mirrorLayerParameterStructure);

                        // Pair Layer.
                        foreach (ZPair pair in Pairs)
                        {
                            ZLayer mirrorPairLayer = pair.Layers.Find(x => x.ID == mirrorLayerId);
                            if (mirrorPairLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                            foreach (ZLayerParameter lParameter in mirrorPairLayer.LayerParameters)
                            {
                                if (ignoreIDs.Contains(lParameter.ID)) continue;

                                mirrorLayerParameterStructure = new LayerParameterCommmandStructure();

                                mirrorLayerParameterStructure.LayerParameter = lParameter;
                                mirrorLayerParameterStructure.OldValue = lParameter.Value;
                                if (lParameter.ID == ZStringConstants.ParameterIDZdiff_IsUsed)
                                    mirrorLayerParameterStructure.NewValue = "false";
                                else
                                    mirrorLayerParameterStructure.NewValue = lParameter.GetDefaulLayerParameterValue(lType);
                                mirrorLayerParameterStructure.RemovedSpans = new Dictionary<ZSpan, int>();

                                InternalChangeLayerType(mirrorLayerParameterStructure, true);
                                layerParameterStructureList.Add(mirrorLayerParameterStructure);
                            }
                        }

                        // Single Layer.
                        foreach (ZSingle single in Singles)
                        {
                            ZLayer mirrorSingleLayer = single.Layers.Find(x => x.ID == mirrorLayerId);
                            if (mirrorSingleLayer == null) throw new IndexOutOfRangeException("The row with specified id was not found.");

                            foreach (ZLayerParameter lParameter in mirrorSingleLayer.LayerParameters)
                            {
                                if (ignoreIDs.Contains(lParameter.ID)) continue;

                                mirrorLayerParameterStructure = new LayerParameterCommmandStructure();

                                mirrorLayerParameterStructure.LayerParameter = lParameter;
                                mirrorLayerParameterStructure.OldValue = lParameter.Value;
                                if (lParameter.ID == ZStringConstants.ParameterIDZo_IsUsed)
                                    mirrorLayerParameterStructure.NewValue = "false";
                                else
                                    mirrorLayerParameterStructure.NewValue = lParameter.GetDefaulLayerParameterValue(lType);
                                mirrorLayerParameterStructure.RemovedSpans = new Dictionary<ZSpan, int>();

                                InternalChangeLayerType(mirrorLayerParameterStructure, true);
                                layerParameterStructureList.Add(mirrorLayerParameterStructure);
                            }
                        }
                        
                        List<ZLayerParameter> mirrorLayerParameters = new List<ZLayerParameter>(mirrorLayer.LayerParameters);
                        foreach (ZLayerParameter lParameter in mirrorLayerParameters)
                        {
                            if (ignoreIDs.Contains(lParameter.ID)) continue;

                            mirrorLayerParameterStructure = new LayerParameterCommmandStructure();

                            mirrorLayerParameterStructure.LayerParameter = lParameter;
                            mirrorLayerParameterStructure.OldValue = lParameter.Value;
                            if (reference != null && lParameter.ID == ZStringConstants.ParameterIDPlaneReference)
                                mirrorLayerParameterStructure.NewValue = reference;
                            else
                                mirrorLayerParameterStructure.NewValue = lParameter.GetDefaulLayerParameterValue(lType);
                            mirrorLayerParameterStructure.RemovedSpans = new Dictionary<ZSpan, int>();

                            InternalChangeLayerType(mirrorLayerParameterStructure, true);
                            layerParameterStructureList.Add(mirrorLayerParameterStructure);
                        }
                    }
                }

                CalculateLayerNumber();
                ZPlannerManager.MainMenu.UpdateMenu();
                ZPlannerManager.UpdateActiveStackup();
                
                ZPlannerManager.CalculatePressedThickness(ZPlannerManager.IsPressedThickness);
                ZPlannerManager.StackupPanel.RecalculateViaSpanAspectRatio();


                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeLayerParameterValuesCommand(this, layerParameterStructureList);

            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        internal void InternalChangeLayerType(LayerParameterCommmandStructure layerParameterStructure, bool useNewValue)
        {
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            try
            {
                if (!useNewValue)
                {
                    // ViaSpans.
                    foreach (ZSpan span in layerParameterStructure.RemovedSpans.Keys)
                    {
                        Spans.Insert(layerParameterStructure.RemovedSpans[span], span);
                    }

                    layerParameterStructure.LayerParameter.InternalValue = layerParameterStructure.OldValue;
                }

                if (useNewValue)
                {
                    layerParameterStructure.LayerParameter.InternalValue = layerParameterStructure.NewValue;

                    // ViaSpans.
                    foreach (ZSpan span in layerParameterStructure.RemovedSpans.Keys)
                    {
                        Spans.Remove(span);
                    }
                }

                if (layerParameterStructure.OldValue != layerParameterStructure.NewValue) layerParameterStructure.LayerParameter.NotifyPropertyChanged("Value");
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
            }
        }

        internal bool FinedMaterialForAnyLayer(ZLibraryCategory[] libraryCetegories, string fabricator)
        {
            bool isMaterialFound = true;
            Dictionary<MaterialSearchAttributes, List<ZLayer>> layersByAttributesList = new Dictionary<MaterialSearchAttributes, List<ZLayer>>();

            foreach (ZLayer layer in Layers)
            {
                ZLayerType? layerType;

                if (layer.isCoreOrPrepreg(out layerType))
                {
                    string thickness = layer.GetLayerParameterValue(ZStringConstants.ParameterIDThickness);
                    string dk = layer.GetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant);
                    string construction = layer.GetLayerParameterValue(ZStringConstants.ParameterIDConstruction);
                    string resin = layer.GetLayerParameterValue(ZStringConstants.ParameterIDResinContent);
                    string materialName = layer.GetLayerParameterValue(ZStringConstants.ParameterIDMaterial);

                    MaterialSearchAttributes attributes = new MaterialSearchAttributes();
                    attributes.LayerType = layerType;
                    attributes.Dk = dk ?? string.Empty;
                    attributes.Construction = construction ?? string.Empty;
                    attributes.Resin = resin ?? string.Empty;
                    attributes.MaterialName = materialName ?? string.Empty;
                    attributes.Thickness = thickness ?? string.Empty;

                    if (!layersByAttributesList.ContainsKey(attributes)) layersByAttributesList.Add(attributes, new List<ZLayer>());

                    layersByAttributesList[attributes].Add(layer);
                }
            }
                
            string fabricatorSuffix = "(" + fabricator + ")";

            foreach (MaterialSearchAttributes attributes in layersByAttributesList.Keys)
            {
                ZMaterial material = ZPlannerManager.Dml.FinedMaterial(libraryCetegories, attributes);

                if (material != null)
                {
                    AssignMaterialToStackup(material, layersByAttributesList[attributes]);
                }
                else
                {
                    if (attributes.LayerType == ZLayerType.Prepreg) isMaterialFound = false;

                    ZMaterial newMaterial = null;
                    string dk = null;
                    string df = null;
                    foreach (ZLayer layer in layersByAttributesList[attributes])
                    {
                        if (string.IsNullOrWhiteSpace(dk) || string.IsNullOrWhiteSpace(df))
                        {
                            DMLMatch.MatchDkDf(layer);
                            dk = layer.GetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant);
                            df = layer.GetLayerParameterValue(ZStringConstants.ParameterIDLossTangent);
                        }
                        else
                        {
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant, dk, true);
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDLossTangent, df, true);
                        }

                        foreach (var single in layer.Stackup.Singles)
                        {
                            layer.Stackup.GetLayerOfSingleImpedance(layer.ID, single.ID).SetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant, dk);
                            layer.Stackup.GetLayerOfSingleImpedance(layer.ID, single.ID).SetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent, df);
                        }

                        foreach (var pair in layer.Stackup.Pairs)
                        {
                            layer.Stackup.GetLayerOfPairImpedance(layer.ID, pair.ID).SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_DielectricConstant, dk);
                            layer.Stackup.GetLayerOfPairImpedance(layer.ID, pair.ID).SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_LossTangent, df);
                        }

                        string sValue = layer.GetLayerParameterValue(ZStringConstants.ParameterIDComments) ?? string.Empty;
                        if (sValue != string.Empty) sValue += " ";
                        layer.SetLayerParameterValue(ZStringConstants.ParameterIDComments, sValue + "Local library.");

                        string sMaterial = layer.GetLayerParameterValue(ZStringConstants.ParameterIDMaterial) ?? string.Empty;
                        if (sMaterial.EndsWith(fabricatorSuffix)) sMaterial = sMaterial.Replace(fabricatorSuffix, string.Empty).Trim();
                        if (sMaterial != string.Empty) sMaterial += " ";
                        layer.SetLayerParameterValue(ZStringConstants.ParameterIDMaterial, sMaterial + fabricatorSuffix);

                        if (newMaterial == null)
                        {
                            newMaterial = layer.GetMaterialFromLayer();
                            newMaterial.SetMaterialParameterValue(ZStringConstants.DMLParameterIDCategory, ZStringConstants.MaterialCategoryLocal);

                            MaterialSearchAttributes newAttributes = attributes.Clone();
                            newAttributes.MaterialName = sMaterial;

                            material = ZPlannerManager.Dml.FinedMaterial(new ZLibraryCategory[] { ZLibraryCategory.Local }, newAttributes);
                            if (material == null) ZPlannerManager.Dml.AddMaterial(newMaterial);
                        }
                    }

                    /*if (layersByAttributesList[attributes].Count > 0)
                    {
                        ZMaterial newMaterial = layersByAttributesList[attributes][0].GetMaterialFromLayer();
                        if (newMaterial != null)
                        {
                            newMaterial.SetMaterialParameterValue(ZStringConstants.DMLParameterIDCategory, ZStringConstants.MaterialCategoryLocal);

                            string sMaterial = newMaterial.GetMaterialParameterValue(ZStringConstants.DMLParameterIDMaterial) ?? string.Empty;
                            if (sMaterial.EndsWith(fabricatorSuffix)) sMaterial = sMaterial.Replace(fabricatorSuffix, string.Empty).Trim();
                            if (sMaterial != string.Empty) sMaterial += " ";
                            sMaterial = sMaterial + fabricatorSuffix;
                            newMaterial.SetMaterialParameterValue(ZStringConstants.DMLParameterIDMaterial, sMaterial);

                            MaterialSearchAttributes newAttributes = attributes.Clone();
                            newAttributes.MaterialName = sMaterial;

                            material = ZPlannerManager.Dml.FinedMaterial(new ZLibraryCategory[] { ZLibraryCategory.Local }, newAttributes);

                            if (material == null) ZPlannerManager.Dml.AddMaterial(newMaterial);
                        }
                    }*/
                }
            }

            return isMaterialFound;
        }

        internal void CopyMaterialFromLayer(string layerId)
        {
            ZLayer layer = GetLayerOfStackup(layerId);
            if (layer == null) return;

            ZLayerType? layerType;
            if (!layer.isDielectric(out layerType)) return;

            bool isIgnoreCommands = ZPlannerManager.Commands.SuspendCommandEvent();

            try
            {
                ZMaterial material = layer.GetMaterialFromLayer();
                if (material == null) return;

                ZPlannerManager.Clipboard.Remove(ZStringConstants.ClipbordKeyMaterials);
                ZPlannerManager.Clipboard.Remove(ZStringConstants.ClipbordKeyMaterialsForStackup);
                List<ZEntity> materials = new List<ZEntity>();
                materials.Add(material.Clone());
                ZPlannerManager.Clipboard.Add(ZStringConstants.ClipbordKeyMaterialsForStackup, materials);
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnoreCommands);
            }
        }

        internal void AssignMaterialToStackup(ZMaterial material, List<ZLayer> layers)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            List<string> layerParametersToBeIgnored = new List<string>(new string[] { ZStringConstants.ParameterIDComments });
            List<LayerMaterialCommandStructure> layerMaterialStructureList = new List<LayerMaterialCommandStructure>();

            try
            {
                Dictionary<ZLayerParameter, string> newValueByLayerParameter = new Dictionary<ZLayerParameter, string>();

                foreach (ZLayer layer in layers)
                {
                    //newValueByLayerParameter.Add(layer.GetLayerParameter(ZStringConstants.ParameterIDMaterial), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDMaterial));
                    newValueByLayerParameter.Add(layer.GetLayerParameter(ZStringConstants.ParameterIDConstruction), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDNormalizedConstruction));
                    newValueByLayerParameter.Add(layer.GetLayerParameter(ZStringConstants.ParameterIDThickness), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDH));
                    newValueByLayerParameter.Add(layer.GetLayerParameter(ZStringConstants.ParameterIDOriginThickness), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDH));
                    
                    newValueByLayerParameter.Add(layer.GetLayerParameter(ZStringConstants.ParameterIDResinContent), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDResin));

                    newValueByLayerParameter.Add(layer.GetLayerParameter(ZStringConstants.ParameterIDDielectricConstant), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDDk));
                    newValueByLayerParameter.Add(layer.GetLayerParameter(ZStringConstants.ParameterIDLossTangent), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDDf));

                    foreach (var single in layer.Stackup.Singles)
                    {
                        newValueByLayerParameter.Add(layer.Stackup.GetLayerOfSingleImpedance(layer.ID, single.ID).GetLayerParameter(ZStringConstants.ParameterIDZo_DielectricConstant), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDDk));
                        newValueByLayerParameter.Add(layer.Stackup.GetLayerOfSingleImpedance(layer.ID, single.ID).GetLayerParameter(ZStringConstants.ParameterIDZo_LossTangent), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDDf));
                    }

                    foreach (var pair in layer.Stackup.Pairs)
                    {
                        newValueByLayerParameter.Add(layer.Stackup.GetLayerOfPairImpedance(layer.ID, pair.ID).GetLayerParameter(ZStringConstants.ParameterIDZdiff_DielectricConstant), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDDk));
                        newValueByLayerParameter.Add(layer.Stackup.GetLayerOfPairImpedance(layer.ID, pair.ID).GetLayerParameter(ZStringConstants.ParameterIDZdiff_LossTangent), material.GetMaterialParameterValue(ZStringConstants.DMLParameterIDDf));
                    }

                    foreach (ZMaterialParameter mParameter in material.MaterialParameters)
                    {
                        if (layerParametersToBeIgnored.Contains(mParameter.ID)) continue;
                        ZLayerParameter lParameter = layer.GetLayerParameter(mParameter.ID);
                        if (lParameter != null && !newValueByLayerParameter.Keys.Contains(lParameter)) newValueByLayerParameter.Add(lParameter, mParameter.Value);
                    }
                }

                foreach (ZLayerParameter layerParameter in newValueByLayerParameter.Keys)
                {
                    if (layerParameter == null) continue;

                    LayerMaterialCommandStructure currentLayerMaterialStructure = new LayerMaterialCommandStructure();

                    currentLayerMaterialStructure.MaterialParameterValue = (newValueByLayerParameter[layerParameter] ?? string.Empty).Trim('%').Trim();
                    currentLayerMaterialStructure.LayerParameter = layerParameter;
                    currentLayerMaterialStructure.LayerParameterValue = layerParameter.Value;
                    currentLayerMaterialStructure.IsMaterialAssigned = layerParameter.Layer.IsMaterialAssigned;

                    layerMaterialStructureList.Add(currentLayerMaterialStructure);
                }

                InternalAssignMaterialToStackup(layerMaterialStructureList, true, false);
            }
            finally
            {
                //ZPlannerManager.CalculatePressedThickness(ZPlannerManager.IsPressedThickness);  // Excessive call
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.UpdateActiveStackup();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new AssignMaterialToLayerCommand(this, layerMaterialStructureList);
            }
        }

        internal void UnAssignMaterialToStackup(List<ZLayer> layers)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            List<LayerMaterialCommandStructure> layerMaterialStructureList = new List<LayerMaterialCommandStructure>();

            try
            {
                Dictionary<ZLayerParameter, string> newValueByLayerParameter = new Dictionary<ZLayerParameter, string>();

                foreach (ZLayer layer in layers)
                {
                    ZLayerParameter layerParameter = layer.GetLayerParameter(ZStringConstants.ParameterIDMaterial);

                    if (layerParameter == null) continue;

                    LayerMaterialCommandStructure currentLayerMaterialStructure = new LayerMaterialCommandStructure();

                    currentLayerMaterialStructure.MaterialParameterValue = layerParameter.Value + " (Editable)";
                    currentLayerMaterialStructure.LayerParameter = layerParameter;
                    currentLayerMaterialStructure.LayerParameterValue = layerParameter.Value;
                    currentLayerMaterialStructure.IsMaterialAssigned = false;

                    layerMaterialStructureList.Add(currentLayerMaterialStructure);

                }

                InternalAssignMaterialToStackup(layerMaterialStructureList, true, true);
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.UpdateActiveStackup();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new UnAssignMaterialToLayerCommand(this, layerMaterialStructureList);
            }
        }

        internal void InternalAssignMaterialToStackup(List<LayerMaterialCommandStructure> layerMaterialStructureList, bool useMaterialValue, bool useIsMaterialAssigned)
        {
            foreach (LayerMaterialCommandStructure layerMaterialStructure in layerMaterialStructureList)
            {
                layerMaterialStructure.LayerParameter.Value = (useMaterialValue) ? layerMaterialStructure.MaterialParameterValue : layerMaterialStructure.LayerParameterValue;
                bool isMaterialAssigned = (useIsMaterialAssigned) ? layerMaterialStructure.IsMaterialAssigned : true;
                ZLayer stackupLayer = GetLayerOfStackup(layerMaterialStructure.LayerParameter.Layer.ID);
                if (stackupLayer != null) stackupLayer.IsMaterialAssigned = isMaterialAssigned;

                foreach (ZSingle single in Singles)
                {
                    ZLayer singleLayer = GetLayerOfSingleImpedance(layerMaterialStructure.LayerParameter.Layer.ID, single.ID);
                    if (singleLayer != null) singleLayer.IsMaterialAssigned = isMaterialAssigned;
                }

                foreach(ZPair pair in Pairs)
                {
                    ZLayer pairLayer = GetLayerOfPairImpedance(layerMaterialStructure.LayerParameter.Layer.ID, pair.ID);
                    if (pairLayer != null) pairLayer.IsMaterialAssigned = isMaterialAssigned;
                }
            }

            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.StackupPanel.SetReadOnlyCells();
                ZPlannerManager.StackupPanel.FormatGridView();
            }
        }

        public string AddSingle()
        {
            return AddSingle(this.Singles.Count);
        }

        public string AddSingle(int index)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                ZSingle single = (ActiveSingle == null) ? new ZSingle(this) : new ZSingle(this, ActiveSingle);
                this.Singles.Insert(index, single);
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new AddSingleCommand(this, index, single);
                return single.ID;
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public void RemoveSingle(int index)
        {
                bool isIndexIncorrect = (index < 0 || index > this.Singles.Count);
                if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

                RemoveSingle(Singles[index].ID);
        }

        public void RemoveSingle(string id)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                ZSingle single = this.Singles.Find(x => x.ID == id);
                if (single == null) throw new IndexOutOfRangeException("The single with specified id was not find.");
                int index = Singles.IndexOf(single);

                this.Singles.Remove(single);

                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveSingleCommand(this, index, single);
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public string AddPair()
        {
            return AddPair(this.Pairs.Count);
        }

        public string AddPair(int index)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                ZPair pair = (ActivePair == null) ? new ZPair(this) : new ZPair(this, ActivePair);
                this.Pairs.Insert(index, pair);
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new AddPairCommand(this, index, pair);
                return pair.ID;
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public void RemovePair(int index)
        {
            bool isIndexIncorrect = (index < 0 || index > this.Pairs.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            RemovePair(Pairs[index].ID);
        }

        public void RemovePair(string id)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                ZPair pair = this.Pairs.Find(x => x.ID == id);
                if (pair == null) throw new IndexOutOfRangeException("The pair with specified id was not find.");
                int index = Pairs.IndexOf(pair);

                this.Pairs.Remove(pair);

                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemovePairCommand(this, index, pair);
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public void AddViaSpan()
        {
            AddViaSpan(this.Spans.Count);
        }

        public void AddViaSpan(int index)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                ZSpan span = new ZSpan(this);
                this.Spans.Insert(index, span);

                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new AddSpanCommand(this, index, span);
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public void RemoveViaSpan(int index)
        {
            bool isIndexIncorrect = (index < 0 || index > this.Spans.Count);
            if (isIndexIncorrect) throw new IndexOutOfRangeException("The specified index is out of range.");

            RemoveViaSpan(Spans[index].ID);
        }

        public void RemoveViaSpan(string id)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            try
            {
                ZSpan span = this.Spans.Find(x => x.ID == id);
                if (span == null) throw new IndexOutOfRangeException("The via span with specified id was not find.");

                int index = Spans.IndexOf(span);
                this.Spans.Remove(span);

                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveSpanCommand(this, index, span);
            }
            finally
            {
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
            }
        }

        public bool MirrorStackup(bool allowEnlargeStackup = false)
        {
            ZPlannerManager.StatusMenu.StartProgress("Mirroring Stackup ...", true);
            Cursor currentCursor = Cursor.Current;
            Cursor.Current = Cursors.WaitCursor;
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            ZPlannerManager.SuspendCollectionChangedEvent();
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
            
            try
            {
                if (!IsMirroredByTypes())
                {
                    List<LayersCommandStructure> addLayerStructureList = new List<LayersCommandStructure>();
                    List<LayersCommandStructure> removeLayerStructureList = new List<LayersCommandStructure>();

                    bool hasCenterLayer;
                    if (allowEnlargeStackup)
                    {
                        hasCenterLayer = true;

                        if (Layers.Count >= 2)
                        {
                            if (Layers[Layers.Count - 1].isMetal())
                            {
                                if (Layers[Layers.Count - 2].GetLayerType() == ZLayerType.Core)
                                {
                                    // add Prepreg row
                                    bool isPrepregFound = false;

                                    if (Layers.Count >= 3)
                                    {
                                        for (int i = Layers.Count - 3; i >= 0; --i)
                                        {
                                            if (Layers[i].GetLayerType() == ZLayerType.Prepreg)
                                            {
                                                LayersCommandStructure currentLayerStructure = new LayersCommandStructure();

                                                currentLayerStructure.Index = Layers.Count;

                                                // Stackup Layers.
                                                currentLayerStructure.StackupLayer = Layers[i].Clone();
                                                currentLayerStructure.Id = currentLayerStructure.StackupLayer.ID;
                                                if (currentLayerStructure.StackupLayer.isMetal())
                                                {
                                                    currentLayerStructure.StackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, string.Empty);
                                                }

                                                // Single Layers.
                                                currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                                                foreach (ZSingle single in this.Singles)
                                                {
                                                    ZLayer singleLayer = single.Layers[i].Clone();
                                                    singleLayer.ID = currentLayerStructure.Id;
                                                    currentLayerStructure.SingleLayers.Add(single.ID, singleLayer);
                                                }

                                                // Pair Layers.
                                                currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                                                foreach (ZPair pair in this.Pairs)
                                                {
                                                    ZLayer pairLayer = pair.Layers[i].Clone();
                                                    pairLayer.ID = currentLayerStructure.Id;
                                                    currentLayerStructure.PairLayers.Add(pair.ID, pairLayer);
                                                }

                                                InternalInsertLayer(currentLayerStructure, false);
                                                addLayerStructureList.Add(currentLayerStructure);
                                                isPrepregFound = true;
                                                break;
                                            }
                                        }
                                    }

                                    if (!isPrepregFound)
                                    {
                                        LayersCommandStructure currentLayerStructure = CreateLayersCommandStructureByType(Layers.Count, ZLayerType.Prepreg);
                                        InternalInsertLayer(currentLayerStructure, false);
                                        addLayerStructureList.Add(currentLayerStructure);
                                    }
                                }
                                else 
                                { 
                                    // add Core row
                                    bool isCoreFound = false;

                                    if (Layers.Count >= 3)
                                    {
                                        for (int i = Layers.Count - 3; i >= 0; --i)
                                        {
                                            if (Layers[i].GetLayerType() == ZLayerType.Core)
                                            {
                                                LayersCommandStructure currentLayerStructure = new LayersCommandStructure();

                                                currentLayerStructure.Index = Layers.Count;

                                                // Stackup Layers.
                                                currentLayerStructure.StackupLayer = Layers[i].Clone();
                                                currentLayerStructure.Id = currentLayerStructure.StackupLayer.ID;
                                                if (currentLayerStructure.StackupLayer.isMetal())
                                                {
                                                    currentLayerStructure.StackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, string.Empty);
                                                }

                                                // Single Layers.
                                                currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                                                foreach (ZSingle single in this.Singles)
                                                {
                                                    ZLayer singleLayer = single.Layers[i].Clone();
                                                    singleLayer.ID = currentLayerStructure.Id;
                                                    currentLayerStructure.SingleLayers.Add(single.ID, singleLayer);
                                                }

                                                // Pair Layers.
                                                currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                                                foreach (ZPair pair in this.Pairs)
                                                {
                                                    ZLayer pairLayer = pair.Layers[i].Clone();
                                                    pairLayer.ID = currentLayerStructure.Id;
                                                    currentLayerStructure.PairLayers.Add(pair.ID, pairLayer);
                                                }

                                                InternalInsertLayer(currentLayerStructure, false);
                                                addLayerStructureList.Add(currentLayerStructure);
                                                isCoreFound = true;
                                                break;
                                            }
                                        }
                                    }

                                    if (!isCoreFound)
                                    {
                                        LayersCommandStructure currentLayerStructure = CreateLayersCommandStructureByType(Layers.Count, ZLayerType.Core);
                                        InternalInsertLayer(currentLayerStructure, false);
                                        addLayerStructureList.Add(currentLayerStructure);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (!ZPlannerManager.ConfirmOKCancel("Auto-Mirror Stackup Confirmation", "Auto-Mirroring the stackup will force symmetry around the current stackup's center line. If the stackup is not currently symmetrical, the bottom half of the present stackup will be overwritten with data from the current top half. Select OK to proceed, or Cancel to go back to the previous view.")) return false;
                    
                        // Remove bottom part of stackup
                        hasCenterLayer = (Layers.Count % 2 != 0);
                        int originLayerCount = Layers.Count;
                        for (int i = originLayerCount - 1; i >= originLayerCount - (int)(originLayerCount / 2); --i)
                        {
                            ZLayer layer = this.Layers[i];
                            if (layer == null) continue;

                            LayersCommandStructure currentLayerStructure = new LayersCommandStructure();

                            currentLayerStructure = GetLayersCommandStructureByID(layer.ID);
                            if (currentLayerStructure.StackupLayer != null && !currentLayerStructure.SingleLayers.ContainsValue(null) && !currentLayerStructure.PairLayers.ContainsValue(null))
                            {
                                InternalRemoveLayer(currentLayerStructure, false);
                                removeLayerStructureList.Add(currentLayerStructure);
                            }
                        }
                    }

                    // Copy top part of stackup to the bottom
                    int index = Layers.Count;

                    for (int i = Layers.Count - ((hasCenterLayer)?2:1); i >= 0; --i)
                    {
                        LayersCommandStructure currentLayerStructure = new LayersCommandStructure();

                        currentLayerStructure.Index = index;

                        // Stackup Layers.
                        currentLayerStructure.StackupLayer = Layers[i].Clone();
                        currentLayerStructure.Id = currentLayerStructure.StackupLayer.ID;
                        if (currentLayerStructure.StackupLayer.isMetal())
                        {
                            currentLayerStructure.StackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, string.Empty);
                        }

                        // Single Layers.
                        currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                        foreach (ZSingle single in this.Singles)
                        {
                            ZLayer singleLayer = single.Layers[i].Clone();
                            singleLayer.ID = currentLayerStructure.Id;
                            currentLayerStructure.SingleLayers.Add(single.ID, singleLayer);
                        }

                        // Pair Layers.
                        currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                        foreach (ZPair pair in this.Pairs)
                        {
                            ZLayer pairLayer = pair.Layers[i].Clone();
                            pairLayer.ID = currentLayerStructure.Id;
                            currentLayerStructure.PairLayers.Add(pair.ID, pairLayer);
                        }

                        InternalInsertLayer(currentLayerStructure, false);
                        addLayerStructureList.Add(currentLayerStructure);
                        ++index;
                    }

                    ZPlannerManager.ResumeCollectionChangedEvent();
                    Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                    ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                    ZPlannerManager.UpdateActiveStackup();
                    if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new MirrorLayersCommand(this, removeLayerStructureList, addLayerStructureList);
                }
                else
                {
                    Dictionary<LayersCommandStructure, LayersCommandStructure> layerStructureDictionary = new Dictionary<LayersCommandStructure, LayersCommandStructure>();
                    
                    for (int i = 0; i < (int)(Layers.Count * 0.5); ++i)
                    {
                        int mirrorIndex = Layers.Count - 1 - i;
                        LayersCommandStructure currentLayerStructure = new LayersCommandStructure();
                        LayersCommandStructure mirroredLayerStructure = new LayersCommandStructure();

                        currentLayerStructure.Index = mirrorIndex;
                        mirroredLayerStructure.Index = mirrorIndex;

                        // Stackup Layers.
                        currentLayerStructure.StackupLayer = Layers[mirrorIndex].Clone();
                        mirroredLayerStructure.StackupLayer = Layers[i].Clone();
                        currentLayerStructure.Id = Layers[mirrorIndex].ID;
                        mirroredLayerStructure.Id = Layers[mirrorIndex].ID;
                        currentLayerStructure.StackupLayer.ID = currentLayerStructure.Id;
                        mirroredLayerStructure.StackupLayer.ID = currentLayerStructure.Id;
                        mirroredLayerStructure.StackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber, string.Empty);
                        mirroredLayerStructure.StackupLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, Layers[mirrorIndex].GetLayerParameterValue(ZStringConstants.ParameterIDLayerName));

                        // Single Layers.
                        currentLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                        mirroredLayerStructure.SingleLayers = new Dictionary<string, ZLayer>();
                        foreach (ZSingle single in this.Singles)
                        {
                            ZLayer singleLayer = single.Layers[mirrorIndex].Clone();
                            singleLayer.ID = currentLayerStructure.Id;
                            currentLayerStructure.SingleLayers.Add(single.ID, singleLayer);

                            singleLayer = single.Layers[i].Clone();
                            singleLayer.ID = currentLayerStructure.Id;
                            mirroredLayerStructure.SingleLayers.Add(single.ID, singleLayer);
                        }

                        // Pair Layers.
                        currentLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                        mirroredLayerStructure.PairLayers = new Dictionary<string, ZLayer>();
                        foreach (ZPair pair in this.Pairs)
                        {
                            ZLayer pairLayer = pair.Layers[mirrorIndex].Clone();
                            pairLayer.ID = currentLayerStructure.Id;
                            currentLayerStructure.PairLayers.Add(pair.ID, pairLayer);

                            pairLayer = pair.Layers[i].Clone();
                            pairLayer.ID = currentLayerStructure.Id;
                            mirroredLayerStructure.PairLayers.Add(pair.ID, pairLayer);
                        }

                        InternalReplaceLayer(mirroredLayerStructure, false);
                        layerStructureDictionary.Add(currentLayerStructure, mirroredLayerStructure);
                    }

                    ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                    ZPlannerManager.UpdateActiveStackup();
                    if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new MirrorLayersAndValuesCommand(this, layerStructureDictionary);
                }

                if (Layers.Count > 0) ZPlannerManager.ProjectPanel.SelectLayer(Layers[0].ID);
                ZPlannerManager.MessagePanel.AddMessage("The Stackup was mirrored.");
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.ResumeCollectionChangedEvent();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                ZPlannerManager.StackupPanel.SetReadOnlyCells();
                ZPlannerManager.StackupPanel.FormatGridView();
                Cursor.Current = currentCursor;
                ZPlannerManager.StatusMenu.StopProgress("Mirroring Stackup ...");
            }

            return true;
        }

        public bool IsMirrored()
        {
            return IsMirroredByTypesAndValues();
        }

        public bool IsMirroredByTypes()
        {
            for (int i = 0; i < (int)(Layers.Count * 0.5); ++i)
            {
                if (Layers[i].GetLayerType() != Layers[Layers.Count - 1 - i].GetLayerType()) return false;
            }

            return true;
        }

        public bool IsMirroredByTypesAndValues()
        {
            List<string> layerParametersToBeIgnored = new List<string>(new string[] { ZStringConstants.ParameterIDComments, ZStringConstants.ParameterIDLayerNumber, ZStringConstants.ParameterIDLayerName, ZStringConstants.ParameterIDCopperPercent, ZStringConstants.ParameterIDNarrowTop, ZStringConstants.ParameterIDCalcRoughTop, ZStringConstants.ParameterIDCalcRoughBottom, ZStringConstants.ParameterIDZo_InsertionLoss, ZStringConstants.ParameterIDZo_TotalLoss, ZStringConstants.ParameterIDZdiff_InsertionLoss, ZStringConstants.ParameterIDZdiff_TotalLoss, ZStringConstants.ParameterIDZo_TopReference, ZStringConstants.ParameterIDZo_BottomReference, ZStringConstants.ParameterIDZdiff_TopReference, ZStringConstants.ParameterIDZdiff_BottomReference });
            
            for (int i = 0; i < (int)(Layers.Count * 0.5); ++i)
            {
                foreach (ZLayerParameter layerParameter in Layers[i].LayerParameters)
                {
                    if (layerParametersToBeIgnored.Contains(layerParameter.ID) || layerParameter.IsReadOnly(false, true)) continue;
                    if (layerParameter.Value != Layers[Layers.Count - 1 - i].GetLayerParameterValue(layerParameter.ID)) return false;
                }
            }

            foreach (ZSingle single in Singles)
            {
                for (int i = 0; i < (int)(single.Layers.Count * 0.5); ++i)
                {
                    foreach (ZLayerParameter layerParameter in single.Layers[i].LayerParameters)
                    {
                        if (layerParametersToBeIgnored.Contains(layerParameter.ID) || layerParameter.IsReadOnly(false, true)) continue;
                        string mirrorValue = single.Layers[single.Layers.Count - 1 - i].GetLayerParameterValue(layerParameter.ID);
                        if (string.IsNullOrWhiteSpace(layerParameter.Value) || string.IsNullOrWhiteSpace(mirrorValue)) continue;
                        if (layerParameter.Value != mirrorValue) return false;
                    }
                }
            }

            foreach(ZPair pair in Pairs)
            {
                for (int i = 0; i < (int)(pair.Layers.Count * 0.5); ++i)
                {
                    foreach (ZLayerParameter layerParameter in pair.Layers[i].LayerParameters)
                    {
                        if (layerParametersToBeIgnored.Contains(layerParameter.ID) || layerParameter.IsReadOnly(false, true)) continue;
                        string mirrorValue = pair.Layers[pair.Layers.Count - 1 - i].GetLayerParameterValue(layerParameter.ID);
                        if (string.IsNullOrWhiteSpace(layerParameter.Value) || string.IsNullOrWhiteSpace(mirrorValue)) continue;
                        if (layerParameter.Value != mirrorValue) return false;
                    }
                }
            }

            return true;
        }

        public void CorrectDefaultParametersForInnerOuterLayers()
        {
            List<ZLayer> outerLayers = new List<ZLayer>();
            List<ZLayer> metallLayers = GetMetallLayerCollection();
            if (metallLayers.Count > 0) outerLayers.Add(metallLayers[0]);
            if (metallLayers.Count > 1) outerLayers.Add(metallLayers[metallLayers.Count - 1]);

            foreach (ZLayer layer in outerLayers)
            {
                layer.SetLayerParameterValue(ZStringConstants.ParameterIDCopperThickness, (Options.TheOptions.base_trace_thickness + Options.TheOptions.plating_thickness).ToString(/*"N" + Settings.Options.TheOptions.lengthDigits,*/ CultureInfo.InvariantCulture));
                layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, ZPlannerManager.GetThicknesStringByCopperWeight(Options.TheOptions.base_trace_thickness + Options.TheOptions.plating_thickness));
                layer.SetLayerParameterValue(ZStringConstants.ParameterIDComments, "Includes plating");

                foreach (ZSingle single in Singles)
                {
                    ZLayer singleLayer = this.GetLayerOfSingleImpedance(layer.ID, single.ID);
                    singleLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZo_TraceWidth, Options.TheOptions.outer_trace_width.ToString(/*"N" + Settings.Options.TheOptions.lengthDigits,*/ CultureInfo.InvariantCulture));
                }

                foreach(ZPair pair in Pairs)
                {
                    ZLayer pairLayer = this.GetLayerOfPairImpedance(layer.ID, pair.ID);
                    pairLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TraceWidth, Options.TheOptions.outer_diff_trace_width.ToString(/*"N" + Settings.Options.TheOptions.lengthDigits,*/ CultureInfo.InvariantCulture));
                    pairLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TraceSpacing, Options.TheOptions.outer_diff_trace_spacing.ToString(/*"N" + Settings.Options.TheOptions.lengthDigits,*/ CultureInfo.InvariantCulture));
                    pairLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TracePitch, (Options.TheOptions.outer_diff_trace_spacing + Options.TheOptions.outer_diff_trace_width).ToString(/*"N" + Settings.Options.TheOptions.lengthDigits,*/ CultureInfo.InvariantCulture));
                }
            }
        }

        public string GetMovedFromStackupLayerParameterValue(ZLayerParameter layerParameter)
        {
            // For Diff pairs only
            string value = null;
            double frequency;
            ZLayer stackupLayer;

            string isUsed = layerParameter.Layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_IsUsed);

            switch(layerParameter.ID)
            { 
                case ZStringConstants.ParameterIDZdiff_BitRate:
                    if (isUsed == null || isUsed.ToLower() != "true") return string.Empty;
                    if (!layerParameter.Layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_Frequency, out frequency))
                    {
                        ZLayerType? layerType = layerParameter.Layer.GetLayerType();
                        if (layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed) frequency = ZPlannerManager.Project.Stackup.GetFrequencyByLayer(layerParameter.Layer);
                        else frequency = double.NaN;
                    }

                    if (double.IsNaN(frequency)) return string.Empty;
                    return (2 * frequency).ToString(ZPlannerManager.GetFormatByParameterID(layerParameter.ID), CultureInfo.InvariantCulture);

                case ZStringConstants.ParameterIDZdiff_UnitInterval:
                    if (isUsed == null || isUsed.ToLower() != "true") return string.Empty;
                    if (!layerParameter.Layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_Frequency, out frequency))
                    {
                        ZLayerType? layerType = layerParameter.Layer.GetLayerType();
                        if (layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed) frequency = ZPlannerManager.Project.Stackup.GetFrequencyByLayer(layerParameter.Layer);
                        else frequency = double.NaN;
                    }

                    if (double.IsNaN(frequency) || frequency == 0) return string.Empty;
                    return (500 / frequency).ToString(ZPlannerManager.GetFormatByParameterID(layerParameter.ID), CultureInfo.InvariantCulture);


                case ZStringConstants.ParameterIDZdiff_SkewTolerance:
                    if (isUsed == null || isUsed.ToLower() != "true") return string.Empty;
                    if (!layerParameter.Layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_Frequency, out frequency))
                    {
                        ZLayerType? layerType = layerParameter.Layer.GetLayerType();
                        if (layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed) frequency = ZPlannerManager.Project.Stackup.GetFrequencyByLayer(layerParameter.Layer);
                        else frequency = double.NaN;
                    }

                    if (double.IsNaN(frequency) || frequency == 0) return string.Empty;
                    return (125 / frequency).ToString(ZPlannerManager.GetFormatByParameterID(layerParameter.ID), CultureInfo.InvariantCulture);

                case ZStringConstants.ParameterIDZdiff_RoughTop:
                    if (isUsed == null || isUsed.ToLower() != "true") return string.Empty;
                    stackupLayer = layerParameter.Layer.Stackup.GetLayerOfStackup(layerParameter.Layer.ID);
                    if (stackupLayer != null)
                    {
                        value = stackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDRoughTop);
                        if (value == null || string.IsNullOrWhiteSpace(value))
                        {
                            ZLayerType? layerType = stackupLayer.GetLayerType();
                            if (layerType == ZLayerType.Plane || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed) value = stackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDCalcRoughTop);
                        }
                    }
                    return value??string.Empty;

                case ZStringConstants.ParameterIDZdiff_RoughBottom:
                    if (isUsed == null || isUsed.ToLower() != "true") return string.Empty;
                    stackupLayer = layerParameter.Layer.Stackup.GetLayerOfStackup(layerParameter.Layer.ID);
                    if (stackupLayer != null)
                    {
                        value = stackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDRoughBottom);
                        if (value == null || string.IsNullOrWhiteSpace(value))
                        {
                            ZLayerType? layerType = stackupLayer.GetLayerType();
                            if (layerType == ZLayerType.Plane || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed) value = stackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDCalcRoughBottom);
                        }
                    }
                    return value??string.Empty;

                case ZStringConstants.ParameterIDZdiff_WeavePitch:
                    stackupLayer = layerParameter.Layer.Stackup.GetLayerOfStackup(layerParameter.Layer.ID);
                    if (stackupLayer != null)
                    {
                        value = stackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDWeavePitch);
                    }
                    return value ?? string.Empty;


                case ZStringConstants.ParameterIDZdiff_FillPitch:
                    stackupLayer = layerParameter.Layer.Stackup.GetLayerOfStackup(layerParameter.Layer.ID);
                    if (stackupLayer != null)
                    {
                        value = stackupLayer.GetLayerParameterValue(ZStringConstants.ParameterIDFillPitch);
                    }
                    return value ?? string.Empty;
                default :
                    return layerParameter.Value;
            }
        }

        public ZList<ZSingle> Singles { get; private set; }
        public ZList<ZPair> Pairs { get; private set; }
        public ZList<ZLayer> Layers { get; private set; }
        public ZList<ZSpan> Spans { get; private set; }

        public ZPlannerProject Project { get; private set; }

        ZPair _activePair;
        internal ZPair ActivePair
        {
            get
            {
                return _activePair;
            }
            set
            {
                if (_activePair != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeStackupActivePairCommand(this, _activePair, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _activePair = value;
                    NotifyPropertyChanged("ActivePair");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        ZSingle _activeSingle;
        internal ZSingle ActiveSingle
        {
            get
            {
                return _activeSingle;
            }
            set
            {
                if (_activeSingle != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeStackupActiveSingleCommand(this, _activeSingle, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _activeSingle = value;
                    NotifyPropertyChanged("ActiveSingle");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        public double GetBoardThickness()
        {
            double h = 0;
            foreach (ZLayer layer in Layers)
            {
                double dval = 0;
                ZPlannerProject.GetLayerParameterValue(layer, ZStringConstants.ParameterIDThickness, ref dval);
                h += dval;
            }

            return h;
        }

        //roughness data
        private double outer_layer_roughness;
        internal double OuterLayerRoughness
        {
            get
            {
                return outer_layer_roughness;
            }
            set
            {
                if (outer_layer_roughness != value)
                {
                    outer_layer_roughness = value;
                    NotifyPropertyChanged("OuterLayerRoughness");
                }
            }
        }

        private double laminate_side_roughness;

        internal double LaminateSideRoughness
        {
            get
            {
                return laminate_side_roughness;
            }
            set
            {
                if (laminate_side_roughness != value)
                {
                    laminate_side_roughness = value;
                    NotifyPropertyChanged("LaminateSideRoughness");
                }
            }
        }

        private double prepreg_side_roughness;
        internal double PrepregSideRoughness
        {
            get
            {
                return prepreg_side_roughness;
            }
            set
            {
                if (prepreg_side_roughness != value)
                {
                    prepreg_side_roughness = value;
                    NotifyPropertyChanged("PrepregSideRoughness");
                }
            }
        }

        private double core_side_roughness_hte;
        internal double CoreSideRoughnessHTE
        {
            get
            {
                return core_side_roughness_hte;
            }
            set
            {
                if (core_side_roughness_hte != value)
                {
                    core_side_roughness_hte = value;
                    NotifyPropertyChanged("CoreSideRoughnessHTE");
                }
            }
        }

        private double core_side_roughness_rtf;
        internal double CoreSideRoughnessRTF
        {
            get
            {
                return core_side_roughness_rtf;
            }
            set
            {
                if (core_side_roughness_rtf != value)
                {
                    core_side_roughness_rtf = value;
                    NotifyPropertyChanged("CoreSideRoughnessRTF");
                }
            }
        }

        private double core_side_roughness_vlp;
        internal double CoreSideRoughnessVLP
        {
            get
            {
                return core_side_roughness_vlp;
            }
            set
            {
                if (core_side_roughness_vlp != value)
                {
                    core_side_roughness_vlp = value;
                    NotifyPropertyChanged("CoreSideRoughnessVLP");
                }
            }
        }

        private double core_side_roughness_vlp2;
        internal double CoreSideRoughnessVLP2
        {
            get
            {
                return core_side_roughness_vlp2;
            }
            set
            {
                if (core_side_roughness_vlp2 != value)
                {
                    core_side_roughness_vlp2 = value;
                    NotifyPropertyChanged("CoreSideRoughnessVLP2");
                }
            }
        }

        private double core_side_roughness_hvlp;
        internal double CoreSideRoughnessHVLP
        {
            get
            {
                return core_side_roughness_hvlp;
            }
            set
            {
                if (core_side_roughness_hvlp != value)
                {
                    core_side_roughness_hvlp = value;
                    NotifyPropertyChanged("CoreSideRoughnessHVLP");
                }
            }
        }

        //prepreg-side roughness
        private double prepreg_side_roughness_mb100le;
        internal double PrepregSideRoughnessMB100LE
        {
            get
            {
                return prepreg_side_roughness_mb100le;
            }
            set
            {
                if (prepreg_side_roughness_mb100le != value)
                {
                    prepreg_side_roughness_mb100le = value;
                    NotifyPropertyChanged("PrepregSideRoughnessMB100LE");
                }
            }
        }

        private double prepreg_side_roughness_alphaprep;
        internal double PrepregSideRoughnessAlphaPrep
        {
            get
            {
                return prepreg_side_roughness_alphaprep;
            }
            set
            {
                if (prepreg_side_roughness_alphaprep != value)
                {
                    prepreg_side_roughness_alphaprep = value;
                    NotifyPropertyChanged("PrepregSideRoughnessAlphaPrep");
                }
            }
        }

        private double prepreg_side_roughness_mb100zk;
        internal double PrepregSideRoughnessMB100ZK
        {
            get
            {
                return prepreg_side_roughness_mb100zk;
            }
            set
            {
                if (prepreg_side_roughness_mb100zk != value)
                {
                    prepreg_side_roughness_mb100zk = value;
                    NotifyPropertyChanged("PrepregSideRoughnessMB100ZK");
                }
            }
        }

        private double prepreg_side_roughness_bf;
        internal double PrepregSideRoughnessBF
        {
            get
            {
                return prepreg_side_roughness_bf;
            }
            set
            {
                if (prepreg_side_roughness_bf != value)
                {
                    prepreg_side_roughness_bf = value;
                    NotifyPropertyChanged("PrepregSideRoughnessBF");
                }
            }
        }

        private double prepreg_side_roughness_cz8100;
        internal double PrepregSideRoughnessCZ8100
        {
            get
            {
                return prepreg_side_roughness_cz8100;
            }
            set
            {
                if (prepreg_side_roughness_cz8100 != value)
                {
                    prepreg_side_roughness_cz8100 = value;
                    NotifyPropertyChanged("PrepregSideRoughnessCZ8100");
                }
            }
        }

        private double prepreg_side_roughness_mb100hp;
        internal double PrepregSideRoughnessMB100HP
        {
            get
            {
                return prepreg_side_roughness_mb100hp;
            }
            set
            {
                if (prepreg_side_roughness_mb100hp != value)
                {
                    prepreg_side_roughness_mb100hp = value;
                    NotifyPropertyChanged("PrepregSideRoughnessMB100HP");
                }
            }
        }

        private OxideAlternativeTreatmentsType? oxide_alternative_treatments = null;
        internal OxideAlternativeTreatmentsType? OxideAlternativeTreatments
        {
            get
            {
                return oxide_alternative_treatments;
            }
            set
            {
                if (oxide_alternative_treatments != value)
                {
                    oxide_alternative_treatments = value;
                    NotifyPropertyChanged("OxideAlternativeTreatments");
                }
            }
        }

        private ZCopperCoverageType copperCoverageType;
        internal ZCopperCoverageType CopperCoverageType
        {
            get
            {
                return copperCoverageType;
            }
            set
            {
                if (copperCoverageType != value)
                {
                    copperCoverageType = value;
                    NotifyPropertyChanged("CopperCoverageType");
                }
            }
        }

        private double forSignal;
        internal double ForSignal
        {
            get
            {
                return forSignal;
            }
            set
            {
                if (forSignal != value)
                {
                    forSignal = value;
                    NotifyPropertyChanged("ForSignal");
                }
            }
        }

        private double forMixed;
        internal double ForMixed
        {
            get
            {
                return forMixed;
            }
            set
            {
                if (forMixed != value)
                {
                    forMixed = value;
                    NotifyPropertyChanged("ForMixed");
                }
            }
        }

        private double forPlane;
        internal double ForPlane
        {
            get
            {
                return forPlane;
            }
            set
            {
                if (forPlane != value)
                {
                    forPlane = value;
                    NotifyPropertyChanged("ForPlane");
                }
            }
        }

        private double forSignalSignal;
        internal double ForSignalSignal
        {
            get
            {
                return forSignalSignal;
            }
            set
            {
                if (forSignalSignal != value)
                {
                    forSignalSignal = value;
                    NotifyPropertyChanged("ForSignalSignal");
                }
            }
        }

        private double forSignalMixed;
        internal double ForSignalMixed
        {
            get
            {
                return forSignalMixed;
            }
            set
            {
                if (forSignalMixed != value)
                {
                    forSignalMixed = value;
                    NotifyPropertyChanged("ForSignalMixed");
                }
            }
        }

        private double forSignalPlane;
        internal double ForSignalPlane
        {
            get
            {
                return forSignalPlane;
            }
            set
            {
                if (forSignalPlane != value)
                {
                    forSignalPlane = value;
                    NotifyPropertyChanged("ForSignalPlane");
                }
            }
        }

        private double forMixedMixed;
        internal double ForMixedMixed
        {
            get
            {
                return forMixedMixed;
            }
            set
            {
                if (forMixedMixed != value)
                {
                    forMixedMixed = value;
                    NotifyPropertyChanged("ForMixedMixed");
                }
            }
        }

        private double forMixedPlane;
        internal double ForMixedPlane
        {
            get
            {
                return forMixedPlane;
            }
            set
            {
                if (forMixedPlane != value)
                {
                    forMixedPlane = value;
                    NotifyPropertyChanged("ForMixedPlane");
                }
            }
        }

        private double forPlanePlane;
        internal double ForPlanePlane
        {
            get
            {
                return forPlanePlane;
            }
            set
            {
                if (forPlanePlane != value)
                {
                    forPlanePlane = value;
                    NotifyPropertyChanged("ForPlanePlane");
                }
            }
        }

        private double etch;
        internal double Etch
        {
            get
            {
                return etch;
            }
            set
            {
                if (etch != value)
                {
                    etch = value;
                    NotifyPropertyChanged("Etch");
                }
            }
        }

        private double _frequency;
        internal double Frequency 
        { 
            get
            { 
                return _frequency;
            }
            set
            {
                if (_frequency != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeStackupFrequencyCommand(this, _frequency, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    _frequency = value;
                    NotifyPropertyChanged("Frequency");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        internal bool IsFileOpened;
        internal bool IsRoughness;
        internal bool IsPressedThickness;
        internal bool IsSequentialLamination;
        internal bool IsTrapezoidalTraces;
        internal bool IsLossPlanning;
        internal bool IsGlassPitch;
        internal bool IsGws;
        internal bool IsCenterLineVisible;
        internal bool IsCoreLocked;
        internal bool IsAutoMirror;
        internal bool IsHeadersVisible;
        internal bool IsColorDisabledCells;
        internal bool IsSequentialLaminationSetByUser;
        internal bool IsKeepImportedPressedThickness;

        internal double GetFrequencyByLayer(ZLayer layer)
        {
            if (layer == null) return Frequency;
            return DefineDielectricFrequency(layer);
        }
        
        public int GetMetallLayerCount()
        {
            List<ZLayer> collection = GetMetallLayerCollection();
            return collection.Count;
        }

        public List<ZLayer> GetMetallLayerCollection(bool excludePlane = false)
        {
            List<ZLayer> collection = new List<ZLayer>();
            foreach (ZLayer layer in Layers)
            {
                ZLayerType? layerType;
                layerType = layer.GetLayerType();
                if ((!excludePlane && layerType == ZLayerType.Plane) || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed)
                    collection.Add(layer);
            }
            return collection;
        }

        public void CalculateLayerNumber()
        {
            int index = 1;
            foreach (ZLayer layer in Layers)
            {
                ZLayerType? layerType = layer.GetLayerType();
                if (layerType == ZLayerType.Plane || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed)
                {
                    if (layer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName) == string.Empty) layer.SetLayerParameterValue(ZStringConstants.ParameterIDLayerName, layerType.ToString() + "-" + index);
                    if (ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDLayerNumber, index)) ++index;
                }
                else 
                {
                    ZPlannerProject.SetLayerParameterValue(layer, ZStringConstants.ParameterIDLayerNumber, "");
                }
            }

            ZPlannerManager.StatusMenu.SetStackupParameters(this);
        }

        public void SetAllLayerParameterValues(string parameterName, string oldValue, string newValue)
        {
            foreach(ZLayer layer in Layers)
            {
                ZLayerParameter layerParameter = layer.LayerParameters.Find(x => x.ID == parameterName);
                if (layerParameter == null) continue;

                if (layerParameter.Parameter.ValueType == ZValueType.Number || layerParameter.Parameter.ValueType == ZValueType.Percent)
                {
                    double dOldValue, dValue;
                    if (double.TryParse (oldValue, NumberStyles.Any, CultureInfo.InvariantCulture, out dOldValue) && 
                        double.TryParse (layerParameter.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue) &&
                        dValue == dOldValue) layerParameter.Value = newValue;
                }
                else
                {
                    if (layerParameter.Value == oldValue) layerParameter.Value = newValue;
                }
            }
        }

        //--------------- calculate layer dynamic properties in the stackup ------------------------------------
        public void DefineRoughness()
        {
            // for metal layers which are (not Edited) or Empty ==> get roughness by neighbours
            foreach (ZLayer zl in Layers)
            {
                switch (zl.GetLayerType())
                {
                    case ZLayerType.Signal:
                    case ZLayerType.Plane:
                    case ZLayerType.SplitMixed:
                        //ZLayerParameter parRoughTop = zl.LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDRoughTop);
                        //ZLayerParameter parRoughBottom = zl.LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDRoughBottom);
                        
                        double rough_top = 0, rough_bottom = 0;
                        bool b = GetLayerRoughness(zl, ref rough_top, ref rough_bottom);

                        if (b)
                        {
                            ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDCalcRoughTop, rough_top);
                            ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDCalcRoughBottom, rough_bottom);

//                                ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDRoughTop, rough_top);
//                                ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDRoughBottom, rough_bottom);
                        }
/*
                        if (b && rough_top != 0)
                        {
                            if (!parRoughTop.IsEdited || parRoughTop.ToString() == String.Empty)
                            {
                                ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDCalcRoughTop, rough_top); 
                            }
                        }
                        if (parRoughBottom != null && rough_bottom != 0)
                        {
                            if (!parRoughBottom.IsEdited || parRoughBottom.ToString() == String.Empty)
                            {
                                ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDCalcRoughBottom, rough_bottom); 
                            }
                        }
*/ 
                        break;
                }
            }
        }

        private double CheckFoilTreatment(ZLayer zl)
        {
            double retval = -1;
            string sval = "";
            if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDFoilTreatment, ref sval)){
                if (sval.Contains("HTE"))
                {
                    retval = CoreSideRoughnessHTE; // 7.5;
                }
                else if (sval.Contains("RTF"))
                {
                    retval = CoreSideRoughnessRTF; // 5;
                }
                else if (sval.Contains("VLP"))
                {
                    retval = CoreSideRoughnessVLP; // 3;
                    if (sval.Contains("VLP-2") || sval.Contains("VLP 2") || sval.Contains("VLP2"))
                    {
                        retval = CoreSideRoughnessVLP2; // 2;
                    }
                    else if (sval.Contains("HVLP"))
                    {
                        retval = CoreSideRoughnessHVLP; // 1;
                    }
                }
            }

            return retval;
        }

        private double CheckPrepregSideRoughness()
        {
            double retval = 0;
            if (OxideAlternativeTreatments == null) return retval;
            switch (OxideAlternativeTreatments)
            {
                case OxideAlternativeTreatmentsType.MacDermidMultiBond100LE:
                    retval = PrepregSideRoughnessMB100LE;
                    break;
                case OxideAlternativeTreatmentsType.MacDermidEnthoneAlphaPrep:
                    retval = PrepregSideRoughnessAlphaPrep;
                    break;
                case OxideAlternativeTreatmentsType.MacDermidMultiBond100ZK:
                    retval = PrepregSideRoughnessMB100ZK;
                    break;
                case OxideAlternativeTreatmentsType.AtotechBondFilm:
                    retval = PrepregSideRoughnessBF;
                    break;
                case OxideAlternativeTreatmentsType.MECetchBONDCZ8100:
                    retval = PrepregSideRoughnessCZ8100;
                    break;
                case OxideAlternativeTreatmentsType.MacDermidMultiBond100HP:
                    retval = PrepregSideRoughnessMB100HP;
                    break;
            }
            return retval;
        }

        private bool GetLayerRoughness(ZLayer zl, ref double rough_top, ref double rough_bottom)
        {
            rough_top = rough_bottom = 0;

            double prepreg_side_roughness = CheckPrepregSideRoughness();
            if (prepreg_side_roughness == 0) prepreg_side_roughness = PrepregSideRoughness;


            int idx = GetLayerOfStackupIndex(zl.ID);
            //double check if layer is metal
            switch (zl.GetLayerType())
            {
                case ZLayerType.Signal:
                case ZLayerType.Plane:
                case ZLayerType.SplitMixed:
                    break;
                default: return false; 
            }

            ZLayer lPrev = null, lNext = null;
            if (idx > 0){
                lPrev = GetLayerOfStackup(idx - 1);
            }
            if (idx < Layers.Count - 1){
                 lNext = GetLayerOfStackup(idx + 1);  
            }

            if ((lPrev == null) || lPrev.GetLayerType() == ZLayerType.SolderMask) //outer top layer
            {
                double r = CheckFoilTreatment(zl);
                if (r > 0)
                {
                    rough_top = r;
                }
                else
                {
                    rough_top = OuterLayerRoughness;
                }
            }
            else
            {
                if (lPrev.GetLayerType() == ZLayerType.Prepreg)
                {
                    rough_top = prepreg_side_roughness;// PrepregSideRoughness;
                }
                else
                {
                    double r = CheckFoilTreatment(zl);
                    if (r > 0)
                    {
                        rough_top = r;
                    }
                    else
                    {
                        rough_top = LaminateSideRoughness;
                    }
                }
            }

            if ((lNext == null) || lNext.GetLayerType() == ZLayerType.SolderMask) //outer bottom layer
            {
                double r = CheckFoilTreatment(zl);
                if (r > 0)
                {
                    rough_bottom = r;
                }
                else
                {
                    rough_bottom = OuterLayerRoughness;
                }
            }
            else { 
                if (lNext.GetLayerType() == ZLayerType.Prepreg)
                {
                    rough_bottom = prepreg_side_roughness;// PrepregSideRoughness;
                }
                else
                {
                    double r = CheckFoilTreatment(zl);
                    if (r > 0)
                    {
                        rough_bottom = r;
                    }
                    else
                    {
                        rough_bottom = LaminateSideRoughness;
                    }
                }
            }

            return true;
        }

        private double DefineDielectricFrequency(ZLayer zl)
        {
            ZList<ZLayer> layers;
            if (zl.Owner is ZSingle) layers = (zl.Owner as ZSingle).Layers;
            else if (zl.Owner is ZPair) layers = (zl.Owner as ZPair).Layers;
            else layers = Layers;

            double f = Frequency;//default is the stackup frequency

            if (zl.isMetal()) return f;

            //-------------------------------------------
            int idx = GetLayerOfStackup(zl.ID).Order; // GetLayerOfStackupIndex(zl.ID);

            double fPrev = f;
            double fNext = f;

            ZLayer l;
            //find upper signal
            bool b = false;
            for (int i = idx - 1; i >= 0; i--)
            {
                l = layers[i];
                switch (l.GetLayerType())
                {
                    case ZLayerType.Plane:
                        fPrev = 0;
                        b = true;
                        break;
                    case ZLayerType.Signal:
                    case ZLayerType.SplitMixed:
                        fPrev = l.GetFrequency();
                        b = true;
                        break;
                }
                if (b) break;
            }

           //find lower signal
            b = false;
            for (int i = idx + 1; i < layers.Count; i++)
            {
                l = layers[i];
                switch (l.GetLayerType())
                {
                    case ZLayerType.Plane:
                        fNext = 0;
                        b = true;
                        break;
                    case ZLayerType.Signal:
                    case ZLayerType.SplitMixed:
                        fNext = l.GetFrequency();
                        b = true;
                        break;
                }
                if (b) break;
            }

            //
            double fmax = Math.Max(fPrev, fNext);
            return fmax > 0 ? fmax : Frequency;
        }

        public void DefineTrapezoids()
        {
            //we don't check for the right dielectric sequence here
            //wide side is facing core
            for (int i = 0; i < Layers.Count; i++)
            {
                bool bNarrowTop = true; //default
                bool bDefined = false;
                ZLayer zl = Layers[i];
                switch (zl.GetLayerType())
                {
                    case ZLayerType.Signal:
                    case ZLayerType.SplitMixed:
                    case ZLayerType.Plane:
                        //try core
                        //try previous layer
                        if (!bDefined && i > 0)
                        {
                            ZLayer zPrev = Layers[i - 1];
                            if(zPrev.GetLayerType() == ZLayerType.Core)
                            {
                                bNarrowTop = false;
                                bDefined = true;
                            }
                        }

                        //try next layer
                        if (!bDefined && i < Layers.Count - 1)
                        {
                            ZLayer zNext = Layers[i + 1];
                            if(zNext.GetLayerType() == ZLayerType.Core)
                            {                                
                                bNarrowTop = true;
                                bDefined = true;
                            }
                        }

                        if (!bDefined)
                        {
                            //if can't define by Core then just ensure symmetry
                            bNarrowTop = i < Layers.Count / 2;
                            bDefined = true;
                        }
#if false
                        //try prepreg
                        if (!bDefined && i > 0)
                        {
                            ZLayer zPrev = Layers[i-1];
                            if (zPrev.GetLayerType() == ZLayerType.Prepreg)
                            {
                                bNarrowTop = false;
                                bDefined = true;
                            }
                        }

                        //try next layer
                        if (!bDefined && i < Layers.Count - 1)
                        {
                            ZLayer zNext = Layers[i+1];
                            if (zNext.GetLayerType() == ZLayerType.Prepreg)
                            {
                                bNarrowTop = true;
                                bDefined = true;
                            }
                        }
#endif
                        //--ZLayer zSingleLayer = zl.Stackup.GetLayerOfSingleImpedance(zl.ID);
                        ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDNarrowTop, bNarrowTop);
                        break;
                }
            }
        }

        public void UpdateSequentialLamination()
        {
            bool bEnabled = ZPlannerManager.IsSequentialLamination;
            if (! bEnabled){
                foreach (ZLayer z in Layers)
                {
                    if (z.GetLayerType() == ZLayerType.Prepreg)
                    {
                        ZPlannerProject.SetLayerParameterValue(z, ZStringConstants.ParameterIDSequentialLamination, false);
                            //GetLayerParameterValue(zl, ZStringConstants.ParameterIDSequentialLamination, ref bSequentialLamination);
                    }
                }
            }
            else {
                int x = SequentialLamination();
                if (x < 0) x = 0;
                int n = GetMetallLayerCount() - (2 * x);
                if (n < 0) n = 0;
                if (x > 0)
                {
                    int nLayers = Layers.Count;
                    for (int i = 1; i <= nLayers / 2; i++)
                    {
                        ZLayer z = Layers[i];
                        if (z.GetLayerType() == ZLayerType.Core) break;
                        if (z.GetLayerType() == ZLayerType.Prepreg)
                        {
                            ZPlannerProject.SetLayerParameterValue(z, ZStringConstants.ParameterIDSequentialLamination, true);
                        }
                    }

                    for (int i = nLayers - 1; i >= nLayers / 2; i--)
                    {
                        ZLayer z = Layers[i];
                        if (z.GetLayerType() == ZLayerType.Core) break;
                        if (z.GetLayerType() == ZLayerType.Prepreg)
                        {
                            ZPlannerProject.SetLayerParameterValue(z, ZStringConstants.ParameterIDSequentialLamination, true);
                        }
                    }
                }
            }
        }

        public void UpdateGlassPitch()
        {
            foreach (ZLayer zl in Layers)
            {
                if (zl.isDielectric())
                {
                    GlassPitch gp = new GlassPitch(zl);
                    gp.Init();
                }
            }
        }

        public void CalculatePrepregAdjustedThickness(bool isAdjusted)
        {
            foreach (ZLayer zl in Layers)
            {
                if (zl.GetLayerType() == ZLayerType.Prepreg)
                {
                    double t = double.NaN;
                    if (isAdjusted)
                    {
                        bool bCalc = true;
                        if (IsKeepImportedPressedThickness)
                        {
                            double x = 0;
                            if (zl.GetLayerParameterValue(ZStringConstants.ParameterIDFabricatorThickness, out x)){
                                t = x;
                                bCalc = false;
                            }
                        }
                        
                        if (bCalc)
                            t = GetPrepregAdjustedThickness(zl);
                    }

                    ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDPrepregThickness, t);
                }
            }
        }

        //helper function
        private bool TypePair(ZLayerType? x, ZLayerType? y, ZLayerType a, ZLayerType b)
        {
            return (x == a && y == b) || (x == b && y == a);
        }

        private bool TypeSingle(ZLayerType? x, ZLayerType? y, ZLayerType a, ZLayerType b)
        {
            //TBD It was just coppyed. Alex, please, change this if it is required
            return (x == a && y == b) || (x == b && y == a);
        }

        ZLayer GetMetalUp(ZLayer prepreg, out bool bAdjacent)
        {
            bAdjacent = false;
            int idx = GetLayerOfStackupIndex(prepreg.ID);
            //can't be the first or the last
            if ((idx == 0) && (idx == Layers.Count-1)){
                return null;
            }

            for (int i = idx - 1; i >= 0; i--) { 
                ZLayer lPrev = GetLayerOfStackup(i);
                if (lPrev.isMetal())
                {
                    bAdjacent = (i == idx - 1);
                    return lPrev;
                }
            }

            return null;
        }

        ZLayer GetMetalDown(ZLayer prepreg, out bool bAdjacent)
        {
            bAdjacent = false;
            int idx = GetLayerOfStackupIndex(prepreg.ID);
            //can't be the first or the last
            if ((idx == 0) && (idx == Layers.Count - 1))
            {
                return null;
            }

            for (int i = idx + 1; i < Layers.Count; i++)
            {
                ZLayer lNext = GetLayerOfStackup(i);
                if (lNext.isMetal()) {
                    bAdjacent = (i == idx + 1);
                    return lNext;
                }
            }

            return null;
        }

        private bool isMicrostripPrepreg(ZLayer zl)
        {
            bool res = false;
            int idx = GetLayerOfStackupIndex(zl.ID);
            //check for microstrip on top
            //1. [Air]-Metal(0)-Prepreg(1)
            if (idx == 1)
            {
                if (Layers[0].isMetal()) return true;
            }
            //2. Soldermask(0)-Metal(1)-Prepreg(2)
            if (idx == 2)
            {
                if (Layers[1].isMetal() && Layers[0].GetLayerType() == ZLayerType.SolderMask) return true;
            }

            //check for microstrip on bottom
            int N = Layers.Count - 1; //last layer Idx
            //1. Prepreg(N-1)-Metal(N)-[Air]
            if (idx == N - 1)
            {
                if (Layers[N].isMetal()) return true;
            }
            //2. Prepreg(N-2)-Metal(N-1)-Soldermask(N)
            if (idx == N - 2)
            {
                if (Layers[N - 1].isMetal() && Layers[N].GetLayerType() == ZLayerType.SolderMask) return true;
            }

            return res;
        }

        public double RestorePrepregUnpressedThickness(ZLayer zl) //from pressed to unpressed
        {
            bool canAdjust = false;
            ZLayerType? type = zl.GetLayerType();

            double base_thicknes = 0;
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDPrepregThickness, ref base_thicknes);

            double new_thickness = base_thicknes;

            if (type == ZLayerType.Prepreg)
            {
                bool bUp = false, bDown = false;
                ZLayer lPrev = null;
                ZLayer lNext = null;


                bool bSequentialLamination = false;
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDSequentialLamination, ref bSequentialLamination);
                bool bSeqLam = isMicrostripPrepreg(zl) || bSequentialLamination;
                int nLayers = Layers.Count;
                int middle = nLayers / 2; //0-based

                bool bUpperHalf = true;
                int idx = 0;
                foreach (ZLayer l in Layers)
                {
                    if (l.ID == zl.ID) break;
                    idx++;
                    if (idx > middle)
                    {
                        bUpperHalf = false;
                        break;
                    }
                }

                lPrev = GetMetalUp(zl, out bUp);
                lNext = GetMetalDown(zl, out bDown);
                canAdjust = lPrev != null && lNext != null;

                //now do the adjustment
                if (canAdjust)
                {
                    ZLayerType? tprev = lPrev.GetLayerType();
                    ZLayerType? tnext = lNext.GetLayerType();

                    double coef = 1;
                    switch (CopperCoverageType)
                    {
                        case ZCopperCoverageType.PrepregPercent:
                            // coper coverage of neighbors
                            if (TypePair(tprev, tnext, ZLayerType.Signal, ZLayerType.Signal))
                            {
                                coef = ForSignalSignal / 100;
                            }
                            else if (TypePair(tprev, tnext, ZLayerType.Signal, ZLayerType.SplitMixed))
                            {
                                coef = ForSignalMixed / 100;
                            }
                            else if (TypePair(tprev, tnext, ZLayerType.Signal, ZLayerType.Plane))
                            {
                                coef = ForSignalPlane / 100;
                            }
                            else if (TypePair(tprev, tnext, ZLayerType.SplitMixed, ZLayerType.SplitMixed))
                            {
                                coef = ForMixedMixed / 100;
                            }
                            else if (TypePair(tprev, tnext, ZLayerType.SplitMixed, ZLayerType.Plane))
                            {
                                coef = ForMixedPlane / 100;
                            }
                            else if (TypePair(tprev, tnext, ZLayerType.Plane, ZLayerType.Plane))
                            {
                                coef = ForPlanePlane / 100;
                            }
                            new_thickness /= coef;
                            break;
                        case ZCopperCoverageType.PrepregProportional:
                            // pressed between
                            double wPrev = 0, wNext = 0;
                            double metal_thickness = 0;
                            double copperPercent = 0;

                            //upper layer
                            ZPlannerProject.GetLayerParameterValue(lPrev, ZStringConstants.ParameterIDThickness, ref metal_thickness);
                            switch (tprev)
                            {
                                case ZLayerType.Signal: wPrev = metal_thickness * (1 - ForSignal / 100); break;
                                case ZLayerType.Plane: wPrev = metal_thickness * (1 - ForPlane / 100); break;
                                case ZLayerType.SplitMixed: wPrev = metal_thickness * (1 - ForMixed / 100); break;
                            }

                            //lower layer
                            ZPlannerProject.GetLayerParameterValue(lNext, ZStringConstants.ParameterIDThickness, ref metal_thickness);
                            switch (tnext)
                            {
                                case ZLayerType.Signal: wNext = metal_thickness * (1 - ForSignal / 100); break;
                                case ZLayerType.Plane: wNext = metal_thickness * (1 - ForPlane / 100); break;
                                case ZLayerType.SplitMixed: wNext = metal_thickness * (1 - ForMixed / 100); break;
                            }

                            double delta = 0;
                            if (bUp)
                            {
                                bool bIgnoreUpper = bSeqLam && bUpperHalf;
                                if (!bIgnoreUpper) delta += wPrev;
                            }
                            if (bDown)
                            {
                                bool bIgnoreLower = bSeqLam && !bUpperHalf;
                                if (!bIgnoreLower) delta += wNext;
                            }

                            new_thickness = base_thicknes + delta;
                            break;
                        case ZCopperCoverageType.ManuallyEntered:
                        case ZCopperCoverageType.RestoredOnImport:
                            // manually entered values
                            wPrev = 0;
                            wNext = 0;
                            metal_thickness = 0;
                            copperPercent = 0;

                            //upper layer
                            ZPlannerProject.GetLayerParameterValue(lPrev, ZStringConstants.ParameterIDThickness, ref metal_thickness);
                            bool bCopper = ZPlannerProject.GetLayerParameterValue(lPrev, ZStringConstants.ParameterIDCopperPercent, ref copperPercent);
                            if (bCopper && copperPercent > 0)
                            {
                                wPrev = metal_thickness * (1 - copperPercent / 100);
                            }
                            else
                            {
                                switch (tprev)
                                {
                                    case ZLayerType.Signal: wPrev = metal_thickness * (1 - ForSignal / 100); break;
                                    case ZLayerType.Plane: wPrev = metal_thickness * (1 - ForPlane / 100); break;
                                    case ZLayerType.SplitMixed: wPrev = metal_thickness * (1 - ForMixed / 100); break;
                                }
                            }

                            //lower layer
                            ZPlannerProject.GetLayerParameterValue(lNext, ZStringConstants.ParameterIDThickness, ref metal_thickness);
                            bCopper = ZPlannerProject.GetLayerParameterValue(lNext, ZStringConstants.ParameterIDCopperPercent, ref copperPercent);
                            if (bCopper && copperPercent > 0)
                            {
                                wNext = metal_thickness * (1 - copperPercent / 100);
                            }
                            else
                            {
                                switch (tnext)
                                {
                                    case ZLayerType.Signal: wNext = metal_thickness * (1 - ForSignal / 100); break;
                                    case ZLayerType.Plane: wNext = metal_thickness * (1 - ForPlane / 100); break;
                                    case ZLayerType.SplitMixed: wNext = metal_thickness * (1 - ForMixed / 100); break;
                                }
                            }

                            delta = 0;
                            if (bUp)
                            {
                                bool bIgnoreUpper = bSeqLam && bUpperHalf;
                                if (!bIgnoreUpper) delta += wPrev;
                            }
                            if (bDown)
                            {
                                bool bIgnoreLower = bSeqLam && !bUpperHalf;
                                if (!bIgnoreLower) delta += wNext;
                            }

                            new_thickness = base_thicknes + delta;
                            break;
                    }
                }
            }

            return new_thickness;
        }

        public double GetPrepregAdjustedThickness(ZLayer zl)
        {
            bool canAdjust = false;
            ZLayerType? type = zl.GetLayerType();

            double base_thicknes = 0;
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDOriginThickness, ref base_thicknes);

            double new_thickness = base_thicknes;

            if (type == ZLayerType.Prepreg)
            {
                bool bUp = false, bDown = false;
                ZLayer lPrev = null;
                ZLayer lNext = null;


                bool bSequentialLamination = false;
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDSequentialLamination, ref bSequentialLamination);
                bool bSeqLam = isMicrostripPrepreg(zl) || bSequentialLamination;
                int nLayers = Layers.Count;
                int middle = nLayers / 2; //0-based

                bool bUpperHalf = true;
                int idx = 0;
                foreach( ZLayer l in Layers)
                {
                    if (l.ID == zl.ID) break;
                    idx++;
                    if (idx > middle)
                    {
                        bUpperHalf = false;
                        break;
                    }
                }

                lPrev = GetMetalUp(zl, out bUp);
                lNext = GetMetalDown(zl, out bDown);
                canAdjust = lPrev != null && lNext != null;

                //now do the adjustment
                if (canAdjust)
                {
                    ZLayerType? tprev = lPrev.GetLayerType();
                    ZLayerType? tnext = lNext.GetLayerType();

                    double coef = 1;
                    switch(CopperCoverageType)
                    { 
                        case ZCopperCoverageType.PrepregPercent:
                            // coper coverage of neighbors
                            if (TypePair(tprev, tnext, ZLayerType.Signal, ZLayerType.Signal))
                            {
                                coef = ForSignalSignal / 100;
                            }
                            else if (TypePair(tprev, tnext, ZLayerType.Signal,ZLayerType.SplitMixed))
                            {
                                coef = ForSignalMixed / 100;
                            }
                            else if (TypePair(tprev, tnext, ZLayerType.Signal, ZLayerType.Plane))
                            {
                                coef = ForSignalPlane / 100;
                            }
                            else if (TypePair(tprev, tnext, ZLayerType.SplitMixed, ZLayerType.SplitMixed))
                            {
                                coef = ForMixedMixed / 100;
                            }
                            else if (TypePair(tprev, tnext, ZLayerType.SplitMixed, ZLayerType.Plane))
                            {
                                coef = ForMixedPlane / 100;
                            }
                            else if (TypePair(tprev, tnext, ZLayerType.Plane, ZLayerType.Plane))
                            {
                                coef = ForPlanePlane / 100;
                            }
                            new_thickness *= coef;
                            break;
                        case ZCopperCoverageType.PrepregProportional: 
                            // pressed between
                            double wPrev = 0, wNext = 0;
                            double metal_thickness = 0;
                            double copperPercent = 0;

                            //upper layer
                            ZPlannerProject.GetLayerParameterValue(lPrev, ZStringConstants.ParameterIDThickness, ref metal_thickness);                           
                            switch (tprev)
                            {
                                case ZLayerType.Signal: wPrev = metal_thickness * (1 - ForSignal / 100); break;
                                case ZLayerType.Plane: wPrev = metal_thickness * (1 - ForPlane / 100); break;
                                case ZLayerType.SplitMixed: wPrev = metal_thickness * (1 - ForMixed / 100); break;
                            }

                            //lower layer
                            ZPlannerProject.GetLayerParameterValue(lNext, ZStringConstants.ParameterIDThickness, ref metal_thickness);
                            switch (tnext)
                            {
                                case ZLayerType.Signal: wNext = metal_thickness * (1 - ForSignal / 100); break;
                                case ZLayerType.Plane: wNext = metal_thickness * (1 - ForPlane / 100); break;
                                case ZLayerType.SplitMixed: wNext = metal_thickness * (1 - ForMixed / 100); break;
                            }

                            double delta = 0;
                            if (bUp)
                            {
                                bool bIgnoreUpper = bSeqLam && bUpperHalf;
                                if (!bIgnoreUpper) delta += wPrev;
                            }
                            if (bDown)
                            {
                                bool bIgnoreLower = bSeqLam && !bUpperHalf;
                                if (!bIgnoreLower)  delta += wNext;
                            }

                            // check for invalid values
                            double min_thickness = 0.2;
                            if (base_thicknes > delta + min_thickness)
                            {
                                new_thickness = base_thicknes - delta;
                            }
                            break;
                        case ZCopperCoverageType.ManuallyEntered:
                        case ZCopperCoverageType.RestoredOnImport:
                            // manually entered or calculated values
                            wPrev = 0;
                            wNext = 0;
                            metal_thickness = 0;
                            copperPercent = 0;

                            //upper layer
                            ZPlannerProject.GetLayerParameterValue(lPrev, ZStringConstants.ParameterIDThickness, ref metal_thickness);
                            bool bCopper = ZPlannerProject.GetLayerParameterValue(lPrev, ZStringConstants.ParameterIDCopperPercent, ref copperPercent);
                            if (bCopper && copperPercent > 0)
                            {
                                wPrev = metal_thickness * (1 - copperPercent / 100);
                            }
                            else
                            {
                                switch (tprev)
                                {
                                    case ZLayerType.Signal: wPrev = metal_thickness * (1 - ForSignal / 100); break;
                                    case ZLayerType.Plane: wPrev = metal_thickness * (1 - ForPlane / 100); break;
                                    case ZLayerType.SplitMixed: wPrev = metal_thickness * (1 - ForMixed / 100); break;
                                }
                            }

                            //lower layer
                            ZPlannerProject.GetLayerParameterValue(lNext, ZStringConstants.ParameterIDThickness, ref metal_thickness);
                            bCopper = ZPlannerProject.GetLayerParameterValue(lNext, ZStringConstants.ParameterIDCopperPercent, ref copperPercent);
                            if (bCopper && copperPercent > 0)
                            {
                                wNext = metal_thickness * (1 - copperPercent / 100);
                            }
                            else
                            {
                                switch (tnext)
                                {
                                    case ZLayerType.Signal: wNext = metal_thickness * (1 - ForSignal / 100); break;
                                    case ZLayerType.Plane: wNext = metal_thickness * (1 - ForPlane / 100); break;
                                    case ZLayerType.SplitMixed: wNext = metal_thickness * (1 - ForMixed / 100); break;
                                }
                            }

                            delta = 0;
                            if (bUp)
                            {
                                bool bIgnoreUpper = bSeqLam && bUpperHalf;
                                if (!bIgnoreUpper) delta += wPrev;
                            }
                            if (bDown)
                            {
                                bool bIgnoreLower = bSeqLam && !bUpperHalf;
                                if (!bIgnoreLower) delta += wNext;
                            }

                            // check for invalid values
                            min_thickness = 0.2;
                            if (base_thicknes > delta + min_thickness)
                            {
                                new_thickness = base_thicknes - delta;
                            }
                            break;
                    }
                }
            }

            return new_thickness;
        }

        //restore copper percent from prepreg pressed/unpressed thicknesses
        class layerData
        {
            public ZLayerType? type;
            public bool bMicrostripPrepreg;
            public double thickness, pressed_thickness;
            public bool bMetal;
            public bool bMaterialAssigned;

            public layerData(ZLayer z)
            {
                type = z.GetLayerType();
                bMetal = z.isMetal();
                bMicrostripPrepreg = false;
                bMaterialAssigned = z.IsMaterialAssigned;

                if (type == ZLayerType.Prepreg)
                {
                    bMicrostripPrepreg = z.Stackup.isMicrostripPrepreg(z);
                    ZPlannerProject.GetLayerParameterValue(z, ZStringConstants.ParameterIDOriginThickness, ref thickness);
                    ZPlannerProject.GetLayerParameterValue(z, ZStringConstants.ParameterIDFabricatorThickness, ref pressed_thickness);
                }
                else
                {
                    ZPlannerProject.GetLayerParameterValue(z, ZStringConstants.ParameterIDThickness, ref thickness);
                    pressed_thickness = thickness;
                }      
            }

            public void Combine(layerData other)
            {
                thickness += other.thickness;
                pressed_thickness += other.pressed_thickness;
            }

            public bool IsDifferentMetalType(layerData other)
            {
                if (bMetal && other.bMetal)
                {
                    if (type == ZLayerType.Plane && other.type != ZLayerType.Plane) return true;
                    if (type != ZLayerType.Plane && other.type == ZLayerType.Plane) return true;
                }
                return false;
            }
        }

        public bool RestoreCopperPercent()
        {
            double signal = ForSignal;
            double plane = ForPlane;

            List<layerData> layerList = new List<layerData>();
            foreach (ZLayer z in Layers){
                layerData lay = new layerData(z);
                int iLast = layerList.Count - 1;
                if (layerList.Count > 0 && lay.type == ZLayerType.Prepreg && layerList[iLast].type == ZLayerType.Prepreg)
                {
                    layerList[iLast].Combine(lay);
                }
                else
                {
                    layerList.Add(lay);
                }
            }

            //microstrip prepeg
            bool msPreg = false;
            bool bFoundSignal = true;
            bool bResult_Plane = false, bResult_Signal = false;
            for (int i = 0; i < layerList.Count / 2 ; i++) 
            {
                layerData ld = layerList[i], next = layerList[i + 1];
                ZLayerType? type = ld.type;
                if (type == ZLayerType.Prepreg && ld.bMicrostripPrepreg && ld.bMaterialAssigned) 
                {
                    if (next.bMetal)
                    {
                        msPreg = true;
                        double d = ld.thickness - ld.pressed_thickness;
                        double coef = (1 - d / next.thickness) * 100;
                        switch (next.type)
                        {
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                signal = coef;
                                bFoundSignal = true;
                                bResult_Signal = true;
                                break;
                            case ZLayerType.Plane:
                                plane = coef;
                                bFoundSignal = false;
                                bResult_Plane = true;
                                break;
                        }
                        break;
                    }
                }
            }
            if (msPreg)
            {
                //find second coefficient
                for (int i = 1; i < layerList.Count - 1; i++) //iterate from second layer to the next-to-last
                {
                    layerData ld = layerList[i], prev = layerList[i - 1], next = layerList[i + 1];
                    ZLayerType? type = ld.type;
                    if (type == ZLayerType.Prepreg && !ld.bMicrostripPrepreg && ld.bMaterialAssigned)
                    {
                        //check adjacent metals
                        if (prev.IsDifferentMetalType(next))
                        {
                            double d = ld.thickness - ld.pressed_thickness;
                            double p = prev.type == ZLayerType.Plane ? prev.thickness : next.thickness;
                            double s = prev.type == ZLayerType.Plane ? next.thickness : prev.thickness;
                            if (bFoundSignal)
                            {
                                double coef = (d - s * (1 - signal / 100)) / p;
                                plane = (1 - coef) * 100;
                                bResult_Plane = true;
                            }
                            else
                            {
                                double coef = (d - p * (1 - plane / 100)) / s;
                                signal = (1 - coef) * 100;
                                bResult_Signal = true;
                            }
                            break;
                        }
                    }
                }
                if (!bResult_Plane && !bResult_Signal)
                {//try artificial signal_copper/plane_copper ratio: 5/9
                    //...
                }
            }
            else //look for Sig-Preg-Plane combinations
            {
                double[] delta = new double[2];
                double[] p = new double[2];
                double[] s = new double[2];
                int n = 0;
                //find two Sig-Preg-Plane combinations
                for (int i = 1; i < layerList.Count - 1; i++) //iterate from second layer to the next-to-last
                {
                    layerData ld = layerList[i], prev = layerList[i - 1], next = layerList[i + 1];
                    if (ld.type == ZLayerType.Prepreg && ld.bMaterialAssigned)
                    {
                        //check adjacent metals
                        if (prev.IsDifferentMetalType(next))
                        {
                            delta[n] = ld.thickness - ld.pressed_thickness;
                            p[n] = prev.type == ZLayerType.Plane ? prev.thickness : next.thickness;
                            s[n] = prev.type == ZLayerType.Plane ? next.thickness : prev.thickness;

                            n++;
                            if (n == 2)
                            {
                                //solve for signal/plane layers copper precent
                                if (n == 2)
                                {
                                    try
                                    {
                                        double a = s[1] / s[0];
                                        double x = 0, y = 0;
                                        x = (delta[1] - a * delta[0]) / (p[1] - a * p[0]);
                                        y = (delta[0] - p[0] * x) / s[0];

                                        plane = (1 - x) * 100;
                                        signal = (1 - y) * 100;

                                        bool bInvalidPlane = Double.IsNaN(plane) || Double.IsInfinity(plane);
                                        bool bInvalidSignal = Double.IsNaN(signal) || Double.IsInfinity(signal);

                                        if (bInvalidPlane || bInvalidSignal)
                                        {
                                            n = 1;
                                            continue; //ill-defined system: use artificial ratio
                                        }

                                        bResult_Plane = bResult_Signal = true;
                                        break;
                                    }
                                    catch
                                    {
                                        n = 1;
                                        continue;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (bResult_Plane || bResult_Signal)
            {
                //double check the results
                if (plane > 0 && signal > 0) { 
                    //assign to layers
                    foreach (ZLayer zl in Layers)
                    {
                        switch (zl.GetLayerType())
                        {
                            case ZLayerType.Plane:
                                {
                                    ZLayerParameter lp = zl.GetLayerParameter(ZStringConstants.ParameterIDCopperPercent);
                                    if (!lp.IsEdited && bResult_Plane)
                                    {
                                        ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDCopperPercent, plane, true);
                                    }
                                }
                                break;
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                {
                                    ZLayerParameter lp = zl.GetLayerParameter(ZStringConstants.ParameterIDCopperPercent);
                                    if (!lp.IsEdited && bResult_Signal)
                                    {
                                        ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDCopperPercent, signal, true);
                                    }
                                }
                                break;
                        }
                    }
                }
            }

            if (bResult_Plane || bResult_Signal) CopperCoverageType = ZCopperCoverageType.RestoredOnImport;

            return bResult_Plane || bResult_Signal;
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

        public bool IsValid(out string message)
        {
            // check correct sequence of dielectric and metal layers
            // check required fields
            // check ranges

            message = "OK";
            ZLayerType? currType = null, prevType = null;
            bool currMetal = false, prevMetal = false;
            int currIdx = 0, prevIdx = 0;
            int numPlanes = 0;
            int nMetals = 0;
            ZLayer zPrevLayer = null;
            foreach (ZLayer zl in Layers)
            {
                currMetal = zl.isMetal();
                if (currMetal) nMetals++;
                currType = zl.GetLayerType();
                if (currType == ZLayerType.Plane)
                {
                    numPlanes++;
                }
                else if (currType == ZLayerType.SplitMixed)
                {
                    string s = "";
                    if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDPlaneReference, ref s))
                    {
                        if (s == ZStringConstants.PlaneReference)
                        {
                            numPlanes++;
                        }
                    }
                }

                currIdx = GetLayerOfStackupIndex(zl.ID);

                //solder mask - only outer layers
                if (currType == ZLayerType.SolderMask)
                {
                    if (currIdx != 0 && currIdx != Layers.Count - 1)
                    {
                        message = String.Format("Solder mask {0} is not the outer layer.", currIdx + 1);                        
                        return false;
                    }
                }

                if (prevType != null)
                {
                    //no adjacent metal layers
                    if (currMetal)
                    {
                        if (prevMetal)
                        {
                            message = String.Format("Two adjacent metal layers: {0} and {1}.", currIdx + 1, prevIdx + 1);
                            return false;
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
                                    if (!zl.isDummyCore())
                                    {
                                        message = String.Format("Dielectric layer {0} adjacent to Core {1}.", prevIdx + 1, currIdx + 1);
                                        return false;
                                    }
                                }
                                break;

                            case ZLayerType.Prepreg:
                                //any layer before prepreg except core
                                if (prevType == ZLayerType.Core)
                                {
                                    if (zPrevLayer != null && !zPrevLayer.isDummyCore())
                                    {
                                        message = String.Format("Prepreg layer {0} adjacent to Core {1}.", currIdx + 1, prevIdx + 1);
                                        return false;
                                    }
                                }
                                break;

                        }
                    }
                }

                // now check required fields for the current layer
                double dval = 0;
                if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref dval))
                {
                    //check range
                    if (!CheckRange(dval, 0.1, 50))
                    {
                        message = String.Format("Layer {0} {1} value must be in the range [{2}..{3}].", currIdx + 1, "thickness", 0.1, 50);
                        return false;
                    }
                }
                else
                {
                    message = String.Format("Layer {0} must have {1} value defined", currIdx + 1, "thickness");
                    return false;
                }

                if (currMetal)
                {
                    foreach (var single in Singles)
                    {
                        ZLayer zsl = GetLayerOfSingleImpedance(zl.ID, single.ID);
                        if (zsl == null) continue;
                        
                        if (ZPlannerProject.GetLayerParameterValue(zsl, ZStringConstants.ParameterIDZo_TraceWidth, ref dval))
                        {
                            //check range
                            if (!CheckRange(dval, 2, 100))
                            {
                                message = String.Format("Layer {0} {1} value must be in the range [{2}..{3}].", currIdx + 1, "trace width", 2, 100);
                                return false;
                            }
                        }
                    }

                    foreach (var pair in Pairs)
                    {
                        ZLayer zpl = GetLayerOfPairImpedance(zl.ID, pair.ID);
                        if (zpl == null) continue;

                        if (ZPlannerProject.GetLayerParameterValue(zpl, ZStringConstants.ParameterIDZdiff_TraceWidth, ref dval))
                        {
                            //check range
                            if (!CheckRange(dval, 2, 100))
                            {
                                message = String.Format("Layer {0} {1} value must be in the range [{2}..{3}].", currIdx + 1, "diff trace width", 2, 100);
                                return false;
                            }
                        }

                        if (ZPlannerProject.GetLayerParameterValue(zpl, ZStringConstants.ParameterIDZdiff_TraceSpacing, ref dval))
                        {
                            //check range
                            if (!CheckRange(dval, 2, 100))
                            {
                                message = String.Format("Layer {0} {1} value must be in the range [{2}..{3}].", currIdx + 1, "diff trace spacing", 2, 100);
                                return false;
                            }
                        }
                    }
                }
                else
                {
                    foreach (var single in Singles)
                    {
                        ZLayer zsl = GetLayerOfSingleImpedance(zl.ID, single.ID);
                        if (zsl == null) continue;
                        
                        string str = zsl.GetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant);
                        //if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_DielectricConstant, ref dval))
                        if (!string.IsNullOrEmpty(str))
                        {
                            //check range
                            if (!CheckFunctionRange(str, 1, 7))
                            {
                                message = String.Format("Layer {0} {1} value must be in the range [{2}..{3}].", currIdx + 1, "dielectric constant", 1, 7);
                                return false;
                            }
                        }
                        else
                        {
                            message = String.Format("Layer {0} must have {1} value defined", currIdx + 1, "dielectric constant");
                            return false;
                        }

                        str = zsl.GetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent);
                        //if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_LossTangent, ref dval))
                        if (!string.IsNullOrEmpty(str))
                        {
                            //check range
                            if (!CheckFunctionRange(str, 0.0001, 0.05))
                            {
                                message = String.Format("Layer {0} {1} value must be in the range [{2}..{3}].", currIdx + 1, "loss tangent", 0.0001, 0.05);
                                return false;
                            }
                        }
                        else
                        {
                            message = String.Format("Layer {0} must have {1} value defined", currIdx + 1, "loss tangent");
                            return false;
                        }
                    }

                    foreach (var pair in Pairs)
                    {
                        ZLayer zpl = GetLayerOfPairImpedance(zl.ID, pair.ID);
                        if (zpl == null) continue;

                        string str = zpl.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_DielectricConstant);
                        //if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZdiff_DielectricConstant, ref dval))
                        if (!string.IsNullOrEmpty(str))
                        {
                            //check range
                            if (!CheckFunctionRange(str, 1, 7))
                            {
                                message = String.Format("Layer {0} {1} value must be in the range [{2}..{3}].", currIdx + 1, "diff dielectric constant", 1, 7);
                                return false;
                            }
                        }
                        else
                        {
                            message = String.Format("Layer {0} must have {1} value defined", currIdx + 1, "diff dielectric constant");
                            return false;
                        }

                        str = zpl.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_LossTangent);
                        //if (ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_LossTangent, ref dval))
                        if (!string.IsNullOrEmpty(str))
                        {
                            //check range
                            if (!CheckFunctionRange(str, 0.0001, 0.05))
                            {
                                message = String.Format("Layer {0} {1} value must be in the range [{2}..{3}].", currIdx + 1, "diff loss tangent", 0.0001, 0.05);
                                return false;
                            }
                        }
                        else
                        {
                            message = String.Format("Layer {0} must have {1} value defined", currIdx + 1, "diff loss tangent");
                            return false;
                        }
                    }
                }

                prevType = currType;
                prevMetal = currMetal;   
                prevIdx = currIdx;
                zPrevLayer = zl;
            }

            if (numPlanes == 0) //&& nMetals > 2)
            {
                message = "Stackup must have at least one plane.";
                return false;
            }
            return true;
        }
        //---------------------------------------------------------------------------------------------------

        public int SequentialLamination()
        {
            int retval = 0; //no sequential lamination
            //look for sequential lamination structure(s)
            int nLayers = Layers.Count;
            if (nLayers <= 0) return 0;

            int middle = nLayers / 2 + 1;//1-based

            int count = Layers.Count;
            int iMetal = 0;

            bool bCore = false; //core layer is the reason search was stopped
            bool bSymmetry = false;  //symmetry is the reason search was stopped
            for (int i = 1; i <= nLayers / 2; i++)
            {
                ZLayer zlUp = Layers[i - 1];
                ZLayer zlDown = Layers[count - i]; 
                ZLayerType? tUp = zlUp.GetLayerType();
                ZLayerType? tDown = zlDown.GetLayerType();
                if (tUp != tDown)
                {
                    bSymmetry = true;
                    break; 
                }

                if (tUp == ZLayerType.Core)
                {
                    bCore = true;
                    break;
                }

                double hUp = 0, hDown = 0;
                ZPlannerProject.GetLayerParameterValue(zlUp, ZStringConstants.ParameterIDThickness, ref hUp);
                ZPlannerProject.GetLayerParameterValue(zlDown, ZStringConstants.ParameterIDThickness, ref hDown);
                double eps = 0.0001;
                if (Math.Abs(hUp - hDown) > eps)
                {
                    bSymmetry = true;
                    break;
                }

                if (zlUp.isMetal())
                {
                    iMetal++;
                }

            }

            if (bSymmetry)
            {
                if (iMetal > 0) return iMetal-1; else return 0;
            }

            //check if Core is the center of stackup symmetry
            if (!bCore)
            {
                if (Layers[middle - 1].GetLayerType() == ZLayerType.Core)
                {
                    bCore = true;
                }
            }

            if (bCore && iMetal > 0)
            {
                iMetal--;
            }
            retval = iMetal;

            return retval;
        }
    }

    class SelectList : ZEntity
    {
        public SelectList(string id, string[] arr)
        {
            Values = new List<string>(arr);
            Dict = new Dictionary<string, string>();
            foreach (string item in arr)
            {
                Regex slashPattern = new Regex(@"^(?<slashKey>/[0123456789]+)\s+-\s+(?<slashValue>.+)$", RegexOptions.Compiled | RegexOptions.Singleline);
                Match slashMatch = slashPattern.Match(item);
                if (!slashMatch.Success) continue;
                string slashKey = slashMatch.Groups["slashKey"].Value;
                string slashValue = slashMatch.Groups["slashValue"].Value.Trim();

                Dict.Add(slashKey, slashKey + " - " + slashValue);
            }
            ID = id;
        }

        public SelectList(string id)
        {
            Values = new List<string>();
            Dict = new Dictionary<string, string>();
            ID = id;
        }

        private List<string> Values { get; set; }
        private Dictionary<string, string> Dict { get; set; }

        public string AddValue(string value)
        {
            if (this.ID == ZStringConstants.ListIDCopperThickness)
            {
                double dValue, dV;

                if (double.TryParse(value, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                {
                    foreach (string item in this.Values)
                    {
                        if (double.TryParse(item, NumberStyles.Any, CultureInfo.InvariantCulture, out dV) && dV == dValue) return item;
                    }
                }
                else
                {
                    if (this.Values.Contains(value)) return value;
                }
            }

            Values.Add(value);

            if (value.StartsWith("/"))
            {
                Regex slashPattern = new Regex(@"^(?<slashKey>/[0123456789]+)\s+-\s+(?<slashValue>.+)$", RegexOptions.Compiled | RegexOptions.Singleline);
                Match slashMatch = slashPattern.Match(value);
                if (!slashMatch.Success) return string.Empty;
                string slashKey = slashMatch.Groups["slashKey"].Value;
                string slashValue = slashMatch.Groups["slashValue"].Value.Trim();

                Dict.Add(slashKey, slashKey + " - " + slashValue);
            }

            return value;
        }

        public List<string> GetValues()
        {
            return Values;
        }

        public Dictionary<string, string> GetDictionary()
        {
            return Dict;
        }
    }
}
