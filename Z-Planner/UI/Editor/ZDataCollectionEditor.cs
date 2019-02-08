using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Collections;
using ZZero.ZPlanner.Data.Entities;
using System.Globalization;
using ZZero.ZPlanner.Data.Properties;

namespace ZZero.ZPlanner.UI.Editor
{
    public partial class ZDataCollectionEditor : UserControl
    {
        private static readonly double LOG10 = Math.Log(10);
        ZDynamicCollection collectionObject;


        public ZDataCollectionEditor()
        {
            InitializeComponent();
            listBox.DrawItem += new DrawItemEventHandler(this.Listbox_drawItem);
        }

        public ListBox CollectionListBox { get { return this.listBox; } }

        public ZDynamicCollection CollectionObject 
        {
            get { return collectionObject; }
            set { collectionObject = value; FillListView(); } 
        }

        private void FillListView()
        {
            listBox.Items.Clear();
            labelTitle.Text = string.Empty;
            labelDescription.Text = string.Empty;

            if (collectionObject != null)
            {
                foreach (var item in collectionObject.Collection)
                {
                    listBox.Items.Add(item);
                }

                labelTitle.Text = collectionObject.Title;
                labelDescription.Text = collectionObject.Description;
            }
        }

        /// <devdoc> 
        ///     This draws a row of the listBox.
        /// </devdoc> 
        private void Listbox_drawItem(object sender, DrawItemEventArgs e)
        {
            if (e.Index != -1)
            {
                ZEntity item = listBox.Items[e.Index] as ZEntity;

                Graphics g = e.Graphics;

                int c = listBox.Items.Count;
                int maxC = (c > 1) ? c - 1 : c;
                // We add the +4 is a fudge factor... 
                // 
                SizeF sizeW = g.MeasureString(maxC.ToString(CultureInfo.InvariantCulture), listBox.Font);

                int charactersInNumber = ((int)(Math.Log((double)maxC) / LOG10) + 1);// Luckily, this is never called if count = 0
                int w = 4 + charactersInNumber * (Font.Height / 2);

                w = Math.Max(w, (int)Math.Ceiling(sizeW.Width));
                w += SystemInformation.BorderSize.Width * 4;

                Rectangle button = new Rectangle(e.Bounds.X, e.Bounds.Y, w, e.Bounds.Height);

                ControlPaint.DrawButton(g, button, ButtonState.Normal);
                button.Inflate(-SystemInformation.BorderSize.Width * 2, -SystemInformation.BorderSize.Height * 2);

                int offset = w;

                Color backColor = SystemColors.Window;
                Color textColor = SystemColors.WindowText;
                if ((e.State & DrawItemState.Selected) == DrawItemState.Selected)
                {
                    backColor = SystemColors.Highlight;
                    textColor = SystemColors.HighlightText;
                }
                Rectangle res = new Rectangle(e.Bounds.X + offset, e.Bounds.Y,
                                              e.Bounds.Width - offset,
                                              e.Bounds.Height);
                g.FillRectangle(new SolidBrush(backColor), res);
                if ((e.State & DrawItemState.Focus) == DrawItemState.Focus)
                {
                    ControlPaint.DrawFocusRectangle(g, res);
                }
                offset += 2;

                StringFormat format = new StringFormat();
                try
                {
                    format.Alignment = StringAlignment.Center;
                    g.DrawString((e.Index + 1).ToString(CultureInfo.InvariantCulture), Font, SystemBrushes.ControlText,
                                 new Rectangle(e.Bounds.X, e.Bounds.Y - 1, w, e.Bounds.Height), format);
                }

                finally
                {
                    if (format != null)
                    {
                        format.Dispose();
                    }
                }

                Brush textBrush = new SolidBrush(textColor);

                string itemText = GetDisplayText(item);

                try
                {
                    g.DrawString(itemText, Font, textBrush,
                                 new Rectangle(e.Bounds.X + offset, e.Bounds.Y, e.Bounds.Width - offset, e.Bounds.Height));
                }

                finally
                {
                    if (textBrush != null)
                    {
                        textBrush.Dispose();
                    }
                }

                // Check to see if we need to change the horizontal extent of the listBox
                //
                int width = offset + (int)g.MeasureString(itemText, Font).Width;
                if (width > e.Bounds.Width && listBox.HorizontalExtent < width)
                {
                    listBox.HorizontalExtent = width;
                }
            }
        }

        private string GetDisplayText(ZEntity item)
        {
            return (item == null) ? String.Empty : item.Title.ToString();
        }

        private object CreateInstance(ZDynamicCollection collectionObject)
        {
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            try
            {
                switch (collectionObject.ElementType.Name)
                {
                    case "ZStackup":
                        return new ZStackup(collectionObject.Project);
                    case "ZLayer":
                        return new ZLayer(collectionObject.Stackup, collectionObject.Stackup);
                    case "ZSingle":
                        return new ZSingle(collectionObject.Stackup);
                    case "ZPair":
                        return new ZPair(collectionObject.Stackup);
                    case "ZSpan":
                        return new ZSpan(collectionObject.Stackup);
                }
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
            return null;
        }

        private void buttonUp_Click(object sender, EventArgs e)
        {
            int index = listBox.SelectedIndex;
            if (index <= 0 || index >= listBox.Items.Count) return;

            (collectionObject.Collection as IMovableList).MoveAt(listBox.Items[index], index - 1);

            FillListView();

            listBox.SelectedIndex = index - 1;
        }

        private void buttonDown_Click(object sender, EventArgs e)
        {
            int index = listBox.SelectedIndex;
            if (index < 0 || index >= listBox.Items.Count - 1) return;

            (collectionObject.Collection as IMovableList).MoveAt(listBox.Items[index], index + 1);

            FillListView();

            listBox.SelectedIndex = index + 1;
        }

        private void buttonAdd_Click(object sender, EventArgs e)
        {
            object instance = CreateInstance(collectionObject);
            if (instance != null)
            {
                collectionObject.Collection.Add(instance);
            }

            FillListView();

            listBox.SelectedIndex = listBox.Items.Count - 1;
        }

        private void buttonRemove_Click(object sender, EventArgs e)
        {
            int index = listBox.SelectedIndex;
            if (index < 0 || index >= listBox.Items.Count) return;

            if (index >= 0 && index < listBox.Items.Count)
            {
                // single object selected or multiple ?
                if (listBox.SelectedItems.Count > 1)
                {
                    foreach (var item in listBox.SelectedItems)
                    {
                        collectionObject.Collection.Remove(item);
                    }
                }
                else
                {
                    collectionObject.Collection.Remove(listBox.SelectedItem);
                }

                FillListView();

                // set the new selected index 
                if (index < listBox.Items.Count)
                {
                    listBox.SelectedIndex = index;
                }
                else if (listBox.Items.Count > 0)
                {
                    listBox.SelectedIndex = listBox.Items.Count - 1;
                }
            } 
        }
    }
}
