// Defines
//#define END_OF_TRANSITION_LIST (struct transition) {cmd = NULL, nextState = NULL}
#define END_OF_COLUMN   (8)
#define UNSET		    (100)
//#define LOGGING_ON	// Un-comment for debugging
#ifdef LOGGING_ON
#define VERBOSE_LOGGING
#endif

// Structure definitions
struct state {
    int name;
    struct transition * transition;
    void (*onEntry)(void);
	void (*onExit)(void);
};

struct transition {
    int trans;
    struct state * nextState;
};

struct stateMachine {
    struct state * initState;
    struct state * currState;
    char * name;
};

// Function Prototypes
int onEntry(void);
int onExit(void);
void userMenu(void);
int processTransition(struct stateMachine * stateMachine, struct transition * transition);
