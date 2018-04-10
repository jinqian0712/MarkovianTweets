/*This is a header file, tweetsdb.h.
  It will be used for dealing with the geemail database.
*/

#include <string>
#include <vector>


using namespace std;

struct Userinfo {
    public:
    string userName;
    unsigned int totalTweets;
    unsigned int currentTweetsNum;
};

bool isUserExist(string username);
bool insertIntoDatabase(string username,string userpassword);
bool isPasshashMatched(string username,string password);
unsigned int getTweetsTotalNum(string username);
void displayTweets(Userinfo* user);
void writeTweets(Userinfo* user);


