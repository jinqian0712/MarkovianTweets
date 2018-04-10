#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <termios.h>
#include <unistd.h>
#include <climits>
#include "userError.h"
#include "tweetsdb.h"
#include "hashgen.h"

using namespace std;


void HideStdinKeystrokes()
{
    termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    /* disable echo */
    tty.c_lflag &= ~ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void ShowStdinKeystrokes()
{
   termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    /* reenable echo */
    tty.c_lflag |= ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

string getPassword(void){
    
    string password;
    
    HideStdinKeystrokes();
    getline(cin,password); 
    ShowStdinKeystrokes();
    
    return password;
}

bool checkLogin(string username, string password, Userinfo* user){
    
    bool loginSuccess=false;
    
    loginSuccess = isPasshashMatched(username,password);
    if(!loginSuccess){
       cout << endl << "\nERROR* password unmatched, please try again.\n" << endl;
    }else{
       user->userName = username;
    }
   
    return loginSuccess;
    
}

bool userAuth(unsigned char userSelection,Userinfo* user){
   
   bool login;
   string username;
   string userpassword;
   
   if((userSelection=='R')||(userSelection == 'r')){
      
      bool validProcess = false;
      login = false;
      
      while(!validProcess){
         try{
            cout << "SIGN_UP PAGE" <<endl;
            cout << "Enter Username(username length must between 4 to 20):";
            getline(cin,username);
            if((username.size()<4)||(username.size()>20)){
               throw usernameError();
            }
            for(int i=0;i<username.length();i++){
               if((!isalpha(username[i]))&&(!isdigit(username[i]))){
                  throw usernameError();
               }
            }
            if(isUserExist(username)){
               throw userExistError(username);
            }
            cout << "Enter Password(password length must between 4 to 16):";
            userpassword = getPassword();
            if((userpassword.size()<4)||(userpassword.size()>16)){
               throw userPasswordError(userpassword);
            }
            validProcess = true;
         }catch(usernameError e){
            cout << e.what() << "\n" << endl;
            cin.clear();
            validProcess = false;
         }catch(userPasswordError e){
            cout << e.what() << "\n" << endl;
            cin.clear();
            validProcess = false;
         }catch(userExistError e){
            cout << e.what() << "\n" << endl;
            cin.clear();
            validProcess = false;
         }
      }
      bool insertSuccess = false;
      insertSuccess = insertIntoDatabase(username,userpassword);
      
      if(insertSuccess){
         cout << endl
              << "\nYOUR ACCOUNT " << username << " ALREADY SET.\n"
              << endl;
      }

   }else{
      
      bool validProcess = false;
      
      while(!validProcess){
         try{
            cout << "LOGIN PAGE" <<endl;
            cout << "USERNAME:";
            getline(cin,username);
            if((username.size()<4)||(username.size()>20)){
               throw usernameError();
            }
            for(int i=0;i<username.length();i++){
               if((!isalpha(username[i]))&&(!isdigit(username[i]))){
                  throw usernameError();
               }
            }
            if(!isUserExist(username)){
               throw userNotExistError(username);
            }
            cout << "PASSWORD:";
            userpassword = getPassword();
            if((userpassword.size()<4)||(userpassword.size()>16)){
               throw userPasswordError(userpassword);
            }
            login = checkLogin(username,userpassword,user);
            if(login){
               validProcess = true;
            }
         }catch(usernameError e){
            cout << e.what() << "\n" << endl;
            cin.clear();
            validProcess = false;
         }catch(userPasswordError e){
            cout << e.what() << "\n" << endl;
            cin.clear();
            validProcess = false;
         }catch(userNotExistError e){
            cout << e.what() << "\n" << endl;
            cin.clear();
            validProcess = false;
         }
      }
   }
   
   return login;
}

bool welcomePage(Userinfo* user){
   
   bool login = false;
   bool userinputError;
   unsigned char userSelection;
   
   cout << "**********************************" 
        << endl
        << "*       WELCOME TO TWEETS        *"
        << endl
        << "**********************************"
        << endl;
   
   userinputError = true;
   while(userinputError){
      cout << endl << "R/r for Sign_up" 
           << endl << "S/s for Sign_in" 
           << endl << "Please Enter:";
      try{
         scanf("%c",&userSelection);
         switch(userSelection){
            case 'R':
            case 'r':
            case 'S':
            case 's':userinputError = false;
                     break;
            default: throw userSelectionError();
         }
      }catch(userSelectionError e){
         cout << "\n" << e.what() << "\n" << endl;
         cin.clear();
      }
      cin.ignore(INT_MAX,'\n');
      cin.clear();
   }
   
   login = userAuth(userSelection,user);
   
   return login;
}

bool userPage(Userinfo* user){
   
   bool login = true;
   bool invalidInput;
   unsigned char userSelection;
   
   cout << endl << "LOGIN SUCCESS! Welcome " << user->userName << "\n" << endl;
   user-> currentTweetsNum = 1;
   while(login){
      invalidInput = true;
      user-> totalTweets = getTweetsTotalNum(user->userName);
      if(user-> totalTweets!=0){
         displayTweets(user);
         while(invalidInput){
            try{
               cout << endl << "P/p for previous tweet.\n"
                            << "N/n for next tweet.\n"
                            << "W/w for write tweet.\n"
                            << "Q/q for logout.\n" <<endl;
               cout << "Enter:";
               scanf("%c",&userSelection);
               switch(userSelection){
                  case 'P':
                  case 'p':if(user->currentTweetsNum==1){
                              user->currentTweetsNum = user->totalTweets;
                           }else{
                              user->currentTweetsNum--;
                           }
                           break;
                  case 'N':
                  case 'n':if(user->currentTweetsNum==user->totalTweets){
                              user->currentTweetsNum = 1;
                           }else{
                              user->currentTweetsNum++;
                           }
                           break;
                  case 'W':
                  case 'w':writeTweets(user);user->currentTweetsNum=user->totalTweets;break;
                  case 'Q':
                  case 'q':login=false;break;
                  default: throw userSelectionError();
               }
               invalidInput = false;
            }catch(userSelectionError e){
               cout << "\n" << e.what() << "\n" << endl;
               cin.clear();
            }
            cin.ignore(INT_MAX,'\n');
            cin.clear();
         }
         
      }
      else{
         cout << endl << "You have no tweet yet." << endl; 
         while(invalidInput){
            try{
               cout << endl << "P/p for previous tweet.\n"
                            << "N/n for next tweet.\n"
                            << "W/w for write tweet.\n"
                            << "Q/q for logout.\n" <<endl;
               cout << "Enter:";
               scanf("%c",&userSelection);
               switch(userSelection){
                  case 'P':
                  case 'p':
                  case 'N':
                  case 'n': break;
                  case 'W':
                  case 'w':writeTweets(user);user->currentTweetsNum=user->totalTweets;break;
                  case 'Q':
                  case 'q':login=false;break;
                  default: throw userSelectionError();
               }
               invalidInput = false;
            }catch(userSelectionError e){
               cout << "\n" << e.what() << "\n" << endl;
               cin.clear();
            }
         }
      }
   }
   
   return login;
   
}

int main(int argc, char *argv[]){
   if(argc != 1){
      cout << "ERROR* system loading failed!" << endl;
      return 1;
   }
   
   Userinfo user;
   bool loginSuccess = false;
   
   while(1){
      if(!loginSuccess){
         loginSuccess = welcomePage(&user);
      }else{
         loginSuccess = userPage(&user);
      }
   }
   
   return 0;
}