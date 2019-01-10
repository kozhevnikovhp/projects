using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class ManufacturerColor : Form
    {
        List<ZMaterial> materialList;
        Dictionary<string, Color> colorMap;
        Dictionary<string, bool>  checkMap;

        class cMaterialCheckBox : CheckBox
        {
            public string manufacturer;
        }

        class cMaterialPictureBox : PictureBox
        {
            public string manufacturer;
        }

        class cMaterialLabel : Label
        {
            public string manufacturer;
        }

        public bool IsChecked(string name)
        {
            return checkMap[name];
        }

        public Color GetColor(string name)
        {
            return colorMap[name];
        }

        cMaterialLabel currentSelection = null;
        internal ManufacturerColor(List<ZMaterial> input, Dictionary<string, Color> aColorMap, Dictionary<string, bool> aCheckMap)
        {
            InitializeComponent();
            materialList = new List<ZMaterial>(input);

            tblMaterialPanel.Controls.Clear();
            tblMaterialPanel.RowStyles.Clear();
            tblMaterialPanel.RowCount = 1;
            int vertScrollWidth = SystemInformation.VerticalScrollBarWidth;
            tblMaterialPanel.Padding = new Padding(0, 0, vertScrollWidth, 0);

            colorMap = aColorMap;
            checkMap = aCheckMap;

            List<Color> palette = new List<Color> { Color.Red, Color.Green, Color.Blue, Color.Orange, Color.LimeGreen, Color.Magenta, Color.RoyalBlue, Color.Brown, Color.BlueViolet };
            int idx = -1;
            bool bFirst = true;
            HashSet<string> materialSet = new HashSet<string>();
            foreach (ZMaterial zM in materialList)
            {
                string name = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value;
                if (materialSet.Contains(name)) continue;
                
                materialSet.Add(name);
                idx++;
                bool bCheck = true;
                if (!checkMap.ContainsKey(name))
                {
                    checkMap.Add(name, bCheck);
                }
                else
                {
                    bCheck = checkMap[name]; 
                }
                Color c = palette[0];
                if (!colorMap.ContainsKey(name))
                {
                    c = palette[idx % palette.Count];
                    colorMap.Add(name, c);
                }
                else
                {
                    c = colorMap[name];
                }

                if (bFirst)
                {
                    bFirst = false;
                }
                else
                {
                    tblMaterialPanel.RowCount += 1;
                }
                tblMaterialPanel.RowStyles.Add(new RowStyle(SizeType.Absolute, 17F));

                var checkBox = new cMaterialCheckBox() { Checked = bCheck, Text = "", Height = 15, Margin = new Padding(0, 0, 0, 0), manufacturer = name };
                checkBox.CheckedChanged += new System.EventHandler(genericCheckBox_CheckedChanged);
                tblMaterialPanel.Controls.Add(checkBox, 0, tblMaterialPanel.RowCount - 1);


                var pictureBox = new cMaterialPictureBox() { BackColor = c, Width = 15, Height = 15, Margin = new Padding(2, 0, 0, 0), BorderStyle = BorderStyle.FixedSingle, manufacturer = name };
                pictureBox.Click += new System.EventHandler(genericPictureBox_Click);
                tblMaterialPanel.Controls.Add(pictureBox, 1, tblMaterialPanel.RowCount - 1);
                int wLabel = (int)tblMaterialPanel.ColumnStyles[2].Width;

                var label = new cMaterialLabel() { Text = name, AutoSize = false, Width = wLabel/*180*/, Height = 15, BorderStyle = BorderStyle.None, BackColor = SystemColors.Window, Margin = new Padding(0, 0, 0, 0), manufacturer = name };
                label.Click += new System.EventHandler(genericLabel_Click);
                tblMaterialPanel.Controls.Add(label, 2, tblMaterialPanel.RowCount - 1);
                if (currentSelection == null)
                {
                    currentSelection = label;
                }

                var label1 = new cMaterialLabel() { AutoSize = false, Width = (int)tblMaterialPanel.ColumnStyles[3].Width, Height = 15, BorderStyle = BorderStyle.None, BackColor = SystemColors.Window, Margin = new Padding(0, 0, 0, 0), manufacturer = name };
                label1.Font = new Font(label1.Font, FontStyle.Bold);
                tblMaterialPanel.Controls.Add(label1, 3, tblMaterialPanel.RowCount - 1);
                
            }
            tblMaterialPanel.HorizontalScroll.Enabled = false;
        }

        private void genericLabel_Click(object sender, EventArgs e)
        {
            currentSelection.BackColor = SystemColors.Window;
            TableLayoutPanelCellPosition selpos = tblMaterialPanel.GetPositionFromControl(currentSelection);
            cMaterialLabel selValLabel = tblMaterialPanel.GetControlFromPosition(selpos.Column + 1, selpos.Row) as cMaterialLabel;
            selValLabel.BackColor = SystemColors.Window;

            cMaterialLabel lb = sender as cMaterialLabel;
            currentSelection = lb;
            lb.BackColor = ZColorConstants.MaterialMapperHighlightColor;
            TableLayoutPanelCellPosition tpos = tblMaterialPanel.GetPositionFromControl(lb);
            cMaterialLabel valLabel = tblMaterialPanel.GetControlFromPosition(tpos.Column + 1, tpos.Row) as cMaterialLabel;
            valLabel.BackColor = ZColorConstants.MaterialMapperHighlightColor;
        }

        private void genericCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            cMaterialCheckBox mcb = sender as cMaterialCheckBox;
            checkMap[mcb.manufacturer] = mcb.Checked;
        }

        private void genericPictureBox_Click(object sender, EventArgs e)
        {
            cMaterialPictureBox mpb = sender as cMaterialPictureBox;

            ColorDialog MyDialog = new ColorDialog();
            MyDialog.AllowFullOpen = false;
            MyDialog.Color = mpb.BackColor;

            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                mpb.BackColor = MyDialog.Color;
                colorMap[mpb.manufacturer] = MyDialog.Color;
            }
        }


    }
}
