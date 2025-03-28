using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
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

namespace TriviaClientGUI
{
    /// <summary>
    /// Interaction logic for StatisticsView.xaml
    /// </summary>
    public partial class StatisticsView : UserControl
    {
        public StatisticsView()
        {
            InitializeComponent();
        }
        public TcpClient client { get; set; }
        public NetworkStream stream { get; set; }

        private bool flag_AskedForStatistics;

        public void GetUserStatistics()
        {
            try
            {
                // Convert the message string to a byte array
                byte[] messageBytes = Encoding.UTF8.GetBytes("l");

                // Send the message to the server
                stream.Write(messageBytes, 0, messageBytes.Length);
                flag_AskedForStatistics = true;
                ReceieveUserStatistics();
            }
            catch (Exception ex)
            {
                // Handle any exceptions that occur during the write operation
                MessageBox.Show($"Error sending logout message: {ex.Message}");
            }
        }

        public void ReceieveUserStatistics()
        {
            try
            {
                while (flag_AskedForStatistics)
                {
                    byte[] buffer = new byte[1024];
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0)
                    {
                        break;
                    }
                    string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                    // Update the text boxes
                    UpdateStatistics(message);
                    
                    // Turn of the flag
                    flag_AskedForStatistics = false;
                }
                
            }
            catch (Exception ex )
            {
                MessageBox.Show(ex.Message);
            }
        }

        public void UpdateStatistics(string jsonMessage)
        {
            try
            {
                // Parse the JSON message
                JObject json = JObject.Parse(jsonMessage);
                string status = json["Status"].ToString();

                if (status == "1")
                {
                    string personalStats = json["PersonalStats"].ToString();

                    // Split the personalStats string by commas
                    var stats = personalStats.Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

                    if (stats.Length >= 4)
                    {
                        // Update the TextBlocks with the corresponding values
                        TotalNumText.Text = stats[0];
                        AvgTimeText.Text = stats[1];
                        CorrectNumText.Text = stats[2];

                        // Calculate the number of wrong answers
                        int totalAnswers = int.Parse(stats[3]);
                        int correctAnswers = int.Parse(stats[2]);
                        int wrongAnswers = totalAnswers - correctAnswers;
                        WrongNumText.Text = wrongAnswers.ToString();
                    }
                }
                else
                {
                    MessageBox.Show("Failed to retrieve statistics.");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error parsing statistics: {ex.Message}");
            }
        }

        private void ReturnToMenu_Click(object sender, RoutedEventArgs e)
        {
            var parentGrid = this.Parent as Grid;
            if (parentGrid != null)
            {
                parentGrid.Children.Remove(this);
            }
        }
    }
}
