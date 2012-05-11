#include <iostream>
#include <libplayerc++/playerc++.h>

using namespace std;
using namespace PlayerCc;

int main(int argc, char **argv) {
	PlayerClient client("localhost", 6665);
	LaserProxy laser(&client,0);

	while(1){
		client.Read();
		for (int i = 0; i < 271; ++i) {
			cout << laser[i] << endl;
		}
	}
}
