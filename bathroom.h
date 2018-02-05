// Kyle Savell & Antony Qin
// Project 3: Bathroom

enum gender = {male, female};
enum filled_with = {vacant, males, females};
void Enter(gender g); //Enter bathroom, set state if needed
void Leave(void); //Leave bathroom, set state if needed
void Initialize();
void Finalize();

struct args {
	enum gender person;
	int arrival;
	int stay;
	int loops;
};
struct monitor {
	int occupants[100];
	enum filled_with status;
};
