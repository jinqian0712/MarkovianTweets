/*This is a header file, hashgen.h.
  It will be used for hash code generate
*/
#include <string>
using namespace std;

string hashGenerator(string fileLine, size_t txtLength); 
string generatePasswordHash(string username, string password, string passSalt);
string userSaltGenertator(void);