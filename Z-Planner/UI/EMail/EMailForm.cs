using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Mail;
using System.Net.Mime;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.EMail
{
    public partial class EMailForm : Form
    {
        public EMailForm()
        {
            InitializeComponent();
        }

        public string From
        {
            get { return textBoxFrom.Text; }
            set { textBoxFrom.Text = value ?? string.Empty; }
        }

        public string To
        {
            get { return textBoxTo.Text; }
            set { textBoxTo.Text = value ?? string.Empty; }
        }

        public string Subject
        {
            get { return textBoxSubject.Text; }
            set { textBoxSubject.Text = value ?? string.Empty; }
        }

        public string Attachment
        {
            get { return textBoxAttachment.Text; }
            set { textBoxAttachment.Text = value ?? string.Empty; }
        }

        public string Message
        {
            get { return textBoxMessage.Text; }
            set { textBoxMessage.Text = value ?? string.Empty; }
        }

        public void CreateMessageWithAttachment()
        {
            // Specify the file to be attached and sent.
            string file = Attachment;
            // Create a message and set up the recipients.
            MailMessage message = new MailMessage(From, To, Subject, Message);
            // Create  the file attachment for this e-mail message.
            Attachment data = new Attachment(file, MediaTypeNames.Text.Xml);
            // Add time stamp information for the file.
            ContentDisposition disposition = data.ContentDisposition;
            disposition.CreationDate = System.IO.File.GetCreationTime(file);
            disposition.ModificationDate = System.IO.File.GetLastWriteTime(file);
            disposition.ReadDate = System.IO.File.GetLastAccessTime(file);
            // Add the file attachment to this e-mail message.
            message.Attachments.Add(data);

            //Send the message.
            SmtpClient client = new SmtpClient();
            // Add credentials if the SMTP server requires them.
            //client.Credentials = CredentialCache.DefaultNetworkCredentials;
            NetworkCredential nc = client.Credentials as NetworkCredential;
            if (nc != null && (nc.UserName == "ENTER YOUR GOOGLE-MAIL ADDRESS" || nc.Password == "ENTER YOUR PASSWORD"))
            {
                client.Credentials = new NetworkCredential(ZStringConstants.eMailAccount, ZStringConstants.ZplannerCRD.Substring(9,9));
            }
            
            message.From = new MailAddress(From);

            try
            {
                client.Send(message);
            }
            catch (SmtpException ex)
            {
                MessageBox.Show("In order to be able to send email from the Z-planner application, you need to install MS Outlook, or fill the host, port, userName and password parameters of the mailSettings section in Z-planner.exe.config file in the Z-planner folder.");
                ZPlannerManager.StatusMenu.SetStatus("ERROR: The e-mail was not sent. " + ex.ToString());
            }
            catch (Exception ex)
            {
                ZPlannerManager.StatusMenu.SetStatus("ERROR: The e-mail was not sent. " + ex.ToString());
            }
            data.Dispose();
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            CreateMessageWithAttachment();
            this.Close();
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
