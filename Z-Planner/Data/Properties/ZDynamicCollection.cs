using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Data.Properties
{
    public class ZDynamicCollection
    {
        public IList Collection { get; set; }
        public Type ElementType { get; set; }
        public string Title { get; set; }
        public string Description { get; set; }
        public ZPlannerProject Project { get; set; }
        internal ZStackup Stackup { get; set; }


        internal ZDynamicCollection(ZPlannerProject project, ZStackup stackup, IList collection, Type elementType, string title, string description)
        {
            this.Project = project;
            this.Stackup = stackup;
            this.Collection = collection;
            this.ElementType = elementType;
            this.Title = title;
            this.Description = description;
        }
    }
}
