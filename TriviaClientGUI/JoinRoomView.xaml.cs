using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
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
using Newtonsoft.Json.Linq;
using Newtonsoft.Json;

namespace TriviaClientGUI
{
    /// <summary>
    /// Interaction logic for UserControl1.xaml
    /// </summary>
    public partial class JoinRoomView : UserControl
    {
        public TcpClient client { get; set; }
        public NetworkStream stream { get; set; }

        public event EventHandler RoomJoined;

        public bool flag_JoinRoom;

        public bool flag_JoinedJoinRoomOption;

        public bool flag_getUsersInRoom;

        public bool flag_tryingToJoinRoom;

        public string joinedRoomNameFinal;
        public int joinedRoomIdFinal;
        public JoinRoomView()
        {
            InitializeComponent();

            flag_JoinedJoinRoomOption = true;
        }

        private void CloseRoom_Click(object sender, RoutedEventArgs e)
        {
            flag_JoinedJoinRoomOption = false;

            // Remove the SignUpView from its parent container
            var parentGrid = this.Parent as Grid;
            if (parentGrid != null)
            {
                parentGrid.Children.Remove(this);
            }
        }

        public void getRooms()
        {
            try
            {
                // Convert the message string to a byte array
                byte[] messageBytes = Encoding.UTF8.GetBytes("j");

                // Send the message to the server
                stream.Write(messageBytes, 0, messageBytes.Length);
                //MessageBox.Show("Sending get rooms message");

                flag_JoinedJoinRoomOption = true;
                ReceiveJoinRoomMessages();
            }
            catch (Exception ex)
            {
                // Handle any exceptions that occur during the write operation
                MessageBox.Show($"Error sending get rooms message: {ex.Message}");
            }
        }

        private void SendJoinRoomMessage(int messageCode, string jsonMessage)
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
                //MessageBox.Show($"Sent message with code {messageCode} and size {messageSize}");

            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error sending message: {ex.Message}");
            }
        }


        // This function shows all of the available rooms to join
        private void ReceiveJoinRoomMessages()
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (flag_JoinedJoinRoomOption)
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
                        roomNamesStackPanel.Children.Clear(); // Clear existing buttons

                        if (json["Rooms"] != null)
                        {
                            JToken roomsToken = json["Rooms"];

                            // Check if the 'Rooms' token is an array
                            if (roomsToken is JArray roomsArray)
                            {
                                // Iterate over each room object in the array
                                foreach (JObject room in roomsArray)
                                {
                                    string roomName = room["Name"].ToString();
                                    int roomId = (int)room["ID"]; // Get the room ID
                                    Button roomButton = new Button();
                                    roomButton.Content = roomName;
                                    roomButton.Width = 150; // Set width to 150 pixels
                                    roomButton.Height = 40; // Set height to 40 pixels
                                    roomButton.FontSize = 18; // Set font size to 16 points
                                    roomButton.Click += (s, e) => OnRoomButtonClick(roomName, roomId); // Call a method to handle button click
                                    roomNamesStackPanel.Children.Add(roomButton);
                                }
                                flag_JoinedJoinRoomOption = false;
                            }
                            else if (roomsToken is JObject roomObject) // Check if 'Rooms' token is a single room object
                            {
                                // Create button for the single room object
                                string roomName = roomObject["Name"].ToString();
                                int roomId = (int)roomObject["ID"]; // Get the room ID
                                Button roomButton = new Button();
                                roomButton.Content = roomName;
                                roomButton.Width = 150; // Set width to 150 pixels
                                roomButton.Height = 40; // Set height to 40 pixels
                                roomButton.FontSize = 18; // Set font size to 16 points
                                roomButton.Click += (s, e) => OnRoomButtonClick(roomName, roomId); // Call a method to handle button click
                                roomNamesStackPanel.Children.Add(roomButton);
                                flag_JoinedJoinRoomOption = false;
                            }
                            else
                            {
                                // Handle unexpected format of 'Rooms' field
                                MessageBox.Show("Unexpected format of 'Rooms' field.");
                                flag_JoinedJoinRoomOption = false;
                            }
                        }
                        else
                        {
                            // Handle the case where 'Rooms' field is missing
                            MessageBox.Show("'Rooms' field not found in the message.");
                            flag_JoinedJoinRoomOption = false;
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
        private void recieveJoinRoomAttemptMessage(string joinedRoomName, int joinedRoomId)
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (flag_tryingToJoinRoom)
                {
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0)
                    {
                        break;
                    }
                    string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                    // TODO: add message that also sends time for each question and question count
                    if (message == "{\"Status\": \"1\"}")
                    {
                        //MessageBox.Show($"Joined room: {joinedRoomName} with id {joinedRoomId}");
                        flag_tryingToJoinRoom = false;

                        joinedRoomNameFinal = joinedRoomName;
                        joinedRoomIdFinal = joinedRoomId;

                        this.Dispatcher.Invoke(() =>
                        {
                            // Trigger the event to indicate that a room is created
                            RoomJoined?.Invoke(this, EventArgs.Empty);
                        });
                    }
                    else if (message == "Max amount of players in room")
                    {
                        MessageBox.Show("Room is full.");
                        flag_tryingToJoinRoom = false;
                    }
                    else
                    {
                        MessageBox.Show("Error joining room.");
                        flag_tryingToJoinRoom = false;
                    }
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
                while (flag_getUsersInRoom && stream != null)
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
                        userInRoomStackPanel.Children.Clear();

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
                                playerTextBlock.FontSize = 16;

                                // Add the TextBlock to the userInRoomStackPanel
                                userInRoomStackPanel.Children.Add(playerTextBlock);
                            }
                        }
                        else
                        {
                            MessageBox.Show("'Players' field not found in the message.");
                        }
                    });
                    flag_getUsersInRoom = false;
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


        // Method to handle button click
        private void OnRoomButtonClick(string roomName, int roomId)
        {
            try
            {
                userInRoomTitle.Text = ("Users in: " + roomName);

                userInRoomTitle.Visibility = Visibility.Visible;
                userInRoomBorder.Visibility = Visibility.Visible;
                JoinRoomButton.Visibility = Visibility.Visible;
                JoinRoomButton.Click += (sender, e) => JoinRoomButtonClick(sender, e, roomId, roomName);
                userInRoomStackPanel.Visibility = Visibility.Visible;

                // Implement your logic here to handle the button click, e.g., joining the room
                //MessageBox.Show($"Clicked on room: {roomName} with ID: {roomId}");

                var loginJson = new { RoomID = roomId };
                string jsonMessage = Newtonsoft.Json.JsonConvert.SerializeObject(loginJson);

                SendJoinRoomMessage(103, jsonMessage);

                flag_getUsersInRoom = true;

                ReceiveGetUsersInRoomMessage();
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void JoinRoomButtonClick(object sender, RoutedEventArgs e, int roomId, string roomName)
        {
            var loginJson = new { RoomID = roomId };
            string jsonMessage = Newtonsoft.Json.JsonConvert.SerializeObject(loginJson);

            flag_tryingToJoinRoom = true;

            SendJoinRoomMessage(104, jsonMessage);

            recieveJoinRoomAttemptMessage(roomName, roomId);
        }
    }
}
