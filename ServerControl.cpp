#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <termios.h>
#include <filesystem>
#include <string>
#include <fstream>
using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

string serverName = "myserver";
string serverPath = " ";
string backupPath = " ";
string pBackupPath = " ";
string amountOfRam = "4";
string jarName = "server.jar";
int lineCount = 50;
string lineBuffer[1000];//This is set to a high number so that there is some overflow and also because I don't know how to reset an array
bool doingWork = false;
string user = getenv("USER");//get username code from: https://gist.github.com/krishna0512/5e5f8761a24ea850f0bd

string color_Off="\033[0m";
string red="\033[0;31m";
string green="\033[0;32m";

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

void sleep(int timeToSleep){
    sleep_for(milliseconds(timeToSleep));
}

bool serverOn(){
    string output = exec("screen -list");
    //cout << output << endl;
    if(output.find(serverName) != std::string::npos){
        return true;
    }
    return false;
}

void refreshScreen(){
    system("clear");
    for(int i=0;i<lineCount-1;i++){
        cout << lineBuffer[i] << endl;
    }
}

void setServerStatus(){
    if(serverOn()){
        lineBuffer[4] = "Server Status: " + green + "Online" + color_Off;
    } else{
        lineBuffer[4] = "Server Status: " + red + "Offline" + color_Off;
    }
}

void setTop(){
    lineBuffer[0] = "(1) Start Server (2) Stop Server (3) Restart Server (4) Make Backup (5) Delete Most Backups";
    lineBuffer[1] = "(6) Restore Last Backup (7) Kick all Players (8)Edit config Values";
    lineBuffer[2] = "(q) Quits the Program (c) Clears the Log";
    setServerStatus();
}

//The next 4 functions are from https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux
//These handle getting user input
static struct termios old, current;
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  current = old; /* make new settings same as old settings */
  current.c_lflag &= ~ICANON; /* disable buffered i/o */
  if (echo) {
      current.c_lflag |= ECHO; /* set echo mode */
  } else {
      current.c_lflag &= ~ECHO; /* set no echo mode */
  }
  tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

void waiting(int logLine){//Playes the animation
    lineBuffer[logLine] = '|';
    refreshScreen();
    sleep(500);
    lineBuffer[logLine] = '/';
    refreshScreen();
    sleep(500);
    lineBuffer[logLine] = '-';
    refreshScreen();
    sleep(500);
    lineBuffer[logLine] = '\\';
    refreshScreen();
    sleep(500);
}

void waitingWithMessage(int logLine, string message){//Allows the animation to have a message
    lineBuffer[logLine] = message + " " + '|';
    refreshScreen();
    sleep(500);
    lineBuffer[logLine] = message + " " + '/';
    refreshScreen();
    sleep(500);
    lineBuffer[logLine] = message + " " + '-';
    refreshScreen();
    sleep(500);
    lineBuffer[logLine] = message + " " + '\\';
    refreshScreen();
    sleep(500);
}

void indefWaiting(int logLine){//Allows for long waiting without a loop in the main
    while(doingWork){
        waiting(logLine);
    }
    return;
}

void indefWaitingWithMessage(int logLine, string message){//allows long waiting with a message
    while(doingWork){
        waitingWithMessage(logLine, message);
    }
    return;
}

void partialRefresh(){
    system("clear");
    for(int i=0;i<8;i++){
        cout << lineBuffer[i] << endl;
    }
}

void changeMenuReset(){
    lineBuffer[0] = "(1)Server Jar Name: " + jarName;
    lineBuffer[1] = "(2)Server Path: " + serverPath;
    lineBuffer[2] = "(3)Amount of Ram: " + amountOfRam;
    lineBuffer[3] = "(4)Backup Path: " + backupPath;
    lineBuffer[4] = "(5)Playerdata Backup Path: " + pBackupPath;
    lineBuffer[5] = "(b)Return to previous menu";
}

void changeConfig(){//This is used to change the config file
    for(int i=0;i<lineCount-1;i++){//clears out the screen
            lineBuffer[i] = " ";
    }
    changeMenuReset();
    char c;
    while(c!='b'){
        changeMenuReset();
        partialRefresh();
        c = getch();
        resetTermios();
        if(c=='1'){
            lineBuffer[7] = "Please type in the name of the server jar";
            partialRefresh();
            cin >> jarName;
            lineBuffer[7] = "Done!";
        } else if(c=='2'){
            lineBuffer[7] = "Please type in the path of the server";
            partialRefresh();
            cin >> serverPath;
            lineBuffer[7] = "Done!";
        } else if(c=='3'){
            lineBuffer[7] = "Please type in the Amount of Ram for the server";
            partialRefresh();
            cin >> amountOfRam;
            lineBuffer[7] = "Done!";
        } else if(c=='4'){
            lineBuffer[7] = "Please type in the backup path for the server";
            partialRefresh();
            cin >> backupPath;
            lineBuffer[7] = "Done!";
        } else if(c=='5'){
            lineBuffer[7] = "Please type in the playerdata backup path for the server";
            partialRefresh();
            cin >> pBackupPath;
            lineBuffer[7] = "Done!";
        }
    }
    lineBuffer[7] = "Writing Changes to the config File";
    ifstream configFile("/home/" + user + "/.config/serverControl.cfg");
    ofstream outConfigFile("temp.txt");
    if(!configFile.is_open()){
        cout << "Could not open the config file\nExit code 1" << endl;
        exit;
    }
    string configLine;
    while (getline(configFile, configLine)) {//gets the needed info from the config file
        if(configLine.find("screenName") != std::string::npos){
            outConfigFile<<"screenName = " + serverName<<endl;
        } else if(configLine.find("serverPath") != std::string::npos){
            outConfigFile<<"serverPath = " + serverPath<<endl;
        } else if(configLine.find("backupPath") != std::string::npos){
            outConfigFile<<"backupPath = " + backupPath<<endl;
        } else if(configLine.find("playerdataBackupPath") != std::string::npos){
            outConfigFile<<"playerdataBackupPath = " + pBackupPath<<endl;
        } else if(configLine.find("amountOfRam") != std::string::npos){
            outConfigFile<<"amountOfRam = " + amountOfRam<<endl;
        } else if(configLine.find("jarName") != std::string::npos){
            outConfigFile<<"jarName = " + jarName<<endl;
        } else{
            outConfigFile<<configLine<<endl;
        }
    } 
    configFile.close(); 
    outConfigFile.close();
    exec("rm /home/" + user + "/.config/serverControl.cfg");
    exec("mv temp.txt /home/" + user + "/.config/serverControl.cfg");
    for(int i=0;i<lineCount-1;i++){//clears out the screen
            lineBuffer[i] = " ";
    }
    setTop();
    return;
}

int main(){
    ifstream configFile("/home/" + user + "/.config/serverControl.cfg");
    if(!configFile.is_open()){
        cout << "Could not open the config file\nExit code 1" << endl;
        return 1;
    }
    string configLine;
    while (getline(configFile, configLine)) {//gets the needed info from the config file
        if(configLine.find("screenName") != std::string::npos){
            serverName = configLine.substr(13);
        } else if(configLine.find("serverPath") != std::string::npos){
            serverPath = configLine.substr(13);
        } else if(configLine.find("backupPath") != std::string::npos){
            backupPath = configLine.substr(13);
        } else if(configLine.find("playerdataBackupPath") != std::string::npos){
            pBackupPath = configLine.substr(23);
        } else if(configLine.find("amountOfRam") != std::string::npos){
            amountOfRam = configLine.substr(14);
        } else if(configLine.find("jarName") != std::string::npos){
            jarName = configLine.substr(10);
        }
    } 
    configFile.close(); 
    //return 0;
    system("tput smcup");
    system("clear");
    lineCount = stoi(exec("tput lines"));
    for(int i=0;i<lineCount+1;i++){
        lineBuffer[i] = " ";
    }
    setTop();
    bool isRunning = true;
    char c = ' ';
    int logLine = 6;
    string tempstr;
    while(isRunning){
        setServerStatus();
        refreshScreen();
        c = getch();
        if(logLine == lineCount-1){
            for(int i=6;i<lineCount;i++){
            lineBuffer[i] = lineBuffer[i+1];
        }
        }
        if(c == 'q'){//quits the loop
            isRunning = false;
            break;
        } else if(c=='1'){//starts the server
            if(!serverOn()){
                lineBuffer[logLine] = serverPath + " hehe";
                refreshScreen();
                std::filesystem::current_path(serverPath);
                //exec("cd " + serverPath);
                exec("screen -dmS " + serverName + " java -Xmx" + amountOfRam + "G -jar " + serverPath + "/" + jarName + " nogui");
                while(!serverOn()){
                   waitingWithMessage(logLine, "Starting Server");
                }
                lineBuffer[logLine] = green + "Server Online" + color_Off;
            } else{
                lineBuffer[logLine] = "The server is already " + green + "Online" + color_Off;
            }
        } else if(c=='2'){//stops the server
            if(serverOn()){
                exec("screen -S " + serverName + " -p 0 -X stuff 'kick @a Stopping Server^M'");//Kicks all players first
                exec("screen -S " + serverName + " -p 0 -X stuff 'stop^M'");//Sends the stop command to the server
                while(serverOn()){
                    waitingWithMessage(logLine, "Stoping Server");
                }
                lineBuffer[logLine] = red + "Server Offline" + color_Off;
            } else{
                lineBuffer[logLine] = "The server is already " + red + "Offline" + color_Off;
            }
        } else if(c=='3'){//restarts the server
            if(serverOn()){
                exec("screen -S myserver -p 0 -X stuff 'kick @a Restarting Server^M'");
                exec("screen -S myserver -p 0 -X stuff 'stop^M'");
                while(serverOn()){
                    waitingWithMessage(logLine, "Restarting Server");
                }//the server should now be restarted
                std::filesystem::current_path(serverPath);
                exec("screen -dmS " + serverName + " java -Xmx" + amountOfRam + "G -jar " + serverPath + "/" + jarName + " nogui");
                while(!serverOn()){
                    waitingWithMessage(logLine, "Restarting Server");
                }
                lineBuffer[logLine] = "The server has been restarted";
                
            } else{
                lineBuffer[logLine] = "The server is not on, please start it before restarting it";
            }
        } else if(c=='4'){//makes the server backup
            std::filesystem::current_path(serverPath + "world/");
            exec("cp playerdata/* " +  pBackupPath);
            doingWork = true;
            std::thread backupAnimation(indefWaitingWithMessage, logLine, "Making Backup, Please Wait");//Runs a thread to keep the animation
            exec("zip -r -1 -q " + backupPath + "$(date +\"%Y-%m-%d:%H:%M:%S\").zip *");
            doingWork = false;
            backupAnimation.join();//If this is not here, the program will crash, I don't know why
            lineBuffer[logLine] = "Backup Created";
            
        } else if(c=='5'){//deletes most backups
            lineBuffer[logLine] = "Are you sure that you want to delete most of the Backups? (y/n)";
            refreshScreen();
            c = getch();
            if(c == 'y'){
                std::filesystem::current_path(backupPath);
                string latest = exec("ls -Art | tail -n 1");//gets the latest backup file
                latest = latest.substr(0, latest.length()-1);//gets rid of the new line that is there for no reason
                exec("mv " + latest + " ../");
                exec("rm *");
                exec("mv ../" + latest + " " + backupPath);
                lineBuffer[logLine] = "Deleted most Backups";
            } else{
                lineBuffer[logLine] = "The Backups will not be deleted";
            }
        } else if(c=='6'){//Restores last backup
            if(!serverOn()){
                lineBuffer[logLine] = "Are you sure that you want to restore the last backup? (y/n)";
                refreshScreen();
                c = getch();
                if(c == 'y'){
                    std::filesystem::current_path(backupPath);
                    string latest = exec("ls -Art | tail -n 1");//gets the latest backup file
                    latest = latest.substr(0, latest.length()-1);//gets rid of the new line that is there for no reason
                    lineBuffer[logLine] = "Removing current World";
                    refreshScreen();
                    exec("rm -r " + serverPath + "/world");
                    doingWork = true;
                    std::thread restorAnimation(indefWaitingWithMessage, logLine, "Restoring World Files, Please Wait");
                    refreshScreen();
                    exec("mkdir " + serverPath + "/world");
                    exec("cp " + backupPath + "/" + latest + " " + serverPath + "/world");
                    std::filesystem::current_path(serverPath + "/world");
                    exec("7z x " + serverPath + "/world/" + latest);
                    exec("rm " + serverPath + "/world/" + latest);
                    doingWork = false;
                    restorAnimation.join();
                    lineBuffer[logLine] = "Backup Restored";
                    refreshScreen();
                } else{
                    lineBuffer[logLine] = "The backup will not be restored";
                }
            } else{
                lineBuffer[logLine] = "The server is online, please turn it off before restoring a backup";
            }
        } else if(c=='7'){
            if(serverOn()){
                exec("screen -S myserver -p 0 -X stuff 'kick @a Kicking all Players^M'");
                lineBuffer[logLine] = "All players have been kicked";
            } else{
                lineBuffer[logLine] = "The server is not on";
            }            
        } else if(c=='c'){
            for(int i=6;i<lineCount-1;i++){
            lineBuffer[i] = " ";
            }
            logLine = 6;
        } else if(c=='8'){
            changeConfig();
        }
        refreshScreen();
        logLine++;
    }
    system("tput rmcup");
    return 0;
}