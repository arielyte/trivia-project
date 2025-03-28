using System;
using System.Collections.Generic;
using System.Linq;
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
using System.Net.Sockets;
using System.Threading;
using TriviaClientGUI;

namespace TriviaClientGUI
{
    public partial class MainWindow : Window
    {
        private TcpClient client;
        private NetworkStream stream;
        private Thread receiveThread;

        public string currentUsername = "";
        public bool flag_LogIn;
        public bool isUserLoggedIn;
        public bool flag_LogOut;
        public bool flag_JoiningRoomButton;

        public MainWindow()
        {
            InitializeComponent();
            ConnectToServer();
        }

        private void ConnectToServer()
        {
            try
            {
                client = new TcpClient();
                client.Connect("127.0.0.1", 8200);
                stream = client.GetStream();
                //MessageBox.Show("Connected to server.");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error connecting to server: {ex.Message}");
                // Attempt to reconnect after a delay
                Task.Delay(5000).ContinueWith(_ => ConnectToServer());
            }
        }

        private void ReceiveLoginLogoutMessage()
        {
            try
            {
                byte[] buffer = new byte[1024];
                int bytesRead = stream.Read(buffer, 0, buffer.Length);
                if (bytesRead == 0)
                {
                    // Server disconnected
                    this.Dispatcher.Invoke(() =>
                    {
                        MessageBox.Show("Server disconnected.");
                    });
                    // Attempt to reconnect
                    ConnectToServer();
                }
                string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                this.Dispatcher.Invoke(() =>
                {
                      //Log($"Server: {message}");
                });

                // Check message
                if (flag_LogIn && message == "{\"Status\": \"1\"}")
                {
                    this.Dispatcher.Invoke(() =>
                    {
                        //Log($"Login Successful, logged in as {currentUsername}");
                        messageTextBlock.Text = $"Logged in as: {currentUsername}";
                    });

                    flag_LogIn = false;
                    isUserLoggedIn = true;

                    this.Dispatcher.Invoke(() =>
                    {
                        LoginButton.Visibility = Visibility.Hidden;
                        LogoutButton.Visibility = Visibility.Visible;
                    });
                }

                // Log in checking

                else if (flag_LogIn && message == "{\"message\": \"Error: User does not exist. Cannot login.\"}")
                {
                    this.Dispatcher.Invoke(() =>
                    {
                        MessageBox.Show("This username doesn't exist");
                    });
                    isUserLoggedIn = false;
                    flag_LogIn = false;
                }
                else if (flag_LogIn && message == "{\"message\": \"Error: Incorrect password. Cannot login.\"}")
                {
                    this.Dispatcher.Invoke(() =>
                    {
                        MessageBox.Show("Incorrect password");
                    });
                    isUserLoggedIn = false;
                    flag_LogIn = false;
                }

                // Log out checking

                else if (flag_LogOut && message == "{\"Status\": \"1\"}")
                {
                    // Hide the logout button and show the login button
                    this.Dispatcher.Invoke(() =>
                    {
                        //MessageBox.Show("Logged out successfully");
                        messageTextBlock.Text = "";
                        LoginButton.Visibility = Visibility.Visible;
                        LogoutButton.Visibility = Visibility.Hidden;
                    });

                    // Reset the flag
                    flag_LogOut = false;

                    // Reset any other relevant variables
                    isUserLoggedIn = false;
                    currentUsername = "";
                }
                else if (!flag_LogOut && message == "{\"message\": \"The user already connected.\"}")
                {
                    this.Dispatcher.Invoke(() =>
                    {
                        MessageBox.Show("This user is already connected.");
                    });
                }
                else if (!flag_LogOut && message != "{\"Status\": \"1\"}" && flag_JoiningRoomButton == false && flag_LogIn)
                {
                    this.Dispatcher.Invoke(() =>
                    {
                        MessageBox.Show("Having issues with logging in");
                    });
                }
            }
            catch (Exception ex)
            {
                this.Dispatcher.Invoke(() =>
                {
                    MessageBox.Show($"Error receiving message: {ex.Message}");
                });
            }
        }

        private void SendMessage(string message)
        {
            try
            {
                byte[] buffer = Encoding.UTF8.GetBytes(message);
                stream.Write(buffer, 0, buffer.Length);
                //MessageBox.Show($"Me: {message}");

            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error sending message: {ex.Message}");
            }
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
                //MessageBox.Show($"Sent message with code {messageCode} and size {messageSize}");

            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error sending message: {ex.Message}");
            }
        }

        private void SignUpButton_Click(object sender, RoutedEventArgs e)
        {
            if (!isUserLoggedIn)
            {

                // Create an instance of the SignUpView
                SignUpView signUpView = new SignUpView();

                // Pass the server information to SignUpView
                signUpView.client = client;
                signUpView.stream = stream;

                // Add SignUpView to the mainGrid
                mainGrid.Children.Add(signUpView);

                // Remove MainWindow from the mainGrid
                mainGrid.Children.Remove(this);
            }
            else
            {
                MessageBox.Show("Already logged in, can't create account");
            }

        }

        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            if (isUserLoggedIn == true)
            {
                messageTextBlock.Text = "Already logged in.";
            } 
            else
            {
                // Get username and password from text boxes
                string username = usernameTextBox.Text;
                string password = passwordTextBox.Text;

                // Check if username or password is empty
                if (string.IsNullOrWhiteSpace(username) || string.IsNullOrWhiteSpace(password))
                {
                    MessageBox.Show("Please enter both username and password.");
                    return;
                }

                // Prepare login JSON message with username and password
                var loginJson = new { username = username, password = password };
                string jsonMessage = Newtonsoft.Json.JsonConvert.SerializeObject(loginJson);

                flag_LogIn = true;

                currentUsername = username;

                // Send login message
                SendUserMessage(101, jsonMessage);

                // Recve login message
                ReceiveLoginLogoutMessage();
            }
        }

        private void LogoutButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //MessageBox.Show("Pressed log out button");

                // Convert the message string to a byte array
                byte[] messageBytes = Encoding.UTF8.GetBytes("k");

                // Set the flag to indicate logout
                flag_LogOut = true;
                
                // Send the message to the server
                stream.Write(messageBytes, 0, messageBytes.Length);
                ReceiveLoginLogoutMessage();
            }
            catch (Exception ex)
            {
                // Handle any exceptions that occur during the write operation
                MessageBox.Show($"Error sending logout message: {ex.Message}");
            }
        }

        private void CreateRoom_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (isUserLoggedIn)
                {
                    CreateRoomView createRoomView = new CreateRoomView();

                    // Pass the server information to SignUpView
                    createRoomView.client = client;
                    createRoomView.stream = stream;

                    // Create a separate event handler for the RoomCreated event
                    EventHandler roomCreatedHandler = null;
                    roomCreatedHandler = (sender, args) =>
                    {
                        // Unsubscribe from the event to prevent memory leaks
                        createRoomView.RoomCreated -= roomCreatedHandler;

                        // Send user to room admin view
                        RoomAdminView roomAdminView = new RoomAdminView();
                        roomAdminView.viewedRoomName = createRoomView.createdRoomName;
                        roomAdminView.client = client;
                        roomAdminView.stream = stream;

                        // Create a separate event handler for the GameStarted event
                        EventHandler gameStartedHandler = null;
                        gameStartedHandler = (sender, args) =>
                        {
                            // Unsubscribe from the event to prevent memory leaks
                            roomAdminView.GameStarted -= gameStartedHandler;

                            // Send user to in-game view
                            InGameView gameUserView = new InGameView();
                            gameUserView.gameRoomName = roomAdminView.viewedRoomName;
                            gameUserView.gameRoomId = roomAdminView.currentRoomId;
                            gameUserView.client = client;
                            gameUserView.stream = stream;

                            // Set parameters given in create room
                            gameUserView.numOfQuestions = createRoomView.numOfQuestions;
                            gameUserView.timeForEachQuestion = createRoomView.timeForEachQuestion;

                            mainGrid.Children.Add(gameUserView);
                            mainGrid.Children.Remove(this);

                            gameUserView.getQuestions();
                        };

                        // Subscribe to the GameStarted event
                        roomAdminView.GameStarted += gameStartedHandler;

                        mainGrid.Children.Add(roomAdminView);
                        mainGrid.Children.Remove(this);

                        // Call the get rooms function
                        roomAdminView.GetRooms();
                    };

                    // Subscribe to the RoomCreated event
                    createRoomView.RoomCreated += roomCreatedHandler;

                    // Add CreateRoomView to the mainGrid
                    mainGrid.Children.Add(createRoomView);

                    // Remove MainWindow from the mainGrid
                    mainGrid.Children.Remove(this);
                }
                else
                {
                    MessageBox.Show("You're not logged in, can't create room.");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }


        private void JoinRoom_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (isUserLoggedIn)
                {
                    // Create an instance of JoinRoomView
                    JoinRoomView joinRoomView = new JoinRoomView();

                    // Set the client and stream properties
                    joinRoomView.client = client;
                    joinRoomView.stream = stream;

                    // Create a separate event handler for the RoomJoined event
                    EventHandler roomJoinedHandler = null;
                    roomJoinedHandler = (joinRoomSender, joinRoomArgs) =>
                    {
                        // Unsubscribe from the event to prevent memory leaks
                        joinRoomView.RoomJoined -= roomJoinedHandler;

                        // Send user to room user view
                        RoomUserView roomUserView = new RoomUserView();
                        roomUserView.joinedRoomName = joinRoomView.joinedRoomNameFinal;
                        roomUserView.joinedRoomId = joinRoomView.joinedRoomIdFinal;
                        roomUserView.client = client;
                        roomUserView.stream = stream;

                        // Create a separate event handler for the GameStarted event
                        EventHandler gameStartedHandler = null;
                        gameStartedHandler = (roomUserSender, roomUserArgs) =>
                        {
                            // Unsubscribe from the event to prevent memory leaks
                            roomUserView.GameStarted -= gameStartedHandler;

                            // Perform UI updates on the UI thread
                            roomUserView.Dispatcher.Invoke(() =>
                            {
                                // Send user to in-game view
                                InGameView gameUserView = new InGameView();
                                gameUserView.gameRoomName = roomUserView.joinedRoomName;
                                gameUserView.gameRoomId = roomUserView.joinedRoomId;
                                gameUserView.client = client;
                                gameUserView.stream = stream;

                                // Assign game statistics
                                gameUserView.numOfQuestions = roomUserView.numOfQuestions;
                                gameUserView.timeForEachQuestion = roomUserView.timeForEachQuestion;

                                mainGrid.Children.Add(gameUserView);
                                mainGrid.Children.Remove(roomUserView);

                                // Call the get users in room function
                                gameUserView.getQuestions();
                            });
                        };

                        // Subscribe to the GameStarted event
                        roomUserView.GameStarted += gameStartedHandler;

                        mainGrid.Children.Add(roomUserView);
                        mainGrid.Children.Remove(this);

                        // Call the get users in room function
                        roomUserView.GetUsersInRoom();
                    };

                    // Subscribe to the RoomJoined event
                    joinRoomView.RoomJoined += roomJoinedHandler;

                    // Add JoinRoomView to the mainGrid
                    mainGrid.Children.Add(joinRoomView);

                    // Remove MainWindow from the mainGrid
                    mainGrid.Children.Remove(this);

                    flag_JoiningRoomButton = true;

                    // Now call the getRooms() method
                    joinRoomView.getRooms();
                }
                else
                {
                    MessageBox.Show("You're not logged in, can't join room.");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        private void MyStatistics_Click(object sender, RoutedEventArgs e)
        {
            if(isUserLoggedIn == true) 
            {
                var statisticsView = new StatisticsView();

                // Pass the server information to StatisticsView
                statisticsView.client = client;
                statisticsView.stream = stream;

                // Add the StatisticsView to the MainGrid
                mainGrid.Children.Add(statisticsView);

                statisticsView.GetUserStatistics();
            }
            else
            {
                MessageBox.Show("You're not logged in.");
            }
        }

        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            // Log out
            LogoutButton_Click(sender, e);
            // Close the application
            Application.Current.Shutdown();
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                // Log out
                LogoutButton_Click(sender, new RoutedEventArgs());

                receiveThread?.Abort();
                stream?.Close();
                client?.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error closing connection: {ex.Message}");
            }
        }
    }
}

