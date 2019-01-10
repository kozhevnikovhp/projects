using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.Data.Provider
{
    /// <summary>
    /// Acts as an interface for all data providers.
    /// </summary>
    public abstract class DataProvider
    {
        private static DataProvider provider = null;

        /// <summary>
        /// Gets an instance according to application configuration.
        /// </summary>
        public static DataProvider Instance 
        {
            get
            {
                if (provider == null)
                {
                    switch (ZSettings.SourceType)
                    {
                        case "XML":
                            provider = new XMLDataProvider();
                            break;
                        default:
                            throw new InvalidOperationException("Incorrect SourceType settings was set in App.config");
                    }
                }

                return provider;
            } 
        }

        /// <summary>
        /// Provides DML data source.
        /// </summary>
        internal abstract ZMaterialLibrary OpenZMaterialLibrary(string arg, ZMaterialLibrary z0Library = null);

        /// <summary>
        /// Open ZPlanner project data source.
        /// </summary>
        internal abstract ZPlannerProject OpenZPlannerProject(string arg);

        /// <summary>
        /// Save ZPlanner project data source.
        /// </summary>
        internal abstract void SaveZPlannerProject(string arg, ZPlannerProject project);
    }
}
