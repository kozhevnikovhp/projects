namespace ZZero.ZPlanner.UI.EMail
{
    partial class EMailForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EMailForm));
            this.labelFrom = new System.Windows.Forms.Label();
            this.labelTo = new System.Windows.Forms.Label();
            this.labelSubject = new System.Windows.Forms.Label();
            this.labelAttachment = new System.Windows.Forms.Label();
            this.textBoxFrom = new System.Windows.Forms.TextBox();
            this.textBoxTo = new System.Windows.Forms.TextBox();
            this.textBoxSubject = new System.Windows.Forms.TextBox();
            this.textBoxAttachment = new System.Windows.Forms.TextBox();
            this.textBoxMessage = new System.Windows.Forms.TextBox();
            this.buttonSend = new System.Windows.Forms.Button();
            this.buttonClose = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // labelFrom
            // 
            this.labelFrom.Location = new System.Drawing.Point(5, 11);
            this.labelFrom.Name = "labelFrom";
            this.labelFrom.Size = new System.Drawing.Size(70, 16);
            this.labelFrom.TabIndex = 0;
            this.labelFrom.Text = "From:";
            this.labelFrom.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // labelTo
            // 
            this.labelTo.Location = new System.Drawing.Point(5, 31);
            this.labelTo.Name = "labelTo";
            this.labelTo.Size = new System.Drawing.Size(70, 16);
            this.labelTo.TabIndex = 1;
            this.labelTo.Text = "To:";
            this.labelTo.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // labelSubject
            // 
            this.labelSubject.Location = new System.Drawing.Point(5, 51);
            this.labelSubject.Name = "labelSubject";
            this.labelSubject.Size = new System.Drawing.Size(70, 16);
            this.labelSubject.TabIndex = 2;
            this.labelSubject.Text = "Subject:";
            this.labelSubject.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // labelAttachment
            // 
            this.labelAttachment.Location = new System.Drawing.Point(5, 71);
            this.labelAttachment.Name = "labelAttachment";
            this.labelAttachment.Size = new System.Drawing.Size(70, 16);
            this.labelAttachment.TabIndex = 3;
            this.labelAttachment.Text = "Attachment:";
            this.labelAttachment.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // textBoxFrom
            // 
            this.textBoxFrom.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxFrom.Location = new System.Drawing.Point(80, 9);
            this.textBoxFrom.Name = "textBoxFrom";
            this.textBoxFrom.Size = new System.Drawing.Size(520, 20);
            this.textBoxFrom.TabIndex = 4;
            // 
            // textBoxTo
            // 
            this.textBoxTo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxTo.Location = new System.Drawing.Point(80, 29);
            this.textBoxTo.Name = "textBoxTo";
            this.textBoxTo.Size = new System.Drawing.Size(520, 20);
            this.textBoxTo.TabIndex = 4;
            // 
            // textBoxSubject
            // 
            this.textBoxSubject.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxSubject.Location = new System.Drawing.Point(80, 49);
            this.textBoxSubject.Name = "textBoxSubject";
            this.textBoxSubject.Size = new System.Drawing.Size(520, 20);
            this.textBoxSubject.TabIndex = 4;
            // 
            // textBoxAttachment
            // 
            this.textBoxAttachment.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxAttachment.Location = new System.Drawing.Point(80, 69);
            this.textBoxAttachment.Name = "textBoxAttachment";
            this.textBoxAttachment.ReadOnly = true;
            this.textBoxAttachment.Size = new System.Drawing.Size(520, 20);
            this.textBoxAttachment.TabIndex = 4;
            // 
            // textBoxMessage
            // 
            this.textBoxMessage.AcceptsReturn = true;
            this.textBoxMessage.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxMessage.Location = new System.Drawing.Point(10, 92);
            this.textBoxMessage.Multiline = true;
            this.textBoxMessage.Name = "textBoxMessage";
            this.textBoxMessage.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBoxMessage.Size = new System.Drawing.Size(589, 140);
            this.textBoxMessage.TabIndex = 5;
            this.textBoxMessage.WordWrap = false;
            // 
            // buttonSend
            // 
            this.buttonSend.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonSend.Location = new System.Drawing.Point(430, 236);
            this.buttonSend.Name = "buttonSend";
            this.buttonSend.Size = new System.Drawing.Size(82, 23);
            this.buttonSend.TabIndex = 6;
            this.buttonSend.Text = "Send";
            this.buttonSend.UseVisualStyleBackColor = true;
            this.buttonSend.Click += new System.EventHandler(this.buttonSend_Click);
            // 
            // buttonClose
            // 
            this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonClose.Location = new System.Drawing.Point(518, 236);
            this.buttonClose.Name = "buttonClose";
            this.buttonClose.Size = new System.Drawing.Size(82, 23);
            this.buttonClose.TabIndex = 6;
            this.buttonClose.Text = "Cancel";
            this.buttonClose.UseVisualStyleBackColor = true;
            this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
            // 
            // EMailForm
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(606, 262);
            this.Controls.Add(this.buttonClose);
            this.Controls.Add(this.buttonSend);
            this.Controls.Add(this.textBoxMessage);
            this.Controls.Add(this.textBoxAttachment);
            this.Controls.Add(this.textBoxSubject);
            this.Controls.Add(this.textBoxTo);
            this.Controls.Add(this.textBoxFrom);
            this.Controls.Add(this.labelAttachment);
            this.Controls.Add(this.labelSubject);
            this.Controls.Add(this.labelTo);
            this.Controls.Add(this.labelFrom);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "EMailForm";
            this.ShowInTaskbar = false;
            this.Text = "  Send Email";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelFrom;
        private System.Windows.Forms.Label labelTo;
        private System.Windows.Forms.Label labelSubject;
        private System.Windows.Forms.Label labelAttachment;
        private System.Windows.Forms.TextBox textBoxFrom;
        private System.Windows.Forms.TextBox textBoxTo;
        private System.Windows.Forms.TextBox textBoxSubject;
        private System.Windows.Forms.TextBox textBoxAttachment;
        private System.Windows.Forms.TextBox textBoxMessage;
        private System.Windows.Forms.Button buttonSend;
        private System.Windows.Forms.Button buttonClose;
    }
}