using Newtonsoft.Json.Linq;
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
    public partial class InGameView : UserControl
    {
        public TcpClient client { get; set; }
        public NetworkStream stream { get; set; }

        public int timeForEachQuestion;
        public int numOfQuestions;

        public string gameRoomName;
        public int gameRoomId;
        public bool flag_AskedForQuestion;
        public bool flag_ExitRoom;
        public bool flag_ReceieveAnswer;
        public bool flag_AskedForGameResults;
        public bool flag_WaitingForPlayers;
        public bool flag_CountdownActive;

        public int correctAnswersDisplay;
        public int totalAnswersDisplay;

        Thread thread_GameOver;
        Thread thread_Countdown;

        int remainingTime;
        public bool isTimerStarted;

        public InGameView()
        {
            // Ensure that the control is created on the STA thread
            if (!CheckAccess())
            {
                // If not on the STA thread, use Dispatcher to create the control on the STA thread
                Dispatcher.Invoke(() => InitializeComponent());
            }
            else
            {
                // If already on the STA thread, initialize the control directly
                InitializeComponent();
            }
        }

        public void getQuestions()
        {
            try
            {
                //MessageBox.Show("Sending message t");
                // Convert the message string to a byte array
                byte[] messageBytes = Encoding.UTF8.GetBytes("t");

                // Send the message to the server
                stream.Write(messageBytes, 0, messageBytes.Length);
                //MessageBox.Show("Sending get rooms message");

                flag_AskedForQuestion = true;
                ReceiveGetQuestionMessages();
            }
            catch (Exception ex)
            {
                this.Dispatcher.Invoke(() =>
                {
                    // Handle any exceptions that occur during the write operation
                    MessageBox.Show($"Error sending get questions message: {ex.Message}");
                });
            }
        }

        // CurrentQuestion
        // button_1_correct button_2_correct button_3_correct button_4_correct
        public void ReceiveGetQuestionMessages()
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (flag_AskedForQuestion)
                {
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0)
                    {
                        break;
                    }

                    // Get message into string
                    string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                    //MessageBox.Show(message);

                    // Parse the JSON message
                    JObject jsonMessage = JObject.Parse(message);
                    string question = jsonMessage["Question"].ToString();
                    string answers = jsonMessage["Answers"].ToString();
                    string status = jsonMessage["Status"].ToString();
                    
                    if(status == "0")
                    {
                        GameOver();
                        break;
                    }

                    // Split answers by comma and trim any extra whitespace
                    var answerList = answers.Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries)
                                            .Select(answer => answer.Trim())
                                            .ToArray();

                    // Ensure there are exactly 4 answers
                    if (answerList.Length != 4)
                    {
                        throw new InvalidOperationException("The number of answers is not exactly 4.");
                    }

                    Random random = new Random();

                    // Shuffle the list of vertical margins
                    List<int> margins = new List<int> { 50, 100, 150, 200 };
                    margins = margins.OrderBy(x => random.Next()).ToList();

                    this.Dispatcher.Invoke(() =>
                    {
                        // Update the UI elements
                        CurrentQuestion.Text = question;

                        // Assign randomized margins to buttons
                        button_1_correct.Margin = new Thickness(0, 0, 0, margins[0]);
                        button_2_correct.Margin = new Thickness(0, 0, 0, margins[1]);
                        button_3_correct.Margin = new Thickness(0, 0, 0, margins[2]);
                        button_4_correct.Margin = new Thickness(0, 0, 0, margins[3]);

                        button_1_correct.Content = answerList[0];
                        button_2_correct.Content = answerList[1];
                        button_3_correct.Content = answerList[2];
                        button_4_correct.Content = answerList[3];
                    });

                    flag_AskedForQuestion = false;

                    if (!flag_CountdownActive)
                    {
                        // Start the timer
                        StartCountdown(timeForEachQuestion);
                    }
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

        public void ReceiveAnswerCheck()
        {
            try
            {
                string checkStatus = "";

                byte[] buffer = new byte[1024];
                while (flag_ReceieveAnswer)
                {
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0)
                    {
                        break;
                    }

                    // Get message into string
                    string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                    // For debugging
                    //MessageBox.Show(message);

                    checkStatus = message;

                    flag_ReceieveAnswer = false;
                }

                if (checkStatus == "{\"Status\": \"1\"}")
                {
                    getQuestions();
                    CurrentScore.Text = ("Score: " + correctAnswersDisplay + "/" + numOfQuestions);
                }
                else if (checkStatus == "{\"Status\": \"0\"}")
                {
                    // Game ended, wait for other players
                    GameOver();
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error getting answer: {ex.Message}");
            }
        }

        // The game over function will trigger the game over scene, displaying the game results.
        public void GameOver()
        {
            try {

                StopCountdown();

                // Reset remaining time
                remainingTime = timeForEachQuestion;

                // Hide all game assets
                this.Dispatcher.Invoke(() =>
                {
                    CurrentQuestion.Visibility = Visibility.Hidden;
                    button_1_correct.Visibility = Visibility.Hidden;
                    button_2_correct.Visibility = Visibility.Hidden;
                    button_3_correct.Visibility = Visibility.Hidden;
                    button_4_correct.Visibility = Visibility.Hidden;
                    CurrentScore.Visibility = Visibility.Hidden;

                    // Rename text
                    TopText.Text = "Game over!";
                    TopText.FontSize = 30;
                });

                // Check if game results were requested or not
                if (!flag_AskedForGameResults) 
                {
                    flag_AskedForGameResults = true;

                    // Start a thread to get game results
                    thread_GameOver = new Thread(GetGameResultsMessage);
                    thread_GameOver.Start();
                }

            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        public void GetGameResultsMessage()
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (flag_AskedForGameResults)
                {
                    // Show waiting message
                    this.Dispatcher.Invoke(() =>
                    {
                        WaitingMessage.Visibility = Visibility.Visible;
                    });

                    // Send game results request
                    // Convert the message string to a byte array
                    byte[] messageBytes = Encoding.UTF8.GetBytes("u");

                    // Send the message to the server
                    stream.Write(messageBytes, 0, messageBytes.Length);

                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0)
                    {
                        break;
                    }

                    // Get message into string
                    string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);


                    // Not all players finished, wait for others
                    if (message != "{\"Status\": \"0\", \"Results\": \"\"}")
                    {
                        // Hide waiting message
                        this.Dispatcher.Invoke(() =>
                        {
                            WaitingMessage.Visibility = Visibility.Hidden;
                        });

                        UpdateLeaderboard(message);
                        flag_AskedForGameResults = false;
                    }
                    else // if recving nothinig
                    {
                        Thread.Sleep(1500); // sleep for 1.5 second
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error getting results: {ex.Message}");
            }
        }

        private void StartCountdown(int timeForEachQuestion)
        {
            try
            {
                flag_CountdownActive = true;
                TimerTextBlock.Visibility = Visibility.Visible;

                if (thread_Countdown == null || !thread_Countdown.IsAlive)
                {
                    thread_Countdown = new Thread(() => CountdownLoop(timeForEachQuestion));
                    thread_Countdown.Start();
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            
        }
        private void CountdownLoop(int timeForEachQuestion)
        {
            try {
                if (flag_CountdownActive)
                {
                    for (remainingTime = timeForEachQuestion; remainingTime >= 0; remainingTime--)
                    {
                        if (flag_CountdownActive == false)
                        {
                            // Stop the timer
                            break;
                        }

                        this.Dispatcher.Invoke(() =>
                        {
                            TimerTextBlock.Text = $"Time left: {remainingTime} seconds";
                        });

                        if (remainingTime == 0)
                        {
                            this.Dispatcher.Invoke(() =>
                            {
                                IncorrectAnswerClick(this, null);
                                remainingTime = timeForEachQuestion + 1;
                            });
                        }

                        Thread.Sleep(1000);
                    }
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void StopCountdown()
        {
            try
            {
                flag_CountdownActive = false;
                TimerTextBlock.Visibility = Visibility.Hidden;

                if (thread_Countdown != null && thread_Countdown.IsAlive)
                {
                    flag_CountdownActive = false;
                    //thread_Countdown.();
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        public void UpdateLeaderboard(string message)
        {
            try
            {
                // Parse the JSON message
                JObject jsonMessage = JObject.Parse(message);
                string results = jsonMessage["Results"].ToString();

                // Split results by dot
                var users = results.Split(new[] { '.' }, StringSplitOptions.RemoveEmptyEntries);

                // Clear existing leaderboard entries
                this.Dispatcher.Invoke(() => LeaderboardStackPanel.Children.Clear());

                foreach (var user in users)
                {
                    // Split user details by comma
                    var userDetails = user.Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

                    if (userDetails.Length == 4)
                    {
                        string username = userDetails[0];
                        string correctAnswers = userDetails[1];
                        string incorrectAnswers = userDetails[2];
                        string averageTime = userDetails[3];

                        string leaderboardEntry = $"{username}: Correct Answers: {correctAnswers}, Incorrect Answers: {incorrectAnswers}, Average Time: {averageTime}";

                        this.Dispatcher.Invoke(() =>
                        {
                            // Create a TextBlock for the leaderboard entry
                            TextBlock leaderboardTextBlock = new TextBlock
                            {
                                Text = leaderboardEntry,
                                FontSize = 16,
                                Margin = new Thickness(0, 10, 0, 10)
                            };

                            leaderboardTextBlock.TextAlignment = TextAlignment.Center;

                            // Add the TextBlock to the StackPanel
                            LeaderboardStackPanel.Children.Add(leaderboardTextBlock);

                            // Add a line separator
                            Line separator = new Line
                            {
                                Stroke = Brushes.Black,
                                X1 = 0,
                                X2 = 665, // Width of the StackPanel or UserControl
                                StrokeThickness = 1,
                                Margin = new Thickness(0, 0, 0, 10)
                            };
                            LeaderboardStackPanel.Children.Add(separator);
                        });
                    }
                    else
                    {
                        throw new InvalidOperationException("The number of user details is not exactly 4.");
                    }
                }
            }
            catch (Exception ex)
            {
                this.Dispatcher.Invoke(() =>
                {
                    MessageBox.Show($"Error updating leaderboard: {ex.Message}");
                });
            }
        }

        public void CorrectAnswerClick(object sender, RoutedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                //MessageBox.Show("Correct Answer!");
            });

            // Answer id 1 is always correct

            // TODO: ADD TIMER THREAD TO COUNT ANSWER TIME
            // Prepare answer JSON message with username and password
            var answerJson = new { AnswerID = 1, AnswerTimeSeconds = (timeForEachQuestion-remainingTime) };
            string jsonMessage = Newtonsoft.Json.JsonConvert.SerializeObject(answerJson);

            // Send message
            SendUserMessage(114, jsonMessage);

            // Add points for display
            correctAnswersDisplay++;

            // Reset timer
            remainingTime = (timeForEachQuestion + 1);

            flag_ReceieveAnswer = true;
            ReceiveAnswerCheck();
        }

        public void IncorrectAnswerClick(object sender, RoutedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                //MessageBox.Show("Incorrect Answer...");            
            });

            // Answer id 2 is always incorrect

            // TODO: ADD TIMER THREAD TO COUNT ANSWER TIME
            // Prepare answer JSON message with username and password
            var answerJson = new { AnswerID = 2, AnswerTimeSeconds = (timeForEachQuestion - remainingTime) };
            string jsonMessage = Newtonsoft.Json.JsonConvert.SerializeObject(answerJson);

            // Send message
            SendUserMessage(114, jsonMessage);

            // Reset timer
            remainingTime = (timeForEachQuestion + 1);

            flag_ReceieveAnswer = true;
            ReceiveAnswerCheck();
        }

        public void ReceieveExitRoomMessage()
        {
            try
            {
                byte[] buffer = new byte[1024];
                while (flag_ExitRoom)
                {
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0)
                    {
                        break;
                    }

                    // Get message into string
                    string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                    if( message == "{\"Status\": \"1\"}")
                    {
                        flag_ExitRoom = false;

                        this.Dispatcher.Invoke(() =>
                        {
                            // Remove the view from its parent container
                            var parentGrid = this.Parent as Grid;
                            if (parentGrid != null)
                            {
                                parentGrid.Children.Remove(this);
                            }
                        });
                    }
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

        public void ExitGame(object sender, RoutedEventArgs e)
        {
            try
            {
                // Send 115 in ASCII which means leave game
                // Convert the message string to a byte array
                byte[] messageBytes = Encoding.UTF8.GetBytes("s");

                // Send the message to the server
                stream.Write(messageBytes, 0, messageBytes.Length);
                //MessageBox.Show("Sending get rooms message");

                flag_ExitRoom = true;
                ReceieveExitRoomMessage();
            }
            catch (Exception ex)
            {
                this.Dispatcher.Invoke(() =>
                {
                    // Handle any exceptions that occur during the write operation
                    MessageBox.Show($"Error sending get questions message: {ex.Message}");
                });
            }
        }
    }
}
