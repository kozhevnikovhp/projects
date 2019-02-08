using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Data
{
    internal class ImportWizardData
    {
        public string FilePath { get; set; }

        public ZLibraryCategory DmlType { get; set; }

        public string MaterialName { get; set; }

        public List<string> ManufacturerList { get; set; }

        public string Manufacturer { get; set; }

        public string Material { get; set; }

        public List<string> MaterialList { get; set; }

        public int SequentialX { get; set; }

        public int SequentialN { get; set; }

        public bool SequentialLaminationEnabled { get; set; }

        public Dictionary<string, double> CopperThicknessDictionary { get; set; }
    }
}
