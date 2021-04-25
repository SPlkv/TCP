using System;
using System.Collections;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Windows.Forms;

namespace TCPserver
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

        }
        private ArrayList nSockets;   //используется динамический массив, т.к. неизвестно количество подключений

        private void Form1_Load(object sender, EventArgs e)  //запуск потока, который ожидает подключения
        {
            const string ip = "127.0.0.1";
            lblStatus.Text = "IP адрес сервера" + IPAddress.Parse(ip);
            nSockets = new ArrayList();
            Thread thdListener = new Thread(new ThreadStart(listenerThread));              //выполнение listenerThread в новом потоке
            thdListener.Start();
        }
        public void listenerThread()
        {
            //Слушает подключения от сетевых клиентов TCP.
            TcpListener tcpListener = new TcpListener(IPAddress.Any, 8080);
            tcpListener.Start();
            while (true)
            {
                Socket handlerSocket = tcpListener.AcceptSocket();
                if (handlerSocket.Connected)           //если подключение успешно
                {                 
                    lbConnections.Items.Add(handlerSocket.RemoteEndPoint.ToString() + " подключено");  
                    nSockets.Add(handlerSocket);
                    ThreadStart thdstHandler = new ThreadStart(handlerThread);       //поток сервера
                    Thread thdHandler = new Thread(thdstHandler);
                    thdHandler.Start();
                }
            }
        }

       public void handlerThread()
       {
                Socket handlerSocket = (Socket)nSockets[nSockets.Count - 1];
                NetworkStream networkStream = new NetworkStream(handlerSocket);
                int thisRead = 0;
                int blockSize = 1024;

                byte[] dataByte = new byte[blockSize];           //буффер

                // Только один процесс может получить доступ к одному и тому же файлу
                Stream fileStream = File.OpenWrite("C:\\Users\\User\\Desktop\\test.txt");
                while (true)
                {
                    thisRead = networkStream.Read(dataByte, 0, blockSize);         //получение байтов
                    fileStream.Write(dataByte, 0, thisRead);                       //Отправление ответа
                    if (thisRead == 0)                                             //если буффер пуст, то заканчиваем
                        break;
                }             
                fileStream.Close();

                lbConnections.Items.Add("Файл передан");
                handlerSocket = null;
       }      
    }
}
