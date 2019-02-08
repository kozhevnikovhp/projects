using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;
using ZZero.ZPlanner.ZConfiguration;
using System.IO;

namespace ZZero.ZPlanner.UI.Help
{
    public partial class HelpTopicsPanel : UserControl
    {
        public HelpTopicsPanel()
        {
            InitializeComponent();
        }

        public void FillData()
        {
            XElement root = XElement.Load(ZSettings.TutorialTopicsFile);

            var query =
            from item in root.Descendants("topic")
            select new
            {
                Id = item.Attribute("id").Value,
                Title = item.Attribute("title").Value,
                Level = item.Attribute("level").Value,
                ParentId = item.Parent.Attribute("id").Value
            };

            var foolList = query.ToList();
            var zeroLevelList = foolList.FindAll(x => x.Level == "0");
            var firstLevelList = foolList.FindAll(x => x.Level == "1");
            var secondLevelList = foolList.FindAll(x => x.Level == "2");
            var thirdLevelList = foolList.FindAll(x => x.Level == "3");

            foreach (var item0 in zeroLevelList)
            {
                TreeNode node0 = AddNode(tutorialTopicsTree.Nodes, item0.Title, item0.Id);

                foreach (var item1 in firstLevelList)
                {
                    TreeNode node1 = AddNode(node0.Nodes, item1.Title, item1.Id);

                    foreach (var item2 in secondLevelList.FindAll(x => x.ParentId == item1.Id))
                    {
                        TreeNode node2 = AddNode(node1.Nodes, item2.Title, item2.Id);

                        foreach (var item3 in thirdLevelList.FindAll(x => x.ParentId == item2.Id))
                        {
                            TreeNode node3 = AddNode(node2.Nodes, item3.Title, item3.Id);
                            SetNodeImage(node3);
                        }

                        SetNodeImage(node2);
                    }

                    SetNodeImage(node1);
                }

                SetNodeImage(node0);
            }

            tutorialTopicsTree.ExpandAll();
        }

        private TreeNode AddNode(TreeNodeCollection nodes, string title, string id)
        {
            TreeNode node = new TreeNode();
            node.Text = title;
            node.Tag = id;
            nodes.Add(node);

            return node;
        }

        private void SetNodeImage(TreeNode node)
        {
            if (node.Nodes != null && (node.Nodes.Count > 0 || node.Level <= 1))
            {
                node.ImageKey = "Collapsed";
                node.SelectedImageKey = "Collapsed";
            }
            else
            {
                node.ImageKey = "Leaf";
                node.SelectedImageKey = "Leaf";
            }
        }

        private void tutorialTopicsTree_AfterCollapse(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Nodes != null && (e.Node.Nodes.Count >= 0 || e.Node.Level == 0))
            {
                e.Node.ImageKey = "Collapsed";
                e.Node.SelectedImageKey = "Collapsed";
            }
        }

        private void tutorialTopicsTree_AfterExpand(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Nodes != null && e.Node.Nodes.Count >= 0)
            {
                e.Node.ImageKey = "Expanded";
                e.Node.SelectedImageKey = "Expanded";
            }
        }

        private void tutorialTopicsTree_AfterSelect(object sender, TreeViewEventArgs e)
        {
            ZPlannerManager.OpenTutorial(e.Node.Tag as string);
        }
    }
}
