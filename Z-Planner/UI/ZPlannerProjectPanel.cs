using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Data.Properties;
using ZZero.ZPlanner.UI.Tree;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI
{
    public partial class ZPlannerProjectPanel : DockContent
    {
        private ZTreeNode DragOverNode;
        private bool IsDragOverNodeBefore;
        private ZPlannerProject project;

        public ZPlannerProjectPanel()
        {
            InitializeComponent();

            this.VisibleChanged += ZPlannerManager.Panel_VisibleChanged;
        }

        public TreeView TreeView
        { get { return this.stackupsTreeView; } }

        public bool IsIgnoreRowSelect { get; set; }

        internal void Clear()
        {
            foreach (TreeNode node in this.stackupsTreeView.Nodes)
            {
                ((ZTreeNode)node).ClearAllNodes();
            }

            this.stackupsTreeView.ItemDrag -= stackupsTreeView_ItemDrag;
            this.stackupsTreeView.DragEnter -= stackupsTreeView_DragEnter;
            this.stackupsTreeView.DragDrop -= stackupsTreeView_DragDrop;
            this.stackupsTreeView.DragOver -= stackupsTreeView_DragOver;
            this.stackupsTreeView.DragLeave -= stackupsTreeView_DragLeave;
            this.stackupsTreeView.AfterSelect -= stackupsTreeView_AfterSelect;

            this.stackupsTreeView.Nodes.Clear();
            DragOverNode = null;
            project = null;
        }

        /// <summary>
        /// Loads form's data.
        /// </summary>
        internal void LoadData(ZPlannerProject project)
        {
            this.project = project;
            FillTreeView();
        }

        private void FillTreeView()
        {
            ZTreeNode.FillProject(this.stackupsTreeView, project);

            this.stackupsTreeView.AllowDrop = true;
            //this.stackupsTreeView.DrawMode = TreeViewDrawMode.Normal;
            //this.stackupsTreeView.DrawNode += stackupsTreeView_DrawNode;
            this.stackupsTreeView.ItemDrag += stackupsTreeView_ItemDrag;
            this.stackupsTreeView.DragEnter += stackupsTreeView_DragEnter;
            this.stackupsTreeView.DragDrop += stackupsTreeView_DragDrop;
            this.stackupsTreeView.DragOver += stackupsTreeView_DragOver;
            this.stackupsTreeView.DragLeave += stackupsTreeView_DragLeave;
            this.stackupsTreeView.AfterSelect += stackupsTreeView_AfterSelect;
            //this.stackupsTreeView.ExpandAll();

            if (project != null && project.Stackup != null && ZPlannerManager.StackupPanel != null && project.Stackup == ZPlannerManager.StackupPanel.Stackup) SelectStackup();
        }

        void stackupsTreeView_Click(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        void stackupsTreeView_DrawNode(object sender, DrawTreeNodeEventArgs e)
        {
            if (e.Node == DragOverNode)
            {
                Rectangle rect;
                if (IsDragOverNodeBefore)
                {
                    Point location = ((TreeView)sender).PointToScreen(DragOverNode.Bounds.Location);
                    rect = new Rectangle(location.X, location.Y - 1, DragOverNode.Bounds.Width, 2);
                }
                else
                {
                    Point location = ((TreeView)sender).PointToScreen(DragOverNode.Bounds.Location);
                    rect = new Rectangle(location.X, location.Y + DragOverNode.Bounds.Height - 1, DragOverNode.Bounds.Width, 2);
                }

                e.Graphics.DrawRectangle(new Pen(Color.Black), rect);
            }
        }

        void stackupsTreeView_DragLeave(object sender, EventArgs e)
        {
            DragOverNode = null;
        }

        void stackupsTreeView_DragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("System.Windows.Forms.ZTreeNode", false))
            {
                Point pt = ((TreeView)sender).PointToClient(new Point(e.X, e.Y));
                ZTreeNode DestinationNode = (ZTreeNode)((TreeView)sender).GetNodeAt(pt);
                ZTreeNode SourceNode = (ZTreeNode)e.Data.GetData("System.Windows.Forms.ZTreeNode");
                bool isBeforeNode = (pt.Y - DestinationNode.Bounds.Top) < (DestinationNode.Bounds.Bottom - pt.Y);

                DragOverNode = DestinationNode;
                IsDragOverNodeBefore = isBeforeNode;

                /*Graphics g = ((TreeView)sender).CreateGraphics();
                Rectangle rect;
                if (isBeforeNode)
                {
                    Point location = ((TreeView)sender).PointToScreen(DestinationNode.Bounds.Location);
                    rect = new Rectangle(location.X, location.Y - 1, DestinationNode.Bounds.Width, 2);
                }
                else
                {
                    Point location = ((TreeView)sender).PointToScreen(DestinationNode.Bounds.Location);
                    rect = new Rectangle(location.X, location.Y + DestinationNode.Bounds.Height - 1, DestinationNode.Bounds.Width, 2);
                }
                
                g.DrawRectangle(new Pen(Color.Black), rect);
                g.Dispose();*/
            }
        }

        void stackupsTreeView_DragDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("System.Windows.Forms.ZTreeNode", false))
            {
                Point pt = ((TreeView)sender).PointToClient(new Point(e.X, e.Y));
                ZTreeNode DestinationNode = (ZTreeNode)((TreeView)sender).GetNodeAt(pt);
                ZTreeNode SourceNode = (ZTreeNode)e.Data.GetData("System.Windows.Forms.ZTreeNode");
                bool isBeforeNode = (pt.Y - DestinationNode.Bounds.Top) < (DestinationNode.Bounds.Bottom - pt.Y);
                if (DestinationNode.Parent == SourceNode.Parent)
                {
                    ZTreeNode parentNode = (ZTreeNode)SourceNode.Parent;
                    SourceNode.Remove();
                    int destinationIndex = (isBeforeNode) ? DestinationNode.Index : DestinationNode.Index + 1;
                    parentNode.Nodes.Insert(destinationIndex, SourceNode);
                }
            }
            DragOverNode = null;
        }

        void stackupsTreeView_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("System.Windows.Forms.ZTreeNode", false))
            {
                Point pt = ((TreeView)sender).PointToClient(new Point(e.X, e.Y));
                ZTreeNode DestinationNode = (ZTreeNode)((TreeView)sender).GetNodeAt(pt);
                ZTreeNode SourceNode = (ZTreeNode)e.Data.GetData("System.Windows.Forms.ZTreeNode");

                if (DestinationNode.Parent == SourceNode.Parent)
                {
                    e.Effect = DragDropEffects.Move;
                }
                else
                {
                    e.Effect = DragDropEffects.None;
                }
            }
        }

        void stackupsTreeView_ItemDrag(object sender, ItemDragEventArgs e)
        {
            DoDragDrop(e.Item, DragDropEffects.Move);
        }

        void stackupsTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Tag != null)
            {
                ZStackup stackup = e.Node.Tag as ZStackup;
                if (stackup != null && ZPlannerManager.StackupPanel != null)
                {
                    ZPlannerManager.PropertiesPanel.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(stackup);
                    //ZPlannerManager.ShowStackupPanel();
                    ZPlannerManager.StackupPanel.Focus();
                    //ZPlannerManager.StackupPanel.ClearPair();
                 }

                ZLayer layer = e.Node.Tag as ZLayer;
                if (layer != null) 
                {
                    ZPlannerManager.PropertiesPanel.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(layer);
                    ZPlannerStackupPanel stackupPanel = ZPlannerManager.StackupPanel;
                    //ZPlannerManager.ShowStackupPanel();
                    stackupPanel.Focus();
                    //stackupPanel.ClearPair();
                    if (!IsIgnoreRowSelect) stackupPanel.SelectRow(layer.ID);
                }

                ZSingle single = e.Node.Tag as ZSingle;
                if (single != null) 
                {
                    ZPlannerManager.PropertiesPanel.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(single);
                    ZPlannerStackupPanel stackupPanel = ZPlannerManager.StackupPanel;
                    //ZPlannerManager.ShowStackupPanel();
                    stackupPanel.Focus();
                    //stackupPanel.LoadSingle(single);
                    stackupPanel.Stackup.ActiveSingle = single;
                    //stackupPanel.SetReadOnlyCells();
                    //stackupPanel.FormatGridView();
                    //stackupPanel.ClearRowSelection();
                }

                ZPair pair = e.Node.Tag as ZPair;
                if (pair != null)
                {
                    ZPlannerManager.PropertiesPanel.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(pair);
                    ZPlannerStackupPanel stackupPanel = ZPlannerManager.StackupPanel;
                    //ZPlannerManager.ShowStackupPanel();
                    stackupPanel.Focus();
                    //stackupPanel.LoadPair(pair);
                    stackupPanel.Stackup.ActivePair = pair;
                    //stackupPanel.SetReadOnlyCells();
                    //stackupPanel.FormatGridView();
                    //stackupPanel.ClearRowSelection();
                }

                ZSpan span = e.Node.Tag as ZSpan;
                if (span != null)
                {
                    ZPlannerManager.PropertiesPanel.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(span);
                    ZPlannerStackupPanel stackupPanel = ZPlannerManager.StackupPanel;
                    //ZPlannerManager.ShowStackupPanel();
                    stackupPanel.Focus();
                    //stackupPanel.ClearPair();
                    //stackupPanel.ClearRowSelection();
                }

                IList collection = e.Node.Tag as IList;
                if (collection != null)
                {
                    ZPlannerManager.PropertiesPanel.CollectionObject = ZDynamicComponentManager.GetDynamicCollection(project, collection);
                    ZPlannerStackupPanel stackupPanel = ZPlannerManager.StackupPanel;
                    //ZPlannerManager.ShowStackupPanel();
                    stackupPanel.Focus();
                    //stackupPanel.ClearPair();
                    //stackupPanel.ClearRowSelection();
                }

                if (ZPlannerManager.StackupPanel != null && ZPlannerManager.StackupPanel.Stackup != null)
                {
                    string singleID = ZPlannerManager.StackupPanel.Single != null ? ZPlannerManager.StackupPanel.Single.ID : "";
                    ZPlannerManager.MainMenu.UpdateSingles(ZPlannerManager.StackupPanel.Stackup.Singles, singleID);
                    string pairID = ZPlannerManager.StackupPanel.Pair != null ? ZPlannerManager.StackupPanel.Pair.ID : "";
                    ZPlannerManager.MainMenu.UpdatePairs(ZPlannerManager.StackupPanel.Stackup.Pairs, pairID); 
                }
            }
        }

        internal void SelectStackup()
        {
            if (ZPlannerManager.StackupPanel == null) return;
            ZTreeNode stackupNode = stackupsTreeView.Nodes[0] as ZTreeNode;
            if (stackupNode == null) return;

            if (stackupsTreeView.SelectedNode != stackupNode) stackupsTreeView.SelectedNode = stackupNode;
            stackupNode.Collapse(false);

            stackupNode.Expand();
            stackupsTreeView.Focus();
        }

        internal void SelectLayer(string layerID)
        {
            ZTreeNode stackupNode = stackupsTreeView.Nodes[0] as ZTreeNode;
            if (stackupNode == null) return;

            stackupNode.Collapse(false);

            ZTreeNode layersNode = stackupNode.GetChildByID("Layers");
            if (layersNode == null) return;
            
            layersNode.Expand();

            ZTreeNode layerNode = layersNode.GetChildByID(layerID);
            if (layerNode == null) return;

            if (stackupsTreeView.SelectedNode != layerNode)
            {
                IsIgnoreRowSelect = true;
                stackupsTreeView.SelectedNode = layerNode;
                IsIgnoreRowSelect = false;
            }
            stackupsTreeView.Focus();
        }

        internal void SelectFirstLayer()
        {
            ZTreeNode stackupNode = stackupsTreeView.Nodes[0] as ZTreeNode;
            if (stackupNode == null) return; 
            
            stackupNode.Collapse(false);

            ZTreeNode layersNode = stackupNode.GetChildByID("Layers");
            if (layersNode == null) return;

            layersNode.Expand();

            ZTreeNode layerNode = layersNode.FirstNode as ZTreeNode;
            if (layerNode == null) return;

            if (stackupsTreeView.SelectedNode != layerNode)
            {
                stackupsTreeView.SelectedNode = layerNode;
            }
            stackupsTreeView.Focus();
        }

        internal void SelectSingle(string singleID)
        {
            //ZPlannerManager.ShowProjectView();

            ZTreeNode stackupNode = stackupsTreeView.Nodes[0] as ZTreeNode;
            if (stackupNode == null) return;

            stackupNode.Collapse(false);

            ZTreeNode singlesNode = stackupNode.GetChildByID("Singles");
            if (singlesNode == null) return;

            singlesNode.Expand();

            ZTreeNode singleNode = singlesNode.GetChildByID(singleID);
            if (singleNode == null) return;

            if (stackupsTreeView.SelectedNode != singleNode) stackupsTreeView.SelectedNode = singleNode;
            stackupsTreeView.Focus();
        }

        internal void SelectPair(string pairID)
        {
            //ZPlannerManager.ShowProjectView();

            ZTreeNode stackupNode = stackupsTreeView.Nodes[0] as ZTreeNode;
            if (stackupNode == null) return;

            stackupNode.Collapse(false);

            ZTreeNode pairsNode = stackupNode.GetChildByID("Pairs");
            if (pairsNode == null) return;

            pairsNode.Expand();

            ZTreeNode pairNode = pairsNode.GetChildByID(pairID);
            if (pairNode == null) return;

            if (stackupsTreeView.SelectedNode != pairNode) stackupsTreeView.SelectedNode = pairNode;
            stackupsTreeView.Focus();
        }

        internal void SelectSpan(string spanID)
        {
            ZTreeNode stackupNode = stackupsTreeView.Nodes[0] as ZTreeNode;
            if (stackupNode == null) return; 
            
            stackupNode.Collapse(false);

            ZTreeNode spansNode = stackupNode.GetChildByID("Spans");
            if (spansNode == null) return;

            spansNode.Expand();

            ZTreeNode spanNode = spansNode.GetChildByID(spanID);
            if (spanNode == null) return;

            if (stackupsTreeView.SelectedNode != spanNode) stackupsTreeView.SelectedNode = spanNode;
            stackupsTreeView.Focus();
        }
    }
}
