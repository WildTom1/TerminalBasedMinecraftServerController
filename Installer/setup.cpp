#include <iostream>
#include <filesystem>
#include <fstream>
using namespace std;

//This will make and do first setup with the config file

string serverName = "myserver";
string serverPath = " ";
string backupPath = " ";
string pBackupPath = " ";
string amountOfRam = "4";
string jarName = "server.jar";
string user = getenv("USER");

string exec(string command) {//taken from https://www.tutorialspoint.com/How-to-execute-a-command-and-get-the-output-of-command-within-Cplusplus-using-POSIX
   char buffer[128];
   string result = "";

   // Open pipe to file
   FILE* pipe = popen(command.c_str(), "r");
   if (!pipe) {
      return "popen failed!";
   }

   // read till end of process:
   while (!feof(pipe)) {

      // use buffer to read and add to result
      if (fgets(buffer, 128, pipe) != NULL)
         result += buffer;
   }

   pclose(pipe);
   return result;
}

int main(){
    cout << "This is the setup for the Server Control Program" << endl;
    cout << "What is the name of the screen?" << endl;
    cin >> serverName;
    cout << "What is the path of the server?" << endl;
    cin >> serverPath;
    cout << "What is the backup Path?" << endl;
    cin >> backupPath;
    cout << "What is the playerdata backup path?" << endl;
    cin >> pBackupPath;
    cout << "What is the amount of ram for the server?" << endl;
    cin >> amountOfRam;
    cout << "What is the name of the jar?" << endl;
    cin >> jarName;
    cout << "Writing config file";
    exec("rm /home/" + user + "/.config/serverControl.cfg");//ensures that there is no previous config file
    ofstream configFile("/home/" + user + "/.config/serverControl.cfg");
    configFile<<"programVersion = 1"<<endl;//This is for if I update the proram
    configFile<<"screenName = " + serverName<<endl;
    configFile<<"serverPath = " + serverPath<<endl;
    configFile<<"backupPath = " + backupPath<<endl;
    configFile<<"playerdataBackupPath = " + pBackupPath<<endl;
    configFile<<"amountOfRam = " + amountOfRam<<endl;
    configFile<<"jarName = " + jarName<<endl;
    configFile.close(); 
}