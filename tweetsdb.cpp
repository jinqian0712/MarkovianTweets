#include "tweetsdb.h"
#include <cstring>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <climits>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <vector>
#include "userError.h"
#include "hashgen.h"

using namespace std;

static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    string* username = (string*)data;
    for(i=0; i<argc; i++){
        (*username) += argv[i];
    }
    
    return 0;
}

static int callbacknum(void *data, int argc, char **argv, char **azColName){
    
    int* num = (int*)data;
    istringstream (argv[0]) >> *num;
    return 0;
}

static int callbacknull(void *NotUsed, int argc, char **argv, char **azColName){
    return 0;
}

int sqlOperation(char* sqlCode,string* user){
    
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    
    rc = sqlite3_open("tweets.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    };
    
    if(user == NULL){
        rc = sqlite3_exec(db, sqlCode, callbacknull, 0, &zErrMsg);
    }else{
        rc = sqlite3_exec(db, sqlCode, callback, user, &zErrMsg);
    }
    
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }  
    sqlite3_close(db);
    return 0;
}

bool isUserExist(string username){
    
    char *sqlCode = sqlite3_mprintf("select username from useraccount where username = '%q';",username.c_str());
    
    bool isExist;
    string user;
    
    sqlOperation(sqlCode,&user);
    if(user != username){
        isExist = false;
    }else{
        isExist = true;
    }
    
    return isExist;
}

bool insertIntoDatabase(string username,string userpassword){
    
    string salt;
    salt = userSaltGenertator();
    string passwordHash = generatePasswordHash(username, userpassword, salt);
    
    char *sqlCode = sqlite3_mprintf("insert into useraccount (username,passwordhash,salt) values ('%q','%q','%q');",username.c_str(),passwordHash.c_str(),salt.c_str());
    sqlOperation(sqlCode, NULL);
    sqlCode = sqlite3_mprintf("insert into tweetslist (username,tweetsnum) values ('%q',0);",username.c_str());
    sqlOperation(sqlCode, NULL);
    
    bool inserted = true;
    
    return inserted;
}

bool isPasshashMatched(string username,string password){
    
    bool isMatch = false;
    string passHash;
    string userSalt;
    string userHash;
    
    char *sqlCode = sqlite3_mprintf("select salt from useraccount where username = '%q';",username.c_str());
    sqlOperation(sqlCode, &userSalt);
    userHash = generatePasswordHash(username,password,userSalt);
    sqlCode = sqlite3_mprintf("select passwordhash from useraccount where username = '%q';",username.c_str());
    sqlOperation(sqlCode, &passHash);
    if(passHash == userHash){
        isMatch = true;
    }else{
        isMatch = false;
    }
    
    return isMatch;
    
}

unsigned int getTweetsTotalNum(string username){
    
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    unsigned int total = 0;
    char *sqlCode = sqlite3_mprintf("select max(tweetsnum) from tweetslist where username = '%q';",username.c_str());
    
    rc = sqlite3_open("tweets.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    
    rc = sqlite3_exec(db, sqlCode, callbacknum, &total, &zErrMsg);
    
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }  
    sqlite3_close(db);
    
    return total;
    
}


void displayTweets(Userinfo* user){
    
    char* sqlCode;
    ostringstream s_num;
    s_num << user->currentTweetsNum;
    string num;
    string date;
    num = s_num.str();
    string username = user->userName;
    string tweets;
    
    cout << "user: " << user->userName << endl;
    cout << "tweets number: " << user->currentTweetsNum << endl;
    sqlCode = sqlite3_mprintf("select date from tweetslist where username = '%q' and tweetsnum = %q;",username.c_str(),num.c_str());
    sqlOperation(sqlCode,&date);
    cout << "tweets date: " << date << endl;
    
    sqlCode = sqlite3_mprintf("select tweets from tweetslist where username = '%q' and tweetsnum = %q;",username.c_str(),num.c_str());
    sqlOperation(sqlCode,&tweets);
    
    unsigned int len = tweets.length();
    string originalTweets;
    for(int i=0; i< len; i+=2)
    {
        string byte = tweets.substr(i,2);
        char chr = (char) (int)strtol(byte.c_str(), NULL, 16);
        originalTweets.push_back(chr);
    }
    
    cout << originalTweets << endl;
}

void writeTweets(Userinfo* user){
   
    char* sqlCode;
    bool sendSuccess = false;
    string message;
    string mesgLine;
    ostringstream hexIt;
    string hexMesg;
    unsigned int index;
    bool checkEnd = true;
    
    cout << "Use 'enter+:q!+enter' to end writing.\n" << endl;
    
    string messOriginal;
    while(checkEnd){
        getline(cin,mesgLine);
        messOriginal.append("\n");
        if(mesgLine == ":q!"){
            checkEnd = false;
        }else{
            messOriginal.append(mesgLine);
            checkEnd = true;
            //message.append(hexMesg);
        }
    }
    for(index=0;index<messOriginal.length();index++){
        hexIt << hex << setfill('0') << setw(2) << (int)messOriginal[index];
    }
    hexMesg = hexIt.str();
    message.append(hexMesg);
    
    time_t rawtime; 
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,sizeof(buffer),"%d-%m-%Y %I:%M:%S",timeinfo);
    string date(buffer);
    
    user->totalTweets++;
    ostringstream s_num;
    s_num << user->totalTweets;
    string num;
    num = s_num.str();
    string username = user->userName;
    
    sqlCode = sqlite3_mprintf("insert into tweetslist (username,tweetsnum,date,tweets) values ('%q',%q,'%q','%q');",username.c_str(),num.c_str(),date.c_str(),message.c_str());
    sqlOperation(sqlCode, NULL);
    
    cout << "Press ENTER to continue .." << endl;
   
}