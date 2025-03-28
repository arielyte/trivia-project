using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace TriviaClientGUI
{
    /// <summary>
    /// Interaction logic for UserControl1.xaml
    /// </summary>
    public partial class CreateRoomView : UserControl
    {
        public TcpClient client { get; set; }
        public NetworkStream stream { get; set; }

        public int timeForEachQuestion;
        public int numOfQuestions;

        public bool flag_CreateRoom;

        public event EventHandler RoomCreated;

        public bool flag_InsideCreateRoomOption;
        public string createdRoomName { get; private set; }
        public CreateRoomView()
        {
            InitializeComponent();
            flag_InsideCreateRoomOption = true;
            // Start the message receiving thread when the CreateRoomView is initialized
        }

        private void ReturnToMenu_Click(object sender, RoutedEventArgs e)
        {
            flag_InsideCreateRoomOption = false; // Disable create room loop
            // Remove the SignUpView from its parent container
            var parentGrid = this.Parent as Grid;
            if (parentGrid != null)
            {
                parentGrid.Children.Remove(this);
            }
        }

        private void ReceiveCreateRoomMessages()
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (flag_InsideCreateRoomOption == true)
                {
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0)
                    {
                        break;
                    }
                    string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                    //MessageBox.Show("CreateRoom: " + message);

                    if (flag_CreateRoom == true && message == "{\"Status\": \"1\"}")
                    {
                        this.Dispatcher.Invoke(() =>
                        {
                            //MessageBox.Show("Created room successfully");
                            flag_CreateRoom = false;

                            flag_InsideCreateRoomOption = false;

                            // Trigger the event to indicate that a room is created
                            RoomCreated?.Invoke(this, EventArgs.Empty);

                            // Remove the SignUpView from its parent container
                            var parentGrid = this.Parent as Grid;
                            if (parentGrid != null)
                            {
                                parentGrid.Children.Remove(this);
                            }
                        });
                    }
                    else if (flag_CreateRoom == true && message != "{\"Status\": \"1\"}")
                    {
                        this.Dispatcher.Invoke(() =>
                        {
                            flag_CreateRoom = false;
                            MessageBox.Show("Error creating room: " + message);
                            flag_InsideCreateRoomOption = false;
                        });
                    }
                }
            }
            catch (Exception ex)
            {
                this.Dispatcher.Invoke(() =>
                {
                    CreateRoomMessageTextBlock.Text = $"Error receiving message: {ex.Message}";
                });
            }
        }

        private void SendCreateRoomMessage(int messageCode, string jsonMessage)
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

                flag_InsideCreateRoomOption = true;
                ReceiveCreateRoomMessages();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        private void CreateButton_Click(object sender, RoutedEventArgs e)
        {
            // Get the text from the text boxes
            string roomName = RoomNameTextBox.Text;
            string maxUsers = MaxUsersTextBox.Text;
            string questionCount = QuestionCountTextBox.Text;
            string answerTimeout = AnswerTimeoutTextBox.Text;

            // Check if any of the text boxes are empty
            if (string.IsNullOrWhiteSpace(roomName) ||
                string.IsNullOrWhiteSpace(maxUsers) ||
                string.IsNullOrWhiteSpace(questionCount) ||
                string.IsNullOrWhiteSpace(answerTimeout))
            {
                MessageBox.Show("Please fill in all fields.");
                return; // Exit the method if any field is empty
            }

            int maxUsersValue, questionCountValue, answerTimeoutValue;

            // Validate integer parsing and non-negativity
            if (!int.TryParse(maxUsers, out maxUsersValue) || maxUsersValue <= 0 ||
                !int.TryParse(questionCount, out questionCountValue) || questionCountValue <= 0 ||
                !int.TryParse(answerTimeout, out answerTimeoutValue) || answerTimeoutValue <= 0)
            {
                MessageBox.Show("All fields must have valid non-negative integer values.");
                return;
            }

            // Validate question count
            if (questionCountValue > 10)
            {
                MessageBox.Show("Question count cannot exceed 10.");
                return;
            }

            // Set global vars for game
            timeForEachQuestion = int.Parse(answerTimeout);
            numOfQuestions = int.Parse(questionCount);

            try
            {
                // Prepare create room JSON message with entered values
                var createRoomJson = new
                {
                    RoomName = roomName,
                    MaxUsers = maxUsersValue,
                    QuestionCount = questionCountValue,
                    AnswerTimeout = answerTimeoutValue
                };

                string jsonMessage = Newtonsoft.Json.JsonConvert.SerializeObject(createRoomJson);

                // Set the name of the created room
                createdRoomName = roomName;

                // Set the flag after sending the message
                flag_CreateRoom = true;

                SendCreateRoomMessage(105, jsonMessage);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

    }
}
