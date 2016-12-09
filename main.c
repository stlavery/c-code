#include <stdio.h>
#include <stdlib.h>
#include "finite_sm_api.h"

// Enumerators
 enum States {
		POWER_OFF = 0,
		IDLE,
		READY_TO_PRINT,
		PRINTING,
		COMPLETE,
		ERROR
};

enum Transition {     // Must be cmd aligned with cmd input
		POWER_ON = 0,
		PREPARE_FOR_PRINT,
		START_PRINTING,
		PRINTING_COMPLETE,
		ERROR_RAISED,
		ERROR_CLEARED
};
	
// Structures
struct transitionRules {
    enum States currState;
	enum Transition transAction;
	enum States nextState;
};

// Items in column 0 MUST be grouped
struct transitionRules transitionLookup[] = {
/* 0 */	{POWER_OFF,			POWER_ON,			IDLE			},
/* 1 */	{IDLE,				PREPARE_FOR_PRINT,	READY_TO_PRINT	},
/* 2 */	{IDLE,				ERROR_RAISED,		ERROR			},
/* 3 */	{READY_TO_PRINT,	START_PRINTING,		PRINTING	    },
/* 4 */	{READY_TO_PRINT,	ERROR_RAISED,		ERROR			},
/* 5 */	{PRINTING,			PRINTING_COMPLETE,	COMPLETE		},			
/* 6 */	{PRINTING,			ERROR_RAISED,		ERROR			},
/* 7 */	{ERROR,				ERROR_CLEARED,		IDLE			},
/* 8 */	{END_OF_COLUMN,		END_OF_COLUMN,		END_OF_COLUMN	}
};

struct logMessage {
    int val;
    const char *msg;
};

struct logMessage logStates[] = {
        {0, "'Power Off'"},
        {1, "'Idle'"},
        {2, "'Ready to Print'"},
        {3, "'Printing'"},
        {4, "'Complete'"},
        {5, "'Error'"},
};

struct logMessage logTransitions[] = {
        {0, "'Power On'"},
        {1, "'Prepare for Print'"},
        {2, "'Start Printing'"},
        {3, "'Printing Complete'"},
        {4, "'Error Raised'"},
        {5, "'Error Cleared'"}
};

void userMenu(void) {
	printf("'o' - Power On\n");
	printf("'p' - Prepare for Print\n");
	printf("'s' - Start Printing\n");
	printf("'c' - Printing Complete\n");
	printf("'e' - Error Raised\n");
	printf("'r' - Error Cleared\n");
}

int onEntry() {
	// some undefined logic will go here
#ifdef VERBOSE_LOGGING
	printf("Printer-API: Entering state (not implemented)\n");
#endif	
	return 0;
}

int onExit() {
	// some undefined logic will go here
#ifdef VERBOSE_LOGGING
	printf("Printer-API: Exiting state (not implemented)\n");
#endif	
	return 0;
}

int processTransition(struct stateMachine * stateMachine, struct transition * transition) {
	int i=0,j=0,count=0; //Loop vars

#ifdef VERBOSE_LOGGING
	printf("Printer-SM:    currState:   %s\n", logStates[(int)(stateMachine->currState)].msg);
	printf("Printer-Trans: transAction: %s\n", logTransitions[(int)(transition->trans)].msg);
#endif

	// Discover number of transitions possible from this state
	for(i=0;i<END_OF_COLUMN;i++) {
		if(transitionLookup[i].currState == (int)(stateMachine->currState)) {
			count++;
		}
	}
	// Find the next state from the table
	if(count != 0) {
		for(i=0;i<END_OF_COLUMN;i++) { 
			if(transitionLookup[i].currState == (int)(stateMachine->currState)) {
				//printf("i %d\n", i);
				for(j=i;j<(count+i);j++) {
					//printf("j %d\n", j);
					if(transitionLookup[j].transAction == (int)(transition->trans)) {
	
						// Exit the current state
						onExit();

						//Found the next state, set current state to it
						(int)stateMachine->currState = transitionLookup[j].nextState;
						printf("Printer-SM:    nextState:   %s\n", logStates[(int)(stateMachine->currState)].msg);
						
						//re-initialize next state
						transition->nextState = NULL;			
				
						return 0;
					}
				}
				return 1;
			}
		}
	}
	else {
		printf("Command %s not found\n", logTransitions[(int)(transition->trans)].msg);
		return 1;
	}  

	return 0;
}

// Main Loop
int main(int argc, char const *argv[]) {
    char cmd;
	int trans = UNSET;
	struct transition smTrans;			// Create a transition obj
	struct stateMachine sm;				// Create a stateMachine obj

	(int)sm.initState = POWER_OFF;		// Always init to this state 
	(int)sm.currState = POWER_OFF;
	
	system ("cls");						// Clear screen
	do {
		userMenu();						// Displaying the Menu
		// Usage
		printf("Enter command: ");
	    scanf_s("%c", &cmd);

		// Map the user input to a transition enum
		switch (cmd)
        {
			case 'o':
				trans = POWER_ON;	
				break;
			case 'p':
				trans = PREPARE_FOR_PRINT;             
				break;
            case 's':
				trans = START_PRINTING;
				break;
            case 'c':
				trans = PRINTING_COMPLETE; 
				break;
			case 'r':
				trans = ERROR_CLEARED; 
				break;
			case 'e':
				trans = ERROR_RAISED;
				break;
			default:
				printf("Unknown command, press Enter");
				trans = UNSET;
				break;
		}
		if(trans != UNSET) {     

			// Given current state and user command, look-up next state
			(int)smTrans.trans = trans;
			if(processTransition(&sm, &smTrans)) {
				printf("Printer-SM: Cannot transition via %s from %s\n", logTransitions[trans].msg, logStates[(int)sm.currState].msg);
			}
			else {
				// Enter the state
				onEntry();

				if((int)sm.currState == COMPLETE) {
					// If the new state is Complete, return to idle without a command
					printf("Printer-SM: Printing complete, transitioning back to idle\n");
					
					// Exit the current state
					onExit();

					// Clearing the state parameters
					(int)smTrans.trans = (int)NULL;
					(int)sm.currState = IDLE;
					processTransition(&sm, &smTrans);
										
				}
			}
		}
	} while(getchar() != EOF);
		
    return 0;

}