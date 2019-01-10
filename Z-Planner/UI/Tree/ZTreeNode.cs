using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Tree
{
    class ZTreeNode : TreeNode
    {
        private string ID;
        public ZTreeNode(string id, string text) : base(text + "     ")
        { ID = id; }

        internal static void FillProject(TreeView treeView, ZPlannerProject project)
        {
            if (project == null || project.Stackup == null) return;

            ZTreeNode stackupNode = new ZTreeNode(project.Stackup.ID, project.Stackup.Title);
            treeView.Nodes.Add(stackupNode);
            project.Stackup.PropertyChanged += stackupNode.ZTitle_PropertyChanged;
            stackupNode.Tag = project.Stackup;
            stackupNode.NodeFont = new Font(treeView.Font, FontStyle.Bold);

            stackupNode.FillStackup(project.Stackup);

            stackupNode.Expand();
        }

        private void FillStackup(ZStackup stackup)
        {
            if (stackup == null) return;
            
            ZTreeNode layersNode = new ZTreeNode("Layers", "Rows  ");
            this.Nodes.Add(layersNode);
            stackup.Layers.CollectionChanged += layersNode.Layers_CollectionChanged;
            layersNode.Tag = stackup.Layers;
            layersNode.NodeFont = new Font(ZPlannerManager.ProjectPanel.TreeView.Font, FontStyle.Bold);
            layersNode.FillLayers(stackup.Layers);
            layersNode.Collapse();

            ZTreeNode singlesNode = new ZTreeNode("Singles", "Single-Ended Signals  ");
            this.Nodes.Add(singlesNode); 
            stackup.Singles.CollectionChanged += singlesNode.Singles_CollectionChanged;
            singlesNode.Tag = stackup.Singles;
            singlesNode.NodeFont = new Font(ZPlannerManager.ProjectPanel.TreeView.Font, FontStyle.Bold);
            singlesNode.FillSingles(stackup.Singles);
            singlesNode.Collapse();

            ZTreeNode pairsNode = new ZTreeNode("Pairs", "Differential Signals  ");
            this.Nodes.Add(pairsNode);
            stackup.Pairs.CollectionChanged += pairsNode.Pairs_CollectionChanged;
            pairsNode.Tag = stackup.Pairs;
            pairsNode.NodeFont = new Font(ZPlannerManager.ProjectPanel.TreeView.Font, FontStyle.Bold);
            pairsNode.FillPairs(stackup.Pairs);
            pairsNode.Collapse();

            ZTreeNode spansNode = new ZTreeNode("Spans", "Via Spans  ");
            this.Nodes.Add(spansNode);
            stackup.Spans.CollectionChanged += spansNode.Spans_CollectionChanged;
            spansNode.Tag = stackup.Spans;
            spansNode.NodeFont = new Font(ZPlannerManager.ProjectPanel.TreeView.Font, FontStyle.Bold);
                
            spansNode.FillSpans(stackup.Spans);
                
            spansNode.Collapse();
            this.Expand();
        }

        private void FillLayers(ZList<ZLayer> layers)
        {
            if (layers == null) return;

            foreach (var layer in layers)
            {
                ZTreeNode layerNode = new ZTreeNode(layer.ID, layer.Title);
                this.Nodes.Add(layerNode);
                layer.PropertyChanged += layerNode.ZTitle_PropertyChanged;
                layerNode.Tag = layer;
                ZLayerParameter layerTypeParameter = layer.LayerParameters.Find((ZLayerParameter x) => x.ID == "LayerType");
                if (layerTypeParameter != null)
                {
                    layerNode.BackColor = ZColorConstants.GetLayerColor(layerTypeParameter.Value);
                    layerTypeParameter.PropertyChanged += layerNode.ZLayerParameter_PropertyChanged;
                }
            }
        }

        private void FillSingles(ZList<ZSingle> singles)
        {
            if (singles == null) return;

            foreach (var single in singles)
            {
                ZTreeNode singleNode = new ZTreeNode(single.ID, single.Title);
                this.Nodes.Add(singleNode);
                single.PropertyChanged += singleNode.ZTitle_PropertyChanged;
                singleNode.Tag = single;
            }
        }

        private void FillPairs(ZList<ZPair> pairs)
        {
            if (pairs == null) return;

            foreach (var pair in pairs)
            {
                ZTreeNode pairNode = new ZTreeNode(pair.ID, pair.Title);
                this.Nodes.Add(pairNode);
                pair.PropertyChanged += pairNode.ZTitle_PropertyChanged;
                pairNode.Tag = pair;
            }
        }

        private void FillSpans(ZList<ZSpan> spans)
        {
            if (spans == null) return;

            foreach (var span in spans)
            {
                ZTreeNode spanNode = new ZTreeNode(span.ID, span.Title);
                this.Nodes.Add(spanNode);
                span.PropertyChanged += spanNode.ZTitle_PropertyChanged;
                spanNode.Tag = span;
            }
        }

        public void Layers_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            this.ClearAllNodes();
            this.Nodes.Clear();
            this.FillLayers(sender as ZList<ZLayer>);
        }

        public void Singles_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            this.ClearAllNodes();
            this.Nodes.Clear();
            this.FillSingles(sender as ZList<ZSingle>);
        }

        public void Pairs_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            this.ClearAllNodes();
            this.Nodes.Clear();
            this.FillPairs(sender as ZList<ZPair>);
        }

        public void Spans_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            this.ClearAllNodes();
            this.Nodes.Clear();
            this.FillSpans(sender as ZList<ZSpan>);
        }

        public void ZTitle_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            ZEntity component = sender as ZEntity;

            if (component == null) return;

            switch (e.PropertyName)
            {
                case "Title":
                    this.Text = component.Title;
                    break;
                default:
                    break;
            }
        }

        public void ZLayerParameter_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            ZLayerParameter layerParameter = sender as ZLayerParameter;

            if (layerParameter == null) return;
            if (layerParameter.ID == "LayerType") this.BackColor = ZColorConstants.GetLayerColor(layerParameter.Value);
        }

        internal void Clear()
        {
            ZEntity entity = this.Tag as ZEntity;
            if (entity != null) entity.PropertyChanged -= ZTitle_PropertyChanged;
            /*
            ZList<ZStackup> stackups = this.Tag as ZList<ZStackup>;
            if(stackups != null) stackups.CollectionChanged -= Stackups_CollectionChanged;

            ZList<ZLayer> layers = this.Tag as ZList<ZLayer>;
            if (layers != null) layers.CollectionChanged -= Layers_CollectionChanged;

            ZList<ZImpedance> pairs = this.Tag as ZList<ZImpedance>;
            if (pairs != null) pairs.CollectionChanged -= Pairs_CollectionChanged;

            ZList<ZSpan> spans = this.Tag as ZList<ZSpan>;
            if (spans != null) spans.CollectionChanged -= Spans_CollectionChanged;
            */
            ZLayer layer = this.Tag as ZLayer;
            if (layer != null)
            {
                ZLayerParameter layerTypeParameter = layer.LayerParameters.Find((ZLayerParameter x) => x.ID == "LayerType");
                if (layerTypeParameter != null)
                {
                    layerTypeParameter.PropertyChanged -= ZLayerParameter_PropertyChanged;
                }
            }
        }

        internal void ClearAllNodes()
        {
            this.Clear();

            foreach (TreeNode childNode in this.Nodes)
            {
                ((ZTreeNode)childNode).ClearAllNodes();
            }
        }

        internal ZTreeNode GetChildByID(string id)
        {
            foreach (ZTreeNode node in this.Nodes)
            {
                /*ZEntity entity = node.Tag as ZEntity;
                if (entity != null && entity.ID == id) return node;*/
                if (node.ID == id) return node;
            }

            return null;
        }
    }
}
