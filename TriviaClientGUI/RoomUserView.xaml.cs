using Newtonsoft.Json.Linq;
using Newtonsoft.Json;
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
    public partial class RoomUserView : UserControl
    {
        public TcpClient client { get; set; }
        public NetworkStream stream { get; set; }

        public string joinedRoomName;
        public int joinedRoomId;

        Thread listenToUsersThread;

        // Define an event for gameStarted
        public event EventHandler GameStarted;

        public bool flag_CheckForMessages;
        public bool flag_refreshUsersInRoom;
        public bool flag_LeavingRoom;

        public int timeForEachQuestion;
        public int numOfQuestions;

        public RoomUserView()
        {
            InitializeComponent();
        }

        // Thread to listen to the server
        public void ListenToServer()
        {
            try
            {
                // Convert the message string to a byte array
                byte[] messageBytes = Encoding.UTF8.GetBytes("o");
                byte[] buffer = new byte[1024];

                while (flag_CheckForMessages)
                {
                    //MessageBox.Show("Checking for users");

                    // Read any incoming messages
                    if (stream.DataAvailable) // Check if there is data available to read
                    {
                        int bytesRead = stream.Read(buffer, 0, buffer.Length);

                        string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                        //MessageBox.Show("User receieved: " + message);

                        if (message == "q\0\0\0\u000f{\"Status\": \"1\"}")
                        {
                            // Remove the view from its parent container
                            var parentGrid = this.Parent as Grid;
                            if (parentGrid != null)
                            {
                                this.Dispatcher.Invoke(() =>
                                {
                                    parentGrid.Children.Remove(this);
                                });
                            }

                            flag_CheckForMessages = false;

                            break;
                        }
                        if (message == "p\0\0\0\u000f{\"Status\": \"1\"}")
                        {
                            // Trigger the gameStarted event
                            GameStarted?.Invoke(this, EventArgs.Empty);

                            // Remove the view from its parent container on the UI thread
                            this.Dispatcher.Invoke(() =>
                            {
                                var parentGrid = this.Parent as Grid;
                                if (parentGrid != null)
                                {
                                    parentGrid.Children.Remove(this);
                                }
                            });

                            flag_CheckForMessages = false;

                            break;
                        }
                    }
                    // else:
                    // Send the message to the server
                    stream.Write(messageBytes, 0, messageBytes.Length);
                    
                    // Set flag to true
                    flag_refreshUsersInRoom = true;

                    // Get the users in this room
                    ReceiveGetUsersInRoomMessage();

                    Thread.Sleep(1000); // wait a second
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        public void GetUsersInRoom()
        {
            try
            {
                RoomTitle.Text = ("Joined Room: " + joinedRoomName);

                listenToUsersThread = new Thread(ListenToServer);
                flag_CheckForMessages = true;
                listenToUsersThread.Start();
            }
            catch (Exception ex)
            {
                // Handle any exceptions that occur during the write operation
                MessageBox.Show($"Error sending get rooms message: {ex.Message}");
            }
        }

        // This function recieves the message from the server that shows the users in selected room
        private void ReceiveGetUsersInRoomMessage()
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (flag_refreshUsersInRoom && stream != null)
                {
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0)
                    {
                        break;
                    }

                    string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                    //MessageBox.Show("Received JSON string: " + message); // Print received JSON string

                    JObject json;
                    try
                    {
                        json = JObject.Parse(message);
                    }
                    catch (JsonReaderException ex)
                    {
                        MessageBox.Show($"Error parsing JSON: {ex.Message}");
                        continue;
                    }

                    this.Dispatcher.Invoke(() =>
                    {
                        // Clear existing player names
                        UsersInRoom.Children.Clear();

                        // Check if the 'Players' token is present
                        if (json["Players"] != null)
                        {
                            // Assign amount of questions
                            numOfQuestions = int.Parse(json["QuestionCount"].ToString());
                            //MessageBox.Show("Num of questions in user: " + numOfQuestions);

                            // Assign time for each question
                            timeForEachQuestion = int.Parse(json["AnswerTimeout"].ToString());
                            //MessageBox.Show("time of each question in user: " + timeForEachQuestion);

                            // Get the players string
                            string playersString = json["Players"].ToString();

                            string[] players = playersString.Split(',');

                            // Display each player in the UI
                            foreach (string player in players)
                            {
                                // Create a TextBlock to display the player name
                                TextBlock playerTextBlock = new TextBlock();
                                playerTextBlock.Text = player;
                                playerTextBlock.FontSize = 20;
                                playerTextBlock.VerticalAlignment = VerticalAlignment.Center;
                                playerTextBlock.HorizontalAlignment = HorizontalAlignment.Center;

                                // Add the TextBlock to the userInRoomStackPanel
                                UsersInRoom.Children.Add(playerTextBlock);
                            }
                        }
                        else
                        {
                            MessageBox.Show("'Players' field not found in the message.");
                        }
                    });
                    flag_refreshUsersInRoom = false;
                }
            }
            catch (Exception ex)
            {
                this.Dispatcher.Invoke(() =>
                {
                    MessageBox.Show($"Error receiving messages: {ex.Message}");
                });
            }
        }

        private void LeavingRoom()
        {
            try
            {
                byte[] buffer = new byte[1024];

                while (flag_LeavingRoom)
                {
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead != 0)
                    {
                        string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                       // MessageBox.Show("Receievd message: " + message);
                        if (message == "q\0\0\0\u000f{\"Status\": \"1\"}")
                        {
                            flag_LeavingRoom = false;

                            var parentGrid = this.Parent as Grid;
                            if (parentGrid != null)
                            {
                                parentGrid.Children.Remove(this);
                            }
                        }
                        else
                        {
                            flag_LeavingRoom = false;
                            MessageBox.Show("Issue with leaving room");
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void LeaveRoom_Click(object sender, RoutedEventArgs e)
        {
            // ADD SENDING MESSAGE THAT THE USER LEAVES THE ROOM

            flag_CheckForMessages = false;
            //listenToUsersThread.Join();

            // Convert the message string to a byte array
            byte[] messageBytes = Encoding.UTF8.GetBytes("q");

            // Send the message to the server
            stream.Write(messageBytes, 0, messageBytes.Length);

            flag_LeavingRoom = true;

            LeavingRoom();

            // Remove the view from its parent container
            var parentGrid = this.Parent as Grid;
            if (parentGrid != null)
            {
                parentGrid.Children.Remove(this);
            }
        }
    }
}

