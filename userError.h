/*This is a header file, userError.h
  This is a file which declare exceptions
*/
#include <stdexcept>
#include <string>

using namespace std;

class userSelectionError: public exception {
    public:
    userSelectionError(){
        cout << endl << "INFO* try to enter a wrong value" << endl; 
    }
    const char* what(){
        return "ERROR* got invalid input.";
    }
};

class usernameError: public exception {
   public:
   usernameError(){
       cout << endl << "INFO* try to use invalid username"<< endl;
   }
   const char* what(){
        return "ERROR* invalid username, length must between 4 to 20, special char not allowed.";
   }
};

class userExistError: public exception {
   public:
   userExistError(string userName){
       cout << endl
            << "INFO* the username "
            << userName 
            << " already taken." << endl;
   }
   const char* what(){
        return "ERROR* username already taken, try another one.";
   }
};

class userPasswordError: public exception {
   private:
   size_t passwordLength;
   public:
   userPasswordError(string password){
       passwordLength = password.size();
   }
   const char* what(){
       cout << endl
            << "INFO* try to use invalid password length "
            << passwordLength << endl;
        return "ERROR* wrong password length, length must between 4 to 16.";
   }
};

class userNotExistError: public exception {
   public:
   userNotExistError(string userName){
       cout << endl
            << "INFO* Cannot find the username "
            << userName  << "." << endl;
   }
   const char* what(){
        return "ERROR* username not exit, try another one.";
   }
};