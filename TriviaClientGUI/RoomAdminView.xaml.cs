using Newtonsoft.Json.Linq;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
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
using System.Threading;
using Microsoft.VisualBasic;

namespace TriviaClientGUI
{
    /// <summary>
    /// Interaction logic for UserControl1.xaml
    /// </summary>
    public partial class RoomAdminView : UserControl
    {
        public TcpClient client { get; set; }
        public NetworkStream stream { get; set; }

        Thread listenToUsersThread;
        Thread listenToServerThread;

        // Define an event for gameStarted
        public event EventHandler GameStarted;

        public string viewedRoomName;
        public int currentRoomId;

        public bool flag_getRooms;
        public bool flag_StartGame;
        public bool flag_CheckForUsers;
        public bool flag_refreshUsersInRoom;

        public RoomAdminView()
        {
            InitializeComponent();
        }

        private void ListenToServer()
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (flag_CheckForUsers)
                {
                    // Convert the message string to a byte array
                    byte[] messageBytes = Encoding.UTF8.GetBytes("o");

                    // Send the message to the server
                    stream.Write(messageBytes, 0, messageBytes.Length);

                    // Set flag to true
                    flag_refreshUsersInRoom = true;

                    // Get the users in this room
                    ReceiveGetUsersInRoomMessage();

                    Thread.Sleep(2000); // Sleep for 2 seconds
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
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

        // TODO: ADD THREAD TO SEND 'GET PLAYERS IN ROOM' EVERY 5 SECONDS TO REFRESH PLAYERS IN ROOM
        public void GetRooms()
        {
            try
            {
                RoomTitle.Text = ("Created Room: " + viewedRoomName);

                // Convert the message string to a byte array
                byte[] messageBytes = Encoding.UTF8.GetBytes("j");

                // Send the message to the server
                stream.Write(messageBytes, 0, messageBytes.Length);
                //MessageBox.Show("Sending get rooms message");

                flag_getRooms = true;
                ReceiveGetRoomMessages();

                listenToServerThread = new Thread(ListenToServer);
                flag_CheckForUsers = true;
                listenToServerThread.Start();
            }
            catch (Exception ex)
            {
                // Handle any exceptions that occur during the write operation
                MessageBox.Show($"Error sending get rooms message: {ex.Message}");
            }
        }

        // This function gets all of the available rooms and gives this room it's ID
        private void ReceiveGetRoomMessages()
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (flag_getRooms)
                {
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0)
                    {
                        break;
                    }
                    string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                    //MessageBox.Show("Received JSON string: " + message); // Print received JSON strin

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
                        if (json["Rooms"] != null)
                        {
                            JToken roomsToken = json["Rooms"];

                            // Check if the 'Rooms' token is an array
                            if (roomsToken is JArray roomsArray)
                            {
                                // Iterate over each room object in the array
                                foreach (JObject room in roomsArray)
                                {
                                    // If the rooms have the same name, give the id to this room
                                    if (viewedRoomName == room["Name"].ToString())
                                    {
                                        currentRoomId = (int)room["ID"];
                                    }
                                }
                                flag_getRooms = false;
                            }
                            else if (roomsToken is JObject roomObject) // Check if 'Rooms' token is a single room object
                            {
                                if (viewedRoomName == roomObject["Name"].ToString())
                                {
                                    currentRoomId = (int)roomObject["ID"];
                                }

                                flag_getRooms = false;
                            }
                            else
                            {
                                // Handle unexpected format of 'Rooms' field
                                MessageBox.Show("Unexpected format of 'Rooms' field.");
                                flag_getRooms = false;
                            }
                        }
                        else
                        {
                            // Handle the case where 'Rooms' field is missing
                            MessageBox.Show("'Rooms' field not found in the message.");
                            flag_getRooms = false;
                        }
                    });
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

        // Helper function for starting game
        private void StartingGame()
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (true)
                {
                    // Read message
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead != 0)
                    {
                        string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                        //"p\0\0\0\u000f{\"Status\": \"1\"}"
                        // Room closed
                        if (message == "p\0\0\0\u000f{\"Status\": \"1\"}")
                        {
                            //MessageBox.Show("Admin side: Game started!");

                            // Trigger the gameStarted event
                            GameStarted?.Invoke(this, EventArgs.Empty);

                            // Remove the view from its parent container
                            var parentGrid = this.Parent as Grid;
                            if (parentGrid != null)
                            {
                                this.Dispatcher.Invoke(() =>
                                {
                                    parentGrid.Children.Remove(this);
                                });
                            }

                            flag_CheckForUsers = false;
                            break;
                        }
                        else
                        {
                            MessageBox.Show("Admin side: error starting game");

                            // Remove the SignUpView from its parent container
                            var parentGrid = this.Parent as Grid;
                            if (parentGrid != null)
                            {
                                parentGrid.Children.Remove(this);
                            }

                            flag_CheckForUsers = false;
                            break;
                        }
                    }

                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        private void StartGame_Click(object sender, RoutedEventArgs e)
        {
            flag_CheckForUsers = false;
            // Join the threads to ensure they stop before removing the UserControl
            //listenToServerThread.Join();

            // Convert the message string to a byte array
            byte[] messageBytes = Encoding.UTF8.GetBytes("p");

            // Send the message to the server
            stream.Write(messageBytes, 0, messageBytes.Length);

            StartingGame();

            //// Remove the SignUpView from its parent container
            //var parentGrid = this.Parent as Grid;
            //if (parentGrid != null)
            //{
            //    parentGrid.Children.Remove(this);
            //}
        }

        // Helper function for closing room
        private void ClosingRoom()
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (true)
                {
                    // Read message
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead != 0)
                    {
                        string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                        //MessageBox.Show("Admin received: " + message);

                        // Room closed
                        if (message == "{\"Status\": \"1\"}")
                        {
                            //MessageBox.Show("Closed room.");

                            // Remove the SignUpView from its parent container
                            var parentGrid = this.Parent as Grid;
                            if (parentGrid != null)
                            {
                                parentGrid.Children.Remove(this);
                            }

                            flag_CheckForUsers = false;

                            break;
                        }
                    }

                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        private void CloseRoom_Click(object sender, RoutedEventArgs e)
        {
            flag_CheckForUsers = false;
            // Join the threads to ensure they stop before removing the UserControl
            //listenToServerThread.Join();

            // Convert the message string to a byte array
            byte[] messageBytes = Encoding.UTF8.GetBytes("n");

            // Send the message to the server
            stream.Write(messageBytes, 0, messageBytes.Length);

            ClosingRoom();

            //// Remove the SignUpView from its parent container
            //var parentGrid = this.Parent as Grid;
            //if (parentGrid != null)
            //{
            //    parentGrid.Children.Remove(this);
            //}
        }
    }
}
