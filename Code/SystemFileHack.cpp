#include <bits/stdc++.h>
using namespace std;
int main()
{
    ifstream Temp("/etc/passwd");
    if (!Temp.is_open())
        exit(errno);
    string s;
    getline(Temp, s);
    cout << s << endl;
    return 0;
}
