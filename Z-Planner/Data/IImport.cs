using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZZero.ZPlanner.Data
{
    public interface IImport
    {
        bool Convert();

        bool IsValidFile();

        bool Import();
    }
}
