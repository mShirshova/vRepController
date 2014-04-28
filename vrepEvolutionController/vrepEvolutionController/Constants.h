#ifndef CONSTANTS_H
#define	CONSTANTS_H 

#include <string>
using namespace std;

namespace Constants
{
	extern const double PI;

	//common network type
	extern const int RECCURENT_NETWORK_TYPE;

	//types of ready networks
	extern const int FEEDFORWARD_NETWORK_TYPE;
	extern const int FULLY_CONNECTED_NETWORK;
	extern const int JORDAN_SIMILAR_NETWORK;

	//types of neurons with differnt dynamics
	extern const int SIMPLE_NEURON;
	extern const int TIME_DELAY_NEURON;
	extern const int AR_NEURON;
	extern const int LEAKY_INTEGRATOR_NEURONS;

	extern const int INPUTE_NEURON_TYPE;
	extern const int INTERM_NEURON_TYPE;
	extern const int OUTPUT_NEURON_TYPE;

	//types of neurons transmission
	extern const int LOGISTIC_FUNCTION;
	extern const int THRESHOLD_FUNCTION;
	extern const int GAUSS_FUNCTION;
	extern const int LOG_AND_THRESHOLD_FUNCTION;
	extern const int SIN_FUNCTION;
	extern const int NONE_FUNCTION;

	extern const int SIMPLE_CONNECTION;
	extern const int AR_CONNECTION;

	extern const int LEFT_KEY;
	extern const int RIGHT_KEY;
	extern const int UP_KEY;
	extern const int DOWN_KEY;
	extern const int ONE_KEY;
	extern const int TWO_KEY;
	extern const int THREE_KEY;
	extern const int ENTER_KEY;

	//types of evolutionary algorithms
	extern const int GENETIC_ALG;
	extern const int EVOLUTIONARY_STRATEGY;
	//types of creation descendant
	extern const int CREATE_DESC_WITH_CROSSOVER;
	extern const int CREATE_DESC_MB;
	extern const int CREATE_DESC_MB_2;
	extern const int CREATE_DESC_FOR_HARMONIC_CONTROL;
	//..neat, aeg e.t.c

	extern const int FULL_DIM;
	extern const int MIDDLE_DIM;

	//types of fitnesses
	extern const int DISTANCE_FTNESS;
	extern const int ACTIVITY_FTNESS;
	extern const int MIX_FTNESS;
	extern const int HEIGHT_DISTANCE_FTNESS;

	//types of joint control
	extern const int COMPETITION_CONTROL;
	extern const int DELTA_COMPETTION_CONTROL;
	extern const int PROP_COMPETTION_CONTROL;
	extern const int ONE_NEURON_CONTROL;
	extern const int MULTIPLE_COMPETTITION_CONTROL;
	extern const int FORCE_COTROL;
	extern const int HARMONIC_CONTROL;

	//commands from main controller and to him (for inner logic, command main corresponde some count of events)
	extern const int INITIALIZE_CYCLE_MESSAGE;
	extern const int START_LIFE_MESSAGE;
	extern const int AGENT_IS_READY_MESSAGE;
	extern const int AGENT_LIFE_ENDED_MESSAGE;
	extern const int EXIT_PROCESS_MESSAGE;
	extern const int AGENT_PROCESS_CLEARED_MESSAGE;

	extern const string wbt_file_content;
	extern const string READ_NETWORK_AND_RESET;
	extern const string RESET_AND_STOP;
	extern const string AGENT_IS_READY;
	extern const string START_LIFE;
	extern const string AGENT_LIFE_ENDED;
	extern const string EXIT_PROCESS;
	extern const string AGENT_PROCESS_CLEARED;

	extern const string BASE_FILE_NAME;

	extern const int SHARED_MEMORY_SIZE;//100mb

	extern const int INT_SIZE;
	extern const int DOUBLE_SIZE;
	extern const int BOOL_SIZE;

	extern const int MAX_FITNESS_PLOT;
	extern const int AVERAGE_FITNESS_PLOT;
	extern const int MAX_NEURONS_PLOT;
	extern const int MIN_NEURONS_PLOT;
	extern const int AVERAGE_NEURONS_PLOT;
	extern const int MAX_CONNECTIONS_PLOT;
	extern const int MIN_CONNECTIONS_PLOT;
	extern const int AVERAGE_CONNECTIONS_PLOT;
	extern const int BEST_AGENT_NET_CONSISTENSY_PLOT;
	extern const int AVERAGE_NET_CONSISTENSY_PLOT;
	extern const int BEST_AGENT_RESULT_TIME_PLOT;
	extern const int AVERAGE_RESULT_TIME_PLOT;
	extern const int BEST_AGENT_AV_DELAY_TIME;
	extern const int AVERAGE_AV_DELAY_TIME;
	extern const int DISPERS_BEST_AGENT_AV_DELAY_TIME;
	extern const int DISPERS_OF_AVAREGE_AV_DELAY_TIME;

	extern const int STATISTIC_TYPE_GRAPH;
	extern const int JOINT_TYPE_GRAPH;
	extern const int NETWORK_TYPE_GRAPH;

	extern const int FITNESS_SAVE;
	extern const int DISTANCE_SAVE;
	extern const int ACTIVITY_SAVE;
	extern const int NET_CONSISTENCY_SAVE;
	extern const int NEURONS_NUM_SAVE;
	extern const int CONNECTIONS_NUM_SAVE;
	extern const int MAX_DELAY_SAVE;
	extern const int MIN_DELAY_SAVE;
	extern const int AV_DELAY_SAVE;
	extern const int DELAY_FOR_BEST_AG_SAVE;
	extern const int AV_TIME_OF_ACHIEVING_RESULT_SAVE;
	extern const int ACTIVATION_CONNECTION_NUM_SAVE;
	extern const int AR_CONNECTION_NUM_SAVE;

	extern const int READ_NETWORK_AND_PLAY_SHOW_MESS;
	extern const int EXIT_AGENT_SHOW_MESS;

	extern const int CALCULATION_MODE;
	extern const int SHOW_MODE;
	extern const int IDLE_MODE;

	extern const int ROULETTE_SEL_TYPE;
	extern const int RANDOM_SEL_TYPE;
};

#endif