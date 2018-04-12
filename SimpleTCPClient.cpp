#include "commonutils.h"
#include <string>
#include <sstream>
#include <vector>

using namespace std;
int Menu();

void inputArray(vector<float>& v){
    int v_size = 0;
    float value;
    v.clear();
    cout << "Array size: ";
    cin >> v_size;
    cout << endl;
    for(unsigned i = 1; i <= v_size; i++){
        cout << "Array[" << i << "]: ";
        cin >> value;
        cout << endl;
        v.push_back(value);
    }
}

string Arr2Str(vector<float>& v){
    std::ostringstream out;
    for (unsigned i=0; i < v.size(); i++){
        out << v[i] << ' ';
    }
    string str(out.str());
    return str;
}

void waitOK(SOCKET socket){
    char recvBuffer[BUFFER_SIZE];
    do{
		int nRec = recv(socket, recvBuffer, BUFFER_SIZE,0);
        recvBuffer[nRec]='\0';
    }while(strcmp (recvBuffer, "OK") != 0);
}

int sendData(SOCKET socket, const char* data){
    if (send(socket, data, strlen(data), 0) < 0) {
        closeSocket(socket);
        return socketError(true, "sending to server", true);
    }
}

string getValue(SOCKET socket, const char* attr){
    sendData(socket, attr);
    char recvBuffer[256];
    int nRec = recv(socket, recvBuffer, 256,0);
    recvBuffer[nRec]='\0';
    return recvBuffer;
}

int main(int argc,char* argv[]) { //Буфер повідомлень
    vector<float> vec;
    string strArr = "";
    bool cycle = true;
	char dataBuffer[BUFFER_SIZE];
	SOCKET clientSocket;
	struct sockaddr_in serverAddr;
	char serverName[256];

	//strcpy(serverName,"192.168.199.1");
	strcpy(serverName,"localhost");
	int nPort = 5150;

    //Отримуємо сервер та порт підключення з командного рядка
	//TODO: write this func in commonutils.h
	//getServerPort(argv,argc,"–server",serverName,nPort);
	memset(&serverAddr,0,sizeof(serverAddr));

	if (initSocketAPI()) {
		return socketError(true,"init socket API");
	}

    //Створюємо активний сокет
	if ((clientSocket=socket(AF_INET,SOCK_STREAM, IPPROTO_TCP))<=0) {
		socketError(true,"create socket",true);
		return -2;
	}

    //Заповнюємо структуру serverAddr для зв’язування
    //функцією bind
	serverAddr.sin_family = AF_INET;
    //Переходимо до мережевого порядку байт
	serverAddr.sin_port = htons(nPort); //Вказуємо адресу серверу

	struct hostent* hptr;

	hptr = gethostbyname(serverName);
	memcpy(&serverAddr.sin_addr.s_addr, hptr->h_addr_list[0],hptr->h_length);

	if (connect(clientSocket,(struct sockaddr*)&serverAddr, sizeof(serverAddr))!=0) {
		closeSocket(clientSocket);
		char c;
		cin.get(c);
		return socketError(true,"connect to server",true);
	}

	printInfo(argv[0],NULL,false);

	cout<<"Connected to server: "<<serverName<<": ";
	cout<<nPort<<endl;

	do {
        switch(Menu()){
            case 1:
                inputArray(vec);
                sendData(clientSocket, "Array");
                waitOK(clientSocket);

                strArr = Arr2Str(vec);
                sendData(clientSocket, strArr.c_str());
                waitOK(clientSocket);
                break;
            case 2:
                cout << "Array: " << ((strArr.empty()) ? "empty, try input array" : strArr) << endl;
                break;
            case 3:
                cout << "Min value: " << ((strArr.empty()) ? "can't get min value of empty array" : getValue(clientSocket, "getMin")) << endl;
                break;
            case 4:
                cout << "Max value: " << ((strArr.empty()) ? "can't get max value of empty array" : getValue(clientSocket, "getMax")) << endl;
                break;
            case 5:
                cout << "Avg value: " << ((strArr.empty()) ? "can't get avg value of empty array" : getValue(clientSocket, "getAvg")) << endl;
                break;
            default:
                sendData(clientSocket, "bye");
                cycle = false;
                break;
        }

	} while (cycle);

    //Закриваємо сокет і звільняємо системні ресурси
	closeSocket(clientSocket);
	deinitSocketAPI();
	return 0;
}

int Menu(){
    cout << endl;
    cout << "1 - Input Array" << endl;
    cout << "2 - Print Array" << endl;
    cout << "3 - Get Min Value" << endl;
    cout << "4 - Get Max Value" << endl;
    cout << "5 - Get Avg Value" << endl;
    cout << "6 - Close App" << endl;
    cout << "Your choice: ";

    int choice;
    cin >> choice;
    while (choice <= 0 || choice > 6){
        cout << endl << "Wrong Input! Try Again: " << endl;
        cin >> choice;
    }
    cout << endl;
    return choice;
}
