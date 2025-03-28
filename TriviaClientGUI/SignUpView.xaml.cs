using TriviaClientGUI;
using System;
using System.Windows;
using System.Windows.Controls;
using System.IO;
using System.Text;
using System.Net.Sockets;

namespace TriviaClientGUI
{
    public partial class SignUpView : UserControl
    {
        public TcpClient client { get; set; }
        public NetworkStream stream { get; set; }

        public bool flag_SignUp;

        public SignUpView()
        {
            InitializeComponent();
        }

        // Your action is not relevant to your current state. do something doable!

        private void CreateButton_Click(object sender, RoutedEventArgs e)
        {
            // Get the information from the text boxes
            string name = nameTextBox.Text;
            string password = passwordTextBox.Text;
            string email = emailTextBox.Text;

            // Prepare the signup JSON message
            var signupJson = new { username = name, password = password, email = email };
            string jsonMessage = Newtonsoft.Json.JsonConvert.SerializeObject(signupJson);

            flag_SignUp = true;

            MessageBox.Show("User created!");
            // Send the signup message to the server
            SendUserMessage(102, jsonMessage);
        }

        private void SendUserMessage(int messageCode, string jsonMessage)
        {
            try
            {
                // Convert JSON message to bytes
                byte[] jsonBytes = Encoding.UTF8.GetBytes(jsonMessage);
                // Calculate message size
                int messageSize = jsonBytes.Length;
                // Construct message with code and size
                byte[] message = new byte[1 + 4 + messageSize];
                // First byte is message code
                message[0] = (byte)messageCode;
                // Next four bytes represent the message size
                message[1] = (byte)((messageSize >> 24) & 0xFF);
                message[2] = (byte)((messageSize >> 16) & 0xFF);
                message[3] = (byte)((messageSize >> 8) & 0xFF);
                message[4] = (byte)(messageSize & 0xFF);
                // Copy JSON bytes to the message
                Array.Copy(jsonBytes, 0, message, 5, messageSize);
                // Send the message
                stream.Write(message, 0, message.Length);
                Console.WriteLine($"Sent message with code {messageCode} and size {messageSize}");
                
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error sending message: {ex.Message}");
            }
        }

        private void ReturnToMenu_Click(object sender, RoutedEventArgs e)
        {
            // Remove the SignUpView from its parent container
            var parentGrid = this.Parent as Grid;
            if (parentGrid != null)
            {
                parentGrid.Children.Remove(this);
            }
        }


    }
}
